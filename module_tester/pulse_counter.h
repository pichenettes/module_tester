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

#ifndef MODULE_TESTER_PULSE_COUNTER_H_
#define MODULE_TESTER_PULSE_COUNTER_H_

#include "avrlib/base.h"

namespace module_tester {

enum PulseCounterUnit {
  PULSE_COUNTER_UNIT_SECONDS,
  PULSE_COUNTER_UNIT_HZ,
  PULSE_COUNTER_UNIT_BPM_1,
  PULSE_COUNTER_UNIT_BPM_4,
  PULSE_COUNTER_UNIT_BPM_8,
  PULSE_COUNTER_UNIT_BPM_24
};

enum PulseCounterDurationUnit {
  PULSE_COUNTER_DURATION_UNIT_SECONDS,
  PULSE_COUNTER_DURATION_UNIT_DUTY_CYCLE,
};

struct PulseCounterSettings {
  PulseCounterUnit unit;
  PulseCounterDurationUnit duration_unit;
};

enum PulseCounterParameter {
  PC_PARAMETER_UNIT,
  PC_PARAMETER_DURATION_UNIT,
};

class PulseCounter {
 public:
  PulseCounter() { }
  ~PulseCounter() { }
  
  static void Init();
  static void UpdatePulseMeasurement(uint32_t duration, uint32_t period) {
    duration_ += duration;
    period_ += period;
    ++num_measurements_;

    if (period_ > (F_CPU / 510 / 2)) {
      current_duration_ = duration_;
      current_period_ = period_;
      current_num_measurements_ = num_measurements_;
      duration_ = 0;
      period_ = 0;
      num_measurements_ = 0;
    }
  }
  
  static const PulseCounterSettings& data() { return data_; }
  static PulseCounterSettings* mutable_data() { return &data_; }
  
  static void PrintPeriod(char* buffer, uint8_t width);
  static void PrintDuration(char* buffer, uint8_t width);

 private:
  static PulseCounterSettings data_;
   
  static uint32_t duration_;
  static uint32_t period_;
  static uint16_t num_measurements_;

  static uint32_t current_duration_;
  static uint32_t current_period_;
  static uint16_t current_num_measurements_;
  
  DISALLOW_COPY_AND_ASSIGN(PulseCounter);
};

extern PulseCounter pulse_counter;

}  // namespace module_tester

#endif  // MODULE_TESTER_PULSE_COUNTER_H_
