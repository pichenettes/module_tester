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
//
// -----------------------------------------------------------------------------
//
// Signal generator.

#include "module_tester/pulse_counter.h"

#include <stdio.h>
#include <math.h>

#include <avr/pgmspace.h>

#include "avrlib/string.h"

#include "module_tester/parameter.h"

namespace module_tester {

/* static */
PulseCounterSettings PulseCounter::data_;

/* static */
uint32_t PulseCounter::duration_;

/* static */
uint32_t PulseCounter::period_;

/* static */
uint16_t PulseCounter::num_measurements_;

/* static */
uint32_t PulseCounter::current_duration_;

/* static */
uint32_t PulseCounter::current_period_;

/* static */
uint16_t PulseCounter::current_num_measurements_;

/* static */
void PulseCounter::Init() {
  data_.unit = PULSE_COUNTER_UNIT_HZ;
  data_.duration_unit = PULSE_COUNTER_DURATION_UNIT_DUTY_CYCLE;
}

const prog_uint8_t bpm_divider[] PROGMEM = { 1, 4, 8, 24 };

/* static */
void PulseCounter::PrintPeriod(char* buffer, uint8_t width) {
  if (data_.unit == PULSE_COUNTER_UNIT_SECONDS) {
    float period = (510.0f / F_CPU) * \
        static_cast<float>(current_period_) / \
        static_cast<float>(current_num_measurements_);
    if (current_period_ == 0 || current_num_measurements_ == 0) {
      period = 0.0f;
    }
    if (period < 0.1f) {
      sprintf(buffer, "per.%.2fms", period * 1000.0f);
    } else {
      sprintf(buffer, "per.%.3fs", period);
    }
  } else if (data_.unit == PULSE_COUNTER_UNIT_HZ) {
    float frequency = (F_CPU / 510) * \
        static_cast<float>(current_num_measurements_) / \
        static_cast<float>(current_period_);
    if (current_period_ == 0 || current_num_measurements_ == 0) {
      frequency = 0.0f;
    }
    sprintf(buffer, "freq%.2fHz", frequency);
  } else {
    uint8_t divider = pgm_read_byte(
        bpm_divider + data_.unit - PULSE_COUNTER_UNIT_BPM_1);
    float bpm = 60 * (F_CPU / 510) * \
        static_cast<float>(current_num_measurements_) / \
        static_cast<float>(current_period_) / static_cast<float>(divider);
    if (current_period_ == 0 || current_num_measurements_ == 0) {
      bpm = 0.0f;
    }
    sprintf(buffer, "bpm %.1f @%d", bpm, divider);
  }
  avrlib::AlignRight(buffer + 4, width - 4);
}

/* static */
void PulseCounter::PrintDuration(char* buffer, uint8_t width) {
  if (data_.duration_unit == PULSE_COUNTER_DURATION_UNIT_SECONDS) {
    float duration = (1000.0f * 510.0f / F_CPU) * \
        static_cast<float>(current_duration_) / \
        static_cast<float>(current_num_measurements_);
    if (current_duration_ == 0 || current_num_measurements_ == 0) {
      duration = 0.0f;
    }
    sprintf(buffer, "dur.%4.2fms", duration);
  } else {
    float ratio = 100.0f * static_cast<float>(current_duration_) / \
        static_cast<float>(current_period_);
    if (current_duration_ == 0 || current_num_measurements_ == 0) {
      ratio = 0.0f;
    }
    sprintf(buffer, "duty%.2f%%", ratio);
  }
  avrlib::AlignRight(buffer + 4, width - 4);
}

/*extern */
PulseCounter pulse_counter;

}  // namespace module_tester
