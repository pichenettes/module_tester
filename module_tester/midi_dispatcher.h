// Copyright 2012 Olivier Gillet.
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

#ifndef MODULE_TESTER_MIDI_DISPATCHER_H_
#define MODULE_TESTER_MIDI_DISPATCHER_H_

#include "module_tester/signal_generator.h"

#include "midi/midi.h"

namespace module_tester {

class MidiDispatcher : public midi::MidiDevice {
 public:
  MidiDispatcher() { }

  // ------ MIDI in handling ---------------------------------------------------

  // Forwarded to the controller.
  static inline void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    signal_generator.NoteOn(note, velocity);
  }
  static inline void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    signal_generator.NoteOff(note);
  }

  // Handled.
  static inline void ControlChange(
      uint8_t channel,
      uint8_t controller,
      uint8_t value) {
    if (controller == midi::kModulationWheelMsb) {
      signal_generator.Modulation(value);
    }
  }
  static inline void PitchBend(uint8_t channel, uint16_t pitch_bend) {
    signal_generator.PitchBend(pitch_bend);
  }
  static void Aftertouch(uint8_t channel, uint8_t note, uint8_t velocity) { }
  static void Aftertouch(uint8_t channel, uint8_t velocity) { }
  static void AllSoundOff(uint8_t channel) { }
  static void ResetAllControllers(uint8_t channel) { }
  static void AllNotesOff(uint8_t channel) { }
  static void OmniModeOff(uint8_t channel) { }
  static void OmniModeOn(uint8_t channel) { }
  static void ProgramChange(uint8_t channel, uint8_t program) { }
  
  static void Reset() { }
  static void Clock() { }
  static void Start() { }
  static void Stop() { }
  static void Continue() { }
  
  static void SysExStart() { }
  static void SysExByte(uint8_t sysex_byte) { }
  static void SysExEnd() { }
  
  static uint8_t CheckChannel(uint8_t channel) {
    return 1;
  }
  
  static void RawMidiData(
      uint8_t status,
      uint8_t* data,
      uint8_t data_size,
      uint8_t accepted_channel) {
  }
  
 private:
  DISALLOW_COPY_AND_ASSIGN(MidiDispatcher);
};

extern MidiDispatcher midi_dispatcher;

}  // namespace module_tester

#endif  // MODULE_TESTER_MIDI_DISPATCHER_H_
