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
// Parameter definitions.

#ifndef MODULE_TESTER_PARAMETER_DEFINITIONS_H_
#define MODULE_TESTER_PARAMETER_DEFINITIONS_H_

#include "avrlib/base.h"

#include "module_tester/resources.h"

namespace module_tester {

enum Unit {
  UNIT_INTEGER,
  UNIT_NOTE_FREQUENCY,
  UNIT_OTHER
};

enum Domain {
  DOMAIN_SIGNAL_GENERATOR,
  DOMAIN_FREQUENCY_COUNTER = 0x20,
  DOMAIN_PULSE_COUNTER = 0x40,
  DOMAIN_OFFSET_MASK = 0x1f
};

enum GlobalParameter {
  PRM_CLOCK_TEMPO,
  PRM_CLOCK_RESOLUTION,
  PRM_CLOCK_PULSE_DURATION,
  PRM_CLOCK_MIDI_MODE,
  
  PRM_GATE_PERIOD,
  PRM_GATE_PULSE_DURATION,
  PRM_GATE_MIDI_MODE,
  
  PRM_CV_MODE,
  PRM_CV_PERIOD,
  PRM_CV_RANGE,
  PRM_CV_MIDI_MODE,
  
  PRM_SYNTH_MODE,
  PRM_SYNTH_FREQUENCY,
  PRM_SYNTH_ENVELOPE,
  PRM_SYNTH_MIDI_MODE,
  
  PRM_FREQUENCY_COUNTER_UNIT,
  
  PRM_PULSE_COUNTER_UNIT,
  PRM_PULSE_COUNTER_DURATION_UNIT,
};

struct Parameter {
  uint8_t offset;
  ResourceId unit;
  uint8_t min_value;
  uint8_t max_value;
  ResourceId name;
  
  uint8_t Increment(uint8_t current_value, int8_t increment) const;
  void Format(uint8_t value, char prefix, char* buffer, uint8_t width) const;
  static void PrintNote(uint8_t note, char* buffer);
};

typedef Parameter PROGMEM prog_Parameter;

class ParameterManager {
 public:
  ParameterManager() { }
  static const Parameter& parameter(uint8_t index);

  static void SetValue(const Parameter& parameter, uint8_t value);

  static uint8_t GetValue(const Parameter& parameter);

  static void Increment(const Parameter& parameter, int8_t increment) {
    uint8_t value = GetValue(parameter);
    value = parameter.Increment(value, increment);
    SetValue(parameter, value);
  }
  
 private:
  static Parameter cached_definition_;
  static uint8_t cached_index_;
  
  DISALLOW_COPY_AND_ASSIGN(ParameterManager);
};

extern ParameterManager parameter_manager;

}  // namespace module_tester

#endif  // MODULE_TESTER_PARAMETER_DEFINITIONS_H_
