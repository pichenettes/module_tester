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
// User interface.

#ifndef MODULE_TESTER_UI_H_
#define MODULE_TESTER_UI_H_

#include "avrlib/base.h"

#include "avrlib/devices/pot_scanner.h"
#include "avrlib/devices/rotary_encoder.h"
#include "avrlib/devices/switch.h"
#include "avrlib/ui/event_queue.h"

#include "module_tester/hardware_config.h"
// #include "controller/resources.h"

namespace module_tester {

using namespace avrlib;

typedef RotaryEncoder<EncoderALine, EncoderBLine, EncoderClickLine> Encoder;
typedef DebouncedSwitches<IOEnableLine, IOClockLine, IOInputLine, 6> Switches;
typedef ShiftRegisterOutput<IOEnableLine, IOClockLine, IOOutputLine, 8, MSB_FIRST> Leds;

enum UiPage {
  UI_PAGE_CLOCK_OUT,
  UI_PAGE_GATE_OUT,
  UI_PAGE_CV_OUT,
  UI_PAGE_AUDIO_OUT,
  UI_PAGE_AUDIO_IN,
  UI_PAGE_GATE_IN
};

struct PageInfo {
  uint8_t num_parameters;
  uint8_t first_parameter;
};

typedef PageInfo PROGMEM prog_PageInfo;

class Ui {
 public:
  Ui() { }
  
  static void Init();
  static void Poll();
  static void DoEvents();
  static void FlushEvents() {
    queue_.Flush();
  }
  static void OnClick();
  static void OnIncrement(int8_t increment);

  static void ChangePage(UiPage page);
  static void RedrawScreen();
  
  static uint8_t has_events() {
    return queue_.available();
  }
  
  static void set_gate(bool gate) {
    if (!gate_) {
      gate_ = gate;
    }
  }
  
 private:
  static void RedrawScreenOutput();
  static void RedrawScreenAudioIn();
  static void RedrawScreenGateIn();
   
  static uint8_t cycle_;
  
  static bool editing_;
  static bool gate_;
  
  static int8_t scrolling_;
  static int8_t active_control_;
  static PageInfo page_info_;
  static UiPage page_;
  static Encoder encoder_;
  static Switches switches_;
  static Leds leds_;
  static EventQueue<8> queue_;

  DISALLOW_COPY_AND_ASSIGN(Ui);
};

extern Ui ui;

}  // namespace module_tester

#endif  // MODULE_TESTER_UI_H_
