// Copyright 2013 Emilie Gillet..
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
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

#include "avrlib/boot.h"
#include "avrlib/gpio.h"
#include "avrlib/parallel_io.h"
#include "avrlib/timer.h"
#include "avrlib/watchdog_timer.h"

#include "midi/midi.h"

#include "module_tester/frequency_counter.h"
#include "module_tester/hardware_config.h"
#include "module_tester/midi_dispatcher.h"
#include "module_tester/pulse_counter.h"
#include "module_tester/signal_generator.h"
#include "module_tester/ui.h"

using namespace avrlib;
using namespace module_tester;
using namespace midi;

MidiIO midi_io;
MidiBuffer midi_in_buffer;
MidiStreamParser<MidiDispatcher> midi_parser;

SystemTimer sys_timer;
AudioTimer audio_timer;
TuningTimer tuning_timer;

ClockOutput clock_output;
GateOutput gate_output;
GateInput gate_input;

DacInterface dac_interface;

volatile uint8_t midi_clock_ticks = 0;

inline void PollMidiIn() {
  if (midi_io.readable()) {
    uint8_t byte = midi_io.ImmediateRead();
    if (byte == 0xf8) {
      ++midi_clock_ticks;
    } else {
      midi_in_buffer.NonBlockingWrite(byte);
    }
  }
}

// 4.9kHz: MIDI in/out; gate & clock poll.
// 1.22kHz: UI poll
ISR(TIMER0_OVF_vect, ISR_NOBLOCK) {
  static uint8_t cycle = 0;
  PollMidiIn();
  if ((cycle & 0x1) == 0) {
    ui.Poll();
  }
  if ((cycle & 0x3) == 0) {
    TickSystemClock();
  }
  ++cycle;
}

// 2.5 MHz timebase used for time measurements
volatile uint8_t num_overflows;

ISR(TIMER1_CAPT_vect) {
  static uint16_t previous_time = 0;
  uint16_t current_time = ICR1;
  uint32_t interval = current_time + \
      (static_cast<uint32_t>(num_overflows) << 16) - previous_time;
  previous_time = current_time;
  num_overflows = 0;
  frequency_counter.UpdateFrequencyMeasurement(interval);
}

ISR(TIMER1_OVF_vect, ISR_NOBLOCK) {
  ++num_overflows;
}

static uint32_t pulse_duration;
static uint32_t pulse_period;
static uint8_t counter;
static uint8_t gate_state;

// 39kHz audio output clock
ISR(TIMER2_OVF_vect) {
  Word sample_12bits;
  
  dac_interface.Strobe();
  if (!(counter & 0x3)) {
    Word cv_sample;
    cv_sample.value = signal_generator.ReadCvSample();
    sample_12bits.value = (cv_sample.value & 0x0fff) | 0x9000;
    dac_interface.Overwrite(sample_12bits.bytes[1]);
    if (signal_generator.data().clock.midi_mode) {
      uint8_t prescaler = signal_generator.clock_prescaler();
      if (midi_clock_ticks >= prescaler) {
        midi_clock_ticks -= prescaler;
        signal_generator.RaiseClockPulse();
      }
      if (signal_generator.DecreaseClockPulse()) {
        cv_sample.bytes[1] |= 0x80;
      }
    }
    clock_output.set_value(cv_sample.bytes[1] & 0x80);
    gate_output.set_value(cv_sample.bytes[1] & 0x40);
    dac_interface.Wait();
    dac_interface.Send(sample_12bits.bytes[0]);
    dac_interface.Strobe();
  }
  
  sample_12bits.value = signal_generator.ReadAudioSample() | 0x1000;
  dac_interface.Overwrite(sample_12bits.bytes[1]);
  uint8_t new_gate_state = !gate_input.value();
  ui.set_gate(new_gate_state);
  if (gate_state && !new_gate_state) {
    pulse_duration = pulse_period;
  }
  if (new_gate_state && !gate_state) {
    pulse_counter.UpdatePulseMeasurement(pulse_duration, pulse_period);
    signal_generator.Trigger();
    pulse_period = 0;
  }
  gate_state = new_gate_state;
  signal_generator.set_gate_state(new_gate_state);
  dac_interface.Wait();
  dac_interface.Overwrite(sample_12bits.bytes[0]);
  ++pulse_period;
  ++counter;
  dac_interface.Wait();
}

inline void Init() {
  sei();
  UCSR0B = 0;
  ResetWatchdog();
  
  midi_io.Init();
  
  dac_interface.Init();
  
  signal_generator.Init();
  frequency_counter.Init();
  pulse_counter.Init();
  ui.Init();
  
  sys_timer.set_prescaler(2);
  sys_timer.set_mode(TIMER_PWM_PHASE_CORRECT);
  sys_timer.Start();
  
  clock_output.set_mode(DIGITAL_OUTPUT);
  gate_output.set_mode(DIGITAL_OUTPUT);
  gate_input.set_mode(DIGITAL_INPUT);
  
  audio_timer.set_prescaler(1);
  audio_timer.set_mode(TIMER_PWM_PHASE_CORRECT);
  audio_timer.Start();
  
  dac_interface.Strobe();
  dac_interface.Send(0x10 | 0x0f);
  dac_interface.Send(0xff);
  
  tuning_timer.set_mode(TIMER_NORMAL);
  tuning_timer.set_prescaler(1);
  tuning_timer.Start();
  tuning_timer.StartInputCapture();
}

int main(void) {
  Init();
  ui.FlushEvents();
  while (1) {
    signal_generator.Render();
    
    // Handle UI events
    ui.DoEvents();
    
    // Process MIDI events
    while (midi_in_buffer.readable()) {
      midi_parser.PushByte(midi_in_buffer.ImmediateRead());
    }
  }
}
