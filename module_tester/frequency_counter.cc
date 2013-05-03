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

#include "module_tester/frequency_counter.h"

#include <stdio.h>
#include <math.h>

#include <avr/pgmspace.h>

#include "avrlib/string.h"

#include "module_tester/parameter.h"

namespace module_tester {

/* static */
FrequencyCounterSettings FrequencyCounter::data_;

/* static */
uint32_t FrequencyCounter::interval_;

/* static */
uint16_t FrequencyCounter::num_measurements_;

/* static */
uint32_t FrequencyCounter::current_interval_;

/* static */
uint16_t FrequencyCounter::current_num_measurements_;

/* static */
int16_t FrequencyCounter::stable_note_;

/* static */
int16_t FrequencyCounter::previous_stable_note_;

/* static */
int16_t FrequencyCounter::note_interval_;

/* static */
uint8_t FrequencyCounter::note_acquisition_delay_;

/* static */
void FrequencyCounter::Init() {
  data_.unit = FREQUENCY_COUNTER_UNIT_HZ;
}

/* static */
void FrequencyCounter::Print(char* buffer, uint8_t width) {
  float frequency = F_CPU * static_cast<float>(current_num_measurements_) /
      static_cast<float>(current_interval_);
  if (current_interval_ == 0 || current_num_measurements_ == 0) {
    frequency = 0;
  }
  if (data_.unit == FREQUENCY_COUNTER_UNIT_HZ) {
    sprintf(buffer, "freq%3.2fHz", frequency);
  } else if (data_.unit == FREQUENCY_COUNTER_UNIT_MIDI_NOTE) {
    char note[4];
    
    float midi_note = 69.0f + (12.0f / 0.69314718f) * logf(frequency / 440.0f);
    float integral_part = roundf(midi_note);
    int16_t integral_part_int = static_cast<uint16_t>(integral_part);
    if (integral_part_int > 127) {
      integral_part_int = 127;
    } else if (integral_part_int < 0) {
      integral_part_int = 0;
    }
    Parameter::PrintNote(integral_part_int, note);
    int16_t cents = static_cast<int16_t>(100.0 * (midi_note - integral_part));
    if (frequency) {
      sprintf(buffer, "note%s %+d", note, cents);
    } else {
      sprintf(buffer, "note----");
    }
  } else if (data_.unit == FREQUENCY_COUNTER_UNIT_INTERVAL) {
    float midi_note = 69.0f + (12.0f / 0.69314718f) * logf(frequency / 440.0f);
    int16_t note_integral = static_cast<int16_t>(midi_note * 100.0);
    if (!note_acquisition_delay_) {
      if (stable_note_ > (note_integral + 50)
          || note_integral > (stable_note_ + 50)) {
        note_acquisition_delay_ = 2;
      }
    }
    if (note_acquisition_delay_ > 0) {
      --note_acquisition_delay_;
      if (note_acquisition_delay_ == 0) {
        previous_stable_note_ = stable_note_;
        stable_note_ = note_integral;
        if (stable_note_ > (previous_stable_note_ + 50)) {
          note_interval_ = stable_note_ - previous_stable_note_;
        } else if (previous_stable_note_ > (stable_note_ + 50)) {
          note_interval_ = previous_stable_note_ - stable_note_;
        }
      }
    }
    sprintf(buffer, "int.%d", note_interval_);
  }
  avrlib::AlignRight(buffer + 4, width - 4);
}

/*extern */
FrequencyCounter frequency_counter;

}  // namespace module_tester
