// Copyright 2013 Olivier Gillet..
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Signal generator.

#include "module_tester/signal_generator.h"

#include <avr/pgmspace.h>

#include "avrlib/gpio.h"
#include "avrlib/op.h"

#include "module_tester/resources.h"

namespace module_tester {

using namespace avrlib;

/* static */
SignalGeneratorSettings SignalGenerator::data_;

/* static */
SignalGeneratorState SignalGenerator::state_;

/* static */
uint16_t SignalGenerator::cv_samples_[kCvBlockSize];

/* static */
avrlib::RingBuffer<AudioBufferSpecs> SignalGenerator::audio_buffer_;

/* static */
avrlib::RingBuffer<CvBufferSpecs> SignalGenerator::cv_buffer_;

/* static */
uint16_t SignalGenerator::pitch_bend_;

/* static */
uint8_t SignalGenerator::modulation_;

/* static */
uint16_t SignalGenerator::last_note_value_;

/* static */
NoteStack<10> SignalGenerator::note_stack_;

/* extern */
SignalGenerator signal_generator;

typedef SignalGeneratorSettings PROGMEM prog_SignalGeneratorSettings;

const prog_SignalGeneratorSettings init_settings PROGMEM = {
  120, CLOCK_RESOLUTION_24_PPQN, PULSE_DURATION_5_MS, false,
  PERIOD_1_S, PULSE_DURATION_50_PERCENT, GATE_MIDI_MODE_OFF, 0,
  CV_MODE_TRIANGLE_LFO, PERIOD_1_S, CV_RANGE_PM_5V, CV_MIDI_MODE_OFF,
  AUDIO_MODE_SINE, 69, AUDIO_ENVELOPE_MODE_OFF, AUDIO_MIDI_MODE_OFF,
};

/* static */
void SignalGenerator::Init() {
  memcpy_P(&data_, &init_settings, sizeof(SignalGeneratorSettings));
  note_stack_.Init();
  pitch_bend_ = 8192;
  modulation_ = 0;
}

/* static */
void SignalGenerator::Render() {
  Gpio<PortD, 7>::set_mode(DIGITAL_OUTPUT);
  
  // Render some audio samples.
  while (audio_buffer_.writable() >= kAudioBlockSize) {
    Gpio<PortD, 7>::High();
    uint8_t frequency = data_.audio.frequency;
    if (data_.audio.midi_mode == AUDIO_MIDI_MODE_NOTE ||
        data_.audio.midi_mode == AUDIO_MIDI_MODE_ON) {
      if (note_stack_.size()) {
        state_.last_midi_note = note_stack_.most_recent_note().note;
        if (state_.last_midi_note < 12) {
          state_.last_midi_note = 12;
        } else if (state_.last_midi_note > 108) {
          state_.last_midi_note = 108;
        }
      }
      frequency = state_.last_midi_note + 9 - 12;
    }
    state_.audio_phase_increment = pgm_read_dword(
        lut_audio_phase_increment + frequency);
    state_.audio_midi_note = pgm_read_word(lut_audio_midi_note + frequency);
    AudioRenderFn render_fn;
    memcpy_P(&render_fn, fn_table_ + data_.audio.mode, sizeof(AudioRenderFn));
    (*render_fn)();
    Gpio<PortD, 7>::Low();
  }
  
  // Render some CV samples.
  while (cv_buffer_.writable() >= kCvBlockSize) {
    UpdateIncrements();
    RenderCv();
    RenderClock();
    RenderGate();
    for (uint8_t i = 0; i < kCvBlockSize; ++i) {
      cv_buffer_.Overwrite(cv_samples_[i]);
    }
  }
}

/* static */
void SignalGenerator::UpdateIncrements() {
  // Clock.
  uint32_t increment = pgm_read_dword(
      lut_tempo_phase_increment + data_.clock.tempo - 40);
  state_.clock_pulse_duration = pgm_read_word(
      lut_pulse_duration + data_.clock.pulse_duration);
  state_.clock_pulse_ratio = pgm_read_word(
      lut_pulse_ratio + data_.clock.pulse_duration);
  switch (data_.clock.resolution) {
    case CLOCK_RESOLUTION_1_PPQN:
      state_.clock_phase_increment = increment >> 3;
      break;
    case CLOCK_RESOLUTION_4_PPQN:
      state_.clock_phase_increment = increment >> 1;
      break;
    case CLOCK_RESOLUTION_8_PPQN:
      state_.clock_phase_increment = increment;
      break;
    case CLOCK_RESOLUTION_24_PPQN:
      state_.clock_phase_increment = (increment << 1) + increment;
      break;
  }
  if (data_.clock.midi_mode) {
    if (!state_.clock_pulse_duration) {
      state_.clock_pulse_duration = pgm_read_word(lut_pulse_duration);
    }
    state_.clock_prescaler = pgm_read_word(
        lut_clock_prescaler + data_.clock.resolution);
  }
  
  // Gate.
  state_.gate_phase_increment = pgm_read_dword(
      lut_gate_phase_increment + data_.gate.period);
  state_.gate_pulse_duration = pgm_read_word(
      lut_pulse_duration + data_.gate.pulse_duration);
  state_.gate_pulse_ratio = pgm_read_word(
      lut_pulse_ratio + data_.gate.pulse_duration);
  if (data_.gate.midi_mode && !state_.gate_pulse_duration) {
    state_.gate_pulse_duration = pgm_read_word(lut_pulse_duration);
  }
  
  // CV.
  state_.cv_phase_increment = pgm_read_dword(
      lut_cv_phase_increment + data_.cv.period);
}

/* static */
void SignalGenerator::RenderClock() {
  if (data_.clock.midi_mode) {
    return;
  }
  LongWord clock_phase;
  clock_phase.value = state_.clock_phase;
  uint32_t increment = state_.clock_phase_increment;
  for (uint8_t i = 0; i < kCvBlockSize; i++) {
    clock_phase.value += increment;
    if (clock_phase.value < increment) {
      RaiseClockPulse();
    }
    if (DecreaseClockPulse()) {
      cv_samples_[i] |= 0x8000;
    }
    if (clock_phase.words[1] < state_.clock_pulse_ratio) {
      cv_samples_[i] |= 0x8000;
    }
  }
  state_.clock_phase = clock_phase.value;
}

/* static */
void SignalGenerator::RenderGate() {
  LongWord gate_phase;
  gate_phase.value = state_.gate_phase;
  
  for (uint8_t i = 0; i < kCvBlockSize; ++i) {
    if (!data_.gate.midi_mode) {
      gate_phase.value += state_.gate_phase_increment;
      if (gate_phase.value < state_.gate_phase_increment) {
        state_.gate_pulse = state_.gate_pulse_duration;
      }
      if (gate_phase.words[1] < state_.gate_pulse_ratio) {
        cv_samples_[i] |= 0x4000;
      }
    } else if (data_.gate.midi_mode == GATE_MIDI_MODE_GATE) {
      if (note_stack_.size()) {
        cv_samples_[i] |= 0x4000;
      }
    }
    if (state_.gate_pulse) {
      cv_samples_[i] |= 0x4000;
      --state_.gate_pulse;
    }
  }
  state_.gate_phase = gate_phase.value;
}

const prog_uint16_t cv_range_scale[] PROGMEM = { 667, 1333, 3333, 333, 667, 1667 };
const prog_uint16_t cv_range_offset[] PROGMEM = { 2048, 2048, 2048, 1881, 1715, 1215 };
const prog_uint16_t chromatic_scale[] PROGMEM = {
    1715, 1687, 1659, 1631, 1604, 1576,
    1548, 1520, 1492, 1465, 1437, 1409, 1381 };

/* static */
void SignalGenerator::RenderCv() {
  uint16_t scale = pgm_read_word(cv_range_scale + data_.cv.range);
  uint16_t offset = pgm_read_word(cv_range_offset + data_.cv.range);
  
  if (data_.cv.midi_mode) {
    uint16_t value = 0;
    switch (data_.cv.midi_mode) {
      case CV_MIDI_MODE_NOTE:
        {
          if (note_stack_.size()) {
            int16_t note = note_stack_.most_recent_note().note;
            value = 2048 - S16U16MulShift16((note - 36) << 8, 7111);
            last_note_value_ = value;
          } else {
            value = last_note_value_;
          }
        }
        break;
      case CV_MIDI_MODE_VELOCITY:
        if (note_stack_.size()) {
          value = note_stack_.most_recent_note().velocity << 9;
        } else {
          value = 0;
        }
        break;
      case CV_MIDI_MODE_PITCH_BEND:
        value = pitch_bend_ << 2;
        break;
      case CV_MIDI_MODE_MOD_WHEEL:
        value = modulation_ << 9;
        break;
    }
    if (data_.cv.midi_mode != CV_MIDI_MODE_NOTE) {
      value = S16U16MulShift16(32767 - value, scale) + offset;
    }
    for (uint8_t i = 0; i < kCvBlockSize; ++i) {
      cv_samples_[i] = value;
    }
  } else {
    LongWord cv_phase;
    cv_phase.value = state_.cv_phase;
    for (uint8_t i = 0; i < kCvBlockSize; ++i) {
      cv_phase.value += state_.cv_phase_increment;
      uint16_t value = cv_phase.words[1];
      switch (data_.cv.mode) {
        case CV_MODE_TRIANGLE_LFO:
          value = (value & 0x8000) ? 65535 - (value << 1) : (value << 1);
          break;
        case CV_MODE_SQUARE_LFO:
          value = (value & 0x8000) ? 0xffff : 0x000;
          break;
        case CV_MODE_RAMP_UP_LFO:
          break;
        case CV_MODE_RAMP_DOWN_LFO:
          value = (~value);
          break;
        case CV_MODE_SINE_LFO:
          {
            uint8_t integral = cv_phase.bytes[3];
            uint8_t fractional = cv_phase.bytes[2];
            uint16_t a = pgm_read_word(lut_sine + integral);
            uint16_t b = pgm_read_word(lut_sine + integral + 1);
            value = a + S16U8MulShift8(b - a, fractional);
          }
          break;
        case CV_MODE_1_OCTAVE_ARP:
          {
            value = (value & 0x8000) ? 1715 : 1381;
          }
          break;
        case CV_MODE_2_OCTAVE_ARP:
          {
            value = (value & 0x8000) ? 1715 : 1048;
          }
          break;
        case CV_MODE_CHROMATIC_SCALE_ARP:
          {
            uint8_t index = U8U8MulShift8(cv_phase.bytes[3], 13);
            value = pgm_read_word(chromatic_scale + index);
          }
          break;
        case CV_MODE_C1_NOTE:
          value = 1715;
          break;
        case CV_MODE_C3_NOTE:
          value = 1048;
          break;
      }
      if (data_.cv.mode < CV_MODE_1_OCTAVE_ARP) {
        value = S16U16MulShift16(32767 - value, scale) + offset;
      }
      cv_samples_[i] = value;
    }
    state_.cv_phase = cv_phase.value;    
  }
}

/* static */
void SignalGenerator::RenderAudioBandLimited() {
  int8_t note = state_.audio_midi_note;
  if (note < 12) {
    note = 12;
  }
  uint8_t balance_index = U8Swap4(note - 12);
  uint8_t gain_2 = balance_index & 0xf0;
  uint8_t gain_1 = ~gain_2;
  uint8_t wave_index = balance_index & 0xf;
  uint8_t base_resource_id = data_.audio.mode == AUDIO_MODE_SAW ?
      WAV_BANDLIMITED_SAW_0 :
      (data_.audio.mode == AUDIO_MODE_SQUARE ? WAV_BANDLIMITED_SQUARE_0  : 
      WAV_BANDLIMITED_TRIANGLE_0);

  const prog_uint8_t* wave_1 = waveform_table[base_resource_id + wave_index];
  wave_index = U8AddClip(wave_index, 1, 6);
  const prog_uint8_t* wave_2 = waveform_table[base_resource_id + wave_index];
  
  uint32_t phase = state_.audio_phase;
  uint32_t increment = state_.audio_phase_increment;
  uint16_t envelope_phase = state_.audio_envelope_phase;
  uint8_t count = kAudioBlockSize;
  
  if (state_.trigger_count) {
    state_.trigger_count = 0;
    envelope_phase = 0;
  }
  uint16_t amplitude = 4095;
  if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_GATE
      && !state_.gate_state) {
    amplitude = 0;
  }
  if (data_.audio.midi_mode == AUDIO_MIDI_MODE_ON ||
      data_.audio.midi_mode == AUDIO_MIDI_MODE_GATE) {
    amplitude = note_stack_.size() ? 4095 : 0;
  }
  
  while (count--) {
    phase += increment;
    LongWord phi;
    phi.value = phase;
    
    envelope_phase += 4;
    if (envelope_phase < 4) {
      envelope_phase = 0xffff;
    }
    
    uint16_t sample = U8U8Mul(InterpolateSample(wave_1, phi.words[1]), gain_1);
    sample += U8U8Mul(InterpolateSample(wave_2, phi.words[1]), gain_2);
    
    if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_TRIGGER) {
      Word envelope_phi;
      envelope_phi.value = envelope_phase;

      uint16_t a = pgm_read_word(lut_envelope + envelope_phi.bytes[1]);
      uint16_t b = pgm_read_word(lut_envelope + envelope_phi.bytes[1] + 1);
      amplitude = a + S16U8MulShift8(b - a, envelope_phi.bytes[0]);
    }
    audio_buffer_.Overwrite(2048 + S16U16MulShift16(sample + 32768, amplitude));
  }
  state_.audio_phase = phase;
  state_.audio_envelope_phase = envelope_phase;
}

/* static */
void SignalGenerator::RenderAudioNoise() {
  uint32_t phase = state_.audio_phase;
  uint8_t count = kAudioBlockSize;
  uint16_t envelope_phase = state_.audio_envelope_phase;
  
  if (state_.trigger_count) {
    state_.trigger_count = 0;
    envelope_phase = 0;
  }
  
  uint16_t amplitude = 4095;
  if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_GATE
      && !state_.gate_state) {
    amplitude = 0;
  }
  if (data_.audio.midi_mode == AUDIO_MIDI_MODE_ON ||
      data_.audio.midi_mode == AUDIO_MIDI_MODE_GATE) {
    amplitude = note_stack_.size() ? 4095 : 0;
  }
  
  while (count--) {
    phase = phase * 1664525L + 1013904223L;
    envelope_phase += 4;
    if (envelope_phase < 4) {
      envelope_phase = 0xffff;
    }
    if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_TRIGGER) {
      Word envelope_phi;
      envelope_phi.value = envelope_phase;
      uint16_t a = pgm_read_word(lut_envelope + envelope_phi.bytes[1]);
      uint16_t b = pgm_read_word(lut_envelope + envelope_phi.bytes[1] + 1);
      amplitude = a + S16U8MulShift8(b - a, envelope_phi.bytes[0]);
    }
    audio_buffer_.Overwrite(2048 + S16U16MulShift16(phase, amplitude));
  }
  state_.audio_phase = phase;
  state_.audio_envelope_phase = envelope_phase;
}

/* static */
void SignalGenerator::RenderAudioSine() {
  uint32_t phase = state_.audio_phase;
  uint32_t increment = state_.audio_phase_increment;
  uint8_t count = kAudioBlockSize;
  uint16_t envelope_phase = state_.audio_envelope_phase;
  
  if (state_.trigger_count) {
    state_.trigger_count = 0;
    envelope_phase = 0;
  }
  
  uint16_t amplitude = 4095;
  if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_GATE
      && !state_.gate_state) {
    amplitude = 0;
  }
  if (data_.audio.midi_mode == AUDIO_MIDI_MODE_ON ||
      data_.audio.midi_mode == AUDIO_MIDI_MODE_GATE) {
    amplitude = note_stack_.size() ? 4095 : 0;
  }
  
  while (count--) {
    phase += increment;
    LongWord phi;
    phi.value = phase;
    
    envelope_phase += 4;
    if (envelope_phase < 4) {
      envelope_phase = 0xffff;
    }
    
    uint16_t a = pgm_read_word(lut_sine + phi.bytes[3]);
    uint16_t b = pgm_read_word(lut_sine + phi.bytes[3] + 1);
    uint16_t sample = a + S16U8MulShift8(b - a, phi.bytes[2]);
    
    if (data_.audio.envelope_mode == AUDIO_ENVELOPE_MODE_TRIGGER) {
      Word envelope_phi;
      envelope_phi.value = envelope_phase;

      a = pgm_read_word(lut_envelope + envelope_phi.bytes[1]);
      b = pgm_read_word(lut_envelope + envelope_phi.bytes[1] + 1);
      amplitude = a + S16U8MulShift8(b - a, envelope_phi.bytes[0]);
    }
    audio_buffer_.Overwrite(2048 + S16U16MulShift16(sample + 32768, amplitude));
  }
  state_.audio_phase = phase;
  state_.audio_envelope_phase = envelope_phase;
}

/* static */
void SignalGenerator::NoteOn(uint8_t note, uint8_t velocity) {
  note_stack_.NoteOn(note, velocity);
  if (data_.gate.midi_mode == GATE_MIDI_MODE_TRIGGER) {
    state_.gate_pulse = state_.gate_pulse_duration;
  }
}

/* static */
void SignalGenerator::NoteOff(uint8_t note) {
  note_stack_.NoteOff(note);
}


/* static */
const SignalGenerator::AudioRenderFn SignalGenerator::fn_table_[] PROGMEM = {
  &SignalGenerator::RenderAudioBandLimited,
  &SignalGenerator::RenderAudioBandLimited,
  &SignalGenerator::RenderAudioBandLimited,
  &SignalGenerator::RenderAudioSine,
  &SignalGenerator::RenderAudioNoise,
};

}  // namespace module_tester
