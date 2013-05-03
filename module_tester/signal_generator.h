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

#ifndef MODULE_TESTER_SIGNAL_GENERATOR_H_
#define MODULE_TESTER_SIGNAL_GENERATOR_H_

#include "avrlib/base.h"
#include "avrlib/ring_buffer.h"

#include "module_tester/note_stack.h"

namespace module_tester {

enum ClockResolution {
  CLOCK_RESOLUTION_1_PPQN,
  CLOCK_RESOLUTION_4_PPQN,
  CLOCK_RESOLUTION_8_PPQN,
  CLOCK_RESOLUTION_24_PPQN
};

enum PulseDuration {
  PULSE_DURATION_1_MS,
  PULSE_DURATION_2_MS,
  PULSE_DURATION_5_MS,
  PULSE_DURATION_10_MS,
  PULSE_DURATION_5_PERCENT,
  PULSE_DURATION_10_PERCENT,
  PULSE_DURATION_20_PERCENT,
  PULSE_DURATION_50_PERCENT,
};

enum Period {
  PERIOD_10_MS,
  PERIOD_20_MS,
  PERIOD_50_MS,
  PERIOD_100_MS,
  PERIOD_200_MS,
  PERIOD_500_MS,
  PERIOD_1_S,
  PERIOD_2_S,
  PERIOD_5_S,
  PERIOD_10_S
};

enum GateMidiMode {
  GATE_MIDI_MODE_OFF,
  GATE_MIDI_MODE_GATE,
  GATE_MIDI_MODE_TRIGGER,
};

enum CvMode {
  CV_MODE_TRIANGLE_LFO,
  CV_MODE_SQUARE_LFO,
  CV_MODE_RAMP_UP_LFO,
  CV_MODE_RAMP_DOWN_LFO,
  CV_MODE_SINE_LFO,
  CV_MODE_1_OCTAVE_ARP,
  CV_MODE_2_OCTAVE_ARP,
  CV_MODE_CHROMATIC_SCALE_ARP,
};

enum CvRange {
  CV_RANGE_PM_1V,
  CV_RANGE_PM_2V,
  CV_RANGE_PM_5V,
  CV_RANGE_0_1V,
  CV_RANGE_0_2V,
  CV_RANGE_0_5V
};

enum CvMidiMode {
  CV_MIDI_MODE_OFF,
  CV_MIDI_MODE_NOTE,
  CV_MIDI_MODE_VELOCITY,
  CV_MIDI_MODE_PITCH_BEND,
  CV_MIDI_MODE_MOD_WHEEL,
};

enum AudioMode {
  AUDIO_MODE_SAW,
  AUDIO_MODE_SQUARE,
  AUDIO_MODE_TRIANGLE,
  AUDIO_MODE_SINE,
};

enum AudioEnvelopeMode {
  AUDIO_ENVELOPE_MODE_OFF,
  AUDIO_ENVELOPE_MODE_GATE,
  AUDIO_ENVELOPE_MODE_TRIGGER,
};

enum AudioMidiMode {
  AUDIO_MIDI_MODE_OFF,
  AUDIO_MIDI_MODE_NOTE,
  AUDIO_MIDI_MODE_GATE,
  AUDIO_MIDI_MODE_ON,
};

struct ClockSettings {
  uint8_t tempo;
  ClockResolution resolution;
  PulseDuration pulse_duration;
  bool midi_mode;
};

struct GateSettings {
  Period period;
  PulseDuration pulse_duration;
  GateMidiMode midi_mode;
  uint8_t padding;
};

struct CvSettings {
  CvMode mode;
  Period period;
  CvRange range;
  CvMidiMode midi_mode;
};

struct AudioSettings {
  AudioMode mode;
  uint8_t frequency;
  AudioEnvelopeMode envelope_mode;
  AudioMidiMode midi_mode;
};

enum SignalGeneratorParameter {
  SG_PRM_CLOCK_TEMPO,
  SG_PRM_CLOCK_RESOLUTION,
  SG_PRM_CLOCK_PULSE_DURATION,
  SG_PRM_CLOCK_MIDI_MODE,
  
  SG_PRM_GATE_PERIOD,
  SG_PRM_GATE_PULSE_DURATION,
  SG_PRM_GATE_MIDI_MODE,
  SG_PRM_GATE_PADDING,
  
  SG_PRM_CV_MODE,
  SG_PRM_CV_PERIOD,
  SG_PRM_CV_RANGE,
  SG_PRM_CV_MIDI_MODE,
  
  SG_PRM_AUDIO_MODE,
  SG_PRM_AUDIO_FREQUENCY,
  SG_PRM_AUDIO_ENVELOPE_MODE,
  SG_PRM_AUDIO_MIDI_MODE,
};

struct AudioBufferSpecs {
  typedef uint16_t Value;
  enum {
    buffer_size = 128,
    data_size = 16,
  };
};

struct CvBufferSpecs {
  typedef uint16_t Value;
  enum {
    buffer_size = 32,
    data_size = 16,
  };
};


static const uint8_t kAudioBlockSize = 40;
static const uint8_t kCvBlockSize = 10;

struct SignalGeneratorSettings {
  ClockSettings clock;
  GateSettings gate;
  CvSettings cv;
  AudioSettings audio;
};

struct SignalGeneratorState {
  uint32_t clock_phase;
  uint32_t clock_phase_increment;
  
  uint32_t gate_phase;
  uint32_t gate_phase_increment;
  
  uint32_t cv_phase;
  uint32_t cv_phase_increment;
  
  uint32_t audio_phase;
  uint32_t audio_phase_increment;
  uint16_t audio_midi_note;
  
  uint16_t clock_pulse;
  uint16_t clock_pulse_duration;
  uint16_t clock_pulse_ratio;
  
  uint16_t gate_pulse;
  uint16_t gate_pulse_duration;
  uint16_t gate_pulse_ratio;
  
  uint16_t audio_envelope_phase;
  
  uint16_t clock_prescaler;
  uint8_t trigger_count;
  uint8_t gate_state;
  uint8_t last_midi_note;
};

class SignalGenerator {
 public:
  typedef void (*AudioRenderFn)();

  SignalGenerator() { }
  ~SignalGenerator() { }
  
  static void Init();
  static void Render();
  
  static uint16_t ReadAudioSample() {
    return audio_buffer_.ImmediateRead();
  }
  
  static uint16_t ReadCvSample() {
    return cv_buffer_.ImmediateRead();
  }
  
  static const SignalGeneratorSettings& data() { return data_; }
  static SignalGeneratorSettings* mutable_data() { return &data_; }

  static inline void RaiseClockPulse() {
    state_.clock_pulse = state_.clock_pulse_duration;
  }
  static inline bool DecreaseClockPulse() {
    bool result = false;
    if (state_.clock_pulse) {
      result = true;
      --state_.clock_pulse;
    }
    return result;
  }
  static inline uint8_t clock_prescaler() { return state_.clock_prescaler; }

  static void NoteOn(uint8_t note, uint8_t velocity);
  static void NoteOff(uint8_t note);
  static inline void PitchBend(uint16_t pitch_bend) {
    pitch_bend_ = pitch_bend;
  }
  static inline void Modulation(uint8_t modulation) {
    modulation_ = modulation;
  }
  static void Trigger() {
    ++state_.trigger_count;
  }
  static void set_gate_state(uint8_t gate_state) {
    state_.gate_state = gate_state;
  }

 private:
  static void UpdateIncrements();

  static void RenderClock();
  static void RenderGate();
  static void RenderCv();
  
  static void RenderAudioBandLimited();
  static void RenderAudioSine();
  
  static uint16_t cv_samples_[kCvBlockSize];
  static uint16_t last_note_value_;
  
  static SignalGeneratorSettings data_;
  static SignalGeneratorState state_;
  
  static avrlib::RingBuffer<AudioBufferSpecs> audio_buffer_;
  static avrlib::RingBuffer<CvBufferSpecs> cv_buffer_;
  
  static uint16_t pitch_bend_;
  static uint8_t modulation_;
  
  static const AudioRenderFn fn_table_[];
  
  static NoteStack<10> note_stack_;
  
  DISALLOW_COPY_AND_ASSIGN(SignalGenerator);
};

extern SignalGenerator signal_generator;

}  // namespace module_tester

#endif  // MODULE_TESTER_SIGNAL_GENERATOR_H_
