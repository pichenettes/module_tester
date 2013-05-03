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
// Parameter definitions.

#include "module_tester/parameter.h"

#include <avr/pgmspace.h>

#include "avrlib/string.h"

#include "module_tester/frequency_counter.h"
#include "module_tester/pulse_counter.h"
#include "module_tester/signal_generator.h"
#include "module_tester/ui.h"

namespace module_tester {

uint8_t Parameter::Increment(
    uint8_t current_value,
    int8_t increment) const {
  int16_t value = current_value;
  uint8_t new_value = current_value;
  value += increment;
  if (value >= min_value && value <= max_value) {
    new_value = static_cast<uint8_t>(value);
  }
  return new_value;
}

static const prog_char note_names[] PROGMEM = " CC# DD# E FF# GG# AA# B";
static const prog_char octaves[] PROGMEM = "-0123456789";

/* static */
void Parameter::PrintNote(uint8_t note, char* buffer) {
  uint8_t octave = 0;
  while (note >= 12) {
    ++octave;
    note -= 12;
  }
  *buffer++ = ResourcesManager::Lookup<char, uint8_t>(
      note_names, note << 1);
  *buffer++ = ResourcesManager::Lookup<char, uint8_t>(
      note_names, 1 + (note << 1));
  *buffer++ = ResourcesManager::Lookup<char, uint8_t>(
      octaves, octave);
  *buffer = '\0';
}

void Parameter::Format(
    uint8_t value,
    char prefix,
    char* buffer,
    uint8_t width) const {
  ResourcesManager::LoadStringResource(name, buffer, 6);
  AlignLeft(buffer, 6);
  buffer += 7;
  width -= 7;

  buffer[-1] = prefix;
  if (unit == UNIT_INTEGER) {
    UnsafeItoa<int16_t>(value, width, buffer);
  } else if (unit == UNIT_NOTE_FREQUENCY) {
    if (value >= 9) {
      PrintNote(value - 9 + 12, buffer);
    } else {
      ResourcesManager::LoadStringResource(STR_10HZ + value, buffer, width);
    }
  } else {
    ResourcesManager::LoadStringResource(unit + value, buffer, width);
  }
  AlignRight(buffer - 1, width + 1);
}

static const prog_Parameter parameters[] PROGMEM = {
  // 0
  { SG_PRM_CLOCK_TEMPO,
    UNIT_INTEGER, 40, 240,
    STR_TEMPO },
    
  // 1
  { SG_PRM_CLOCK_RESOLUTION,
    STR_1_PPQN, CLOCK_RESOLUTION_1_PPQN, CLOCK_RESOLUTION_24_PPQN,
    STR_RESOL_ },

  // 2
  { SG_PRM_CLOCK_PULSE_DURATION,
    STR_1MS, PULSE_DURATION_1_MS, PULSE_DURATION_50_PERCENT,
    STR_PULSE },

  // 3
  { SG_PRM_CLOCK_MIDI_MODE,
    STR_OFF, 0, 1,
    STR_MIDI },
  
  // 4
  { SG_PRM_GATE_PERIOD,
    STR__10MS, PERIOD_10_MS, PERIOD_10_S,
    STR_PERIOD },
    
  // 5
  { SG_PRM_GATE_PULSE_DURATION,
    STR_1MS, PULSE_DURATION_1_MS, PULSE_DURATION_50_PERCENT,
    STR_PULSE },
    
  // 6
  { SG_PRM_GATE_MIDI_MODE,
    STR__OFF, GATE_MIDI_MODE_OFF, GATE_MIDI_MODE_TRIGGER,
    STR_MIDI },
  
  // 7
  { SG_PRM_CV_MODE,
    STR_TRIANGLE, CV_MODE_TRIANGLE_LFO, CV_MODE_CHROMATIC_SCALE_ARP,
    STR_MODE },
    
  // 8
  { SG_PRM_CV_PERIOD,
    STR__10MS, PERIOD_10_MS, PERIOD_10_S,
    STR_PERIOD },

  // 8
  { SG_PRM_CV_RANGE,
    STR_11V, CV_RANGE_PM_1V, CV_RANGE_0_5V,
    STR_RANGE },

  // 9
  { SG_PRM_CV_MIDI_MODE,
    STR___OFF, CV_MIDI_MODE_OFF, CV_MIDI_MODE_MOD_WHEEL,
    STR_MIDI },
  
  // 10
  { SG_PRM_AUDIO_MODE,
    STR_SAW, AUDIO_MODE_SAW, AUDIO_MODE_SINE,
    STR_SHAPE },

  // 11
  { SG_PRM_AUDIO_FREQUENCY,
    UNIT_NOTE_FREQUENCY, 0, 96 + 9, 
    STR_FREQ_ },

  // 12
  { SG_PRM_AUDIO_ENVELOPE_MODE,
    STR__OFF, AUDIO_ENVELOPE_MODE_OFF, AUDIO_ENVELOPE_MODE_TRIGGER,
    STR_ENVEL_ },
    
  // 13
  { SG_PRM_AUDIO_MIDI_MODE,
    STR____OFF, AUDIO_MIDI_MODE_OFF, AUDIO_MIDI_MODE_ON,
    STR_MIDI },

  // 14
  { FC_PARAMETER_UNIT | DOMAIN_FREQUENCY_COUNTER,
    0, FREQUENCY_COUNTER_UNIT_HZ, FREQUENCY_COUNTER_UNIT_INTERVAL,
    0 },
    
  // 15
  { PC_PARAMETER_UNIT | DOMAIN_PULSE_COUNTER,
    0, PULSE_COUNTER_UNIT_SECONDS, PULSE_COUNTER_UNIT_BPM_24,
    0 },

  // 16
  { PC_PARAMETER_DURATION_UNIT | DOMAIN_PULSE_COUNTER,
    0, PULSE_COUNTER_DURATION_UNIT_SECONDS,
    PULSE_COUNTER_DURATION_UNIT_DUTY_CYCLE,
    0 }
    
};

/* static */
Parameter ParameterManager::cached_definition_;

/* static */
uint8_t ParameterManager::cached_index_ = 0xff;

/* static */
const Parameter& ParameterManager::parameter(uint8_t index) {
  if (index != cached_index_) {
    memcpy_P(&cached_definition_, &parameters[index], sizeof(Parameter));
    cached_index_ = index;
  }
  return cached_definition_;
}

/* static */
void ParameterManager::SetValue(const Parameter& parameter, uint8_t value) {
  uint8_t* bytes = static_cast<uint8_t*>(
      static_cast<void*>(signal_generator.mutable_data()));
  uint8_t offset = parameter.offset;
  if (offset & DOMAIN_FREQUENCY_COUNTER) {
    bytes = static_cast<uint8_t*>(
      static_cast<void*>(frequency_counter.mutable_data()));
  } else if (offset & DOMAIN_PULSE_COUNTER) {
    bytes = static_cast<uint8_t*>(
      static_cast<void*>(pulse_counter.mutable_data()));
  }
  bytes[offset & DOMAIN_OFFSET_MASK] = value;
}

/* static */
uint8_t ParameterManager::GetValue(const Parameter& parameter) {
  if (parameter.offset & DOMAIN_FREQUENCY_COUNTER) {
    const uint8_t* bytes = static_cast<const uint8_t*>(
      static_cast<const void*>(&frequency_counter.data()));
    return bytes[parameter.offset & DOMAIN_OFFSET_MASK];
  } else if (parameter.offset & DOMAIN_PULSE_COUNTER) {
    const uint8_t* bytes = static_cast<const uint8_t*>(
      static_cast<const void*>(&pulse_counter.data()));
    return bytes[parameter.offset & DOMAIN_OFFSET_MASK];
  } else {
    const uint8_t* bytes = static_cast<const uint8_t*>(
      static_cast<const void*>(&signal_generator.data()));
    return bytes[parameter.offset];
  }
}

/* extern */
ParameterManager parameter_manager;

}  // namespace module_tester
