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

#ifndef MODULE_TESTER_DISPLAY_H_
#define MODULE_TESTER_DISPLAY_H_

#include "avrlib/base.h"
#include "avrlib/devices/buffered_display.h"
#include "avrlib/devices/hd44780_lcd.h"
#include "avrlib/gpio.h"
#include "avrlib/parallel_io.h"

#include "module_tester/hardware_config.h"

using avrlib::BufferedDisplay;
using avrlib::Gpio;
using avrlib::Hd44780Lcd;
using avrlib::PARALLEL_NIBBLE_HIGH;
using avrlib::PARALLEL_NIBBLE_LOW;
using avrlib::ParallelPort;
using avrlib::PortD;

namespace module_tester {

static const uint8_t kDelimiter = 0x07;

typedef Hd44780Lcd<
    LcdRsLine, LcdEnableLine, LcdDataBus, kLcdWidth, kLcdHeight> Lcd;

extern Lcd lcd;
extern BufferedDisplay<Lcd> display;

}  // namespace module_tester

#endif  // MODULE_TESTER_DISPLAY_H_
