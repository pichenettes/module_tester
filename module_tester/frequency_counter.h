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

#ifndef MODULE_TESTER_FREQUENCY_COUNTER_H_
#define MODULE_TESTER_FREQUENCY_COUNTER_H_

#include "avrlib/base.h"

namespace module_tester {

enum FrequencyCounterUnit {
  FREQUENCY_COUNTER_UNIT_HZ,
  FREQUENCY_COUNTER_UNIT_MIDI_NOTE,
  FREQUENCY_COUNTER_UNIT_INTERVAL
};

struct FrequencyCounterSettings {
  FrequencyCounterUnit unit;
};

enum FrequencyCounterParameter {
  FC_PARAMETER_UNIT,
};

class FrequencyCounter {
 public:
  FrequencyCounter() { }
  ~FrequencyCounter() { }
  
  static void Init();
  static void UpdateFrequencyMeasurement(uint32_t interval) {
    interval_ += interval;
    ++num_measurements_;

    LongWord w;
    w.value = interval_;
    if (w.words[1] >= (F_CPU / 10) >> 16) {
      current_interval_ = interval_;
      current_num_measurements_ = num_measurements_;
      interval_ = 0;
      num_measurements_ = 0;
    }
  }
  
  static const FrequencyCounterSettings& data() { return data_; }
  static FrequencyCounterSettings* mutable_data() { return &data_; }
  
  static void Print(char* buffer, uint8_t width);

 private:
  static FrequencyCounterSettings data_;
   
  static uint32_t interval_;
  static uint32_t current_interval_;
  static uint16_t num_measurements_;
  static uint16_t current_num_measurements_;
  
  static int16_t stable_note_;
  static int16_t previous_stable_note_;
  static int16_t note_interval_;
  static uint8_t note_acquisition_delay_;
  
  DISALLOW_COPY_AND_ASSIGN(FrequencyCounter);
};

extern FrequencyCounter frequency_counter;

}  // namespace module_tester

#endif  // MODULE_TESTER_FREQUENCY_COUNTER_H_
