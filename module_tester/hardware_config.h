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

#ifndef MODULE_TESTER_HARDWARE_CONFIG_H_
#define MODULE_TESTER_HARDWARE_CONFIG_H_

#include "avrlib/base.h"
#include "avrlib/gpio.h"
#include "avrlib/parallel_io.h"
#include "avrlib/serial.h"
#include "avrlib/spi.h"
#include "avrlib/timer.h"

namespace module_tester {

using avrlib::Gpio;
using avrlib::MSB_FIRST;
using avrlib::PARALLEL_NIBBLE_HIGH;
using avrlib::PARALLEL_TRIPLE_LOW;
using avrlib::ParallelPort;
using avrlib::PortA;
using avrlib::PortB;
using avrlib::PortC;
using avrlib::PortD;
using avrlib::RingBuffer;
using avrlib::SerialInput;
using avrlib::SerialPort0;
using avrlib::SpiMaster;
using avrlib::Timer;

// Encoder
typedef Gpio<PortC, 1> EncoderALine;
typedef Gpio<PortC, 2> EncoderBLine;
typedef Gpio<PortC, 0> EncoderClickLine;

// MIDI
typedef avrlib::Serial<
    SerialPort0,
    31250,
    avrlib::POLLED,
    avrlib::DISABLED> MidiIO;
typedef RingBuffer<SerialInput<SerialPort0> > MidiBuffer;

// LCD
static const uint8_t kLcdWidth = 16;
static const uint8_t kLcdHeight = 2;
typedef Gpio<PortA, 3> LcdRsLine;
typedef Gpio<PortA, 2> LcdEnableLine;
typedef ParallelPort<PortA, PARALLEL_NIBBLE_HIGH> LcdDataBus;

// IO
typedef Gpio<PortC, 7> IOClockLine;
typedef Gpio<PortC, 6> IOInputLine;
typedef Gpio<PortC, 5> IOOutputLine;
typedef Gpio<PortC, 4> IOEnableLine;

// Timers
typedef Timer<0> SystemTimer;
typedef Timer<1> TuningTimer;
typedef Timer<2> AudioTimer;

// Clock and gate output
typedef Gpio<PortD, 4> ClockOutput;
typedef Gpio<PortD, 3> GateOutput;

// Gate input
typedef Gpio<PortD, 2> GateInput;

// Spi DAC
typedef SpiMaster<Gpio<PortB, 4>, MSB_FIRST, 2> DacInterface;

}  // namespace module_tester

#endif  // MODULE_TESTER_HARDWARE_CONFIG_H_
