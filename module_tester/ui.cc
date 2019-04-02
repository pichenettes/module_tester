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
// User interface.

#include "module_tester/ui.h"

#include "avrlib/string.h"

#include "module_tester/display.h"
#include "module_tester/frequency_counter.h"
#include "module_tester/parameter.h"
#include "module_tester/pulse_counter.h"
#include "module_tester/signal_generator.h"

namespace module_tester {

const prog_PageInfo page_registry[] PROGMEM = {
  // Clock output
  {
    4,
    PRM_CLOCK_TEMPO
  },
  // Gate output
  {
    3,
    PRM_GATE_PERIOD
  },
  // CV output
  {
    4,
    PRM_CV_MODE
  },
  // Audio output
  {
    4,
    PRM_SYNTH_MODE
  },
  // Audio input
  {
    1,
    PRM_FREQUENCY_COUNTER_UNIT
  },
  // Gate input
  {
    2,
    PRM_PULSE_COUNTER_UNIT
  }
};

/* <static> */
uint8_t Ui::cycle_;
bool Ui::editing_;
bool Ui::gate_;
UiPage Ui::page_;
PageInfo Ui::page_info_;
int8_t Ui::active_control_;
int8_t Ui::scrolling_;
Encoder Ui::encoder_;
Switches Ui::switches_;
Leds Ui::leds_;
EventQueue<8> Ui::queue_;
/* </static> */

/* extern */
Ui ui;

/* static */
void Ui::Init() {
  encoder_.Init();
  switches_.Init();
  leds_.Init();
  lcd.Init();
  display.Init();
  ChangePage(UI_PAGE_CLOCK_OUT);
  display.Clear();
  lcd.SetCustomCharMapRes(&chr_special_characters[0], 7, 1);
  RedrawScreen();
}

/* static */
void Ui::OnClick() {
  editing_ = !editing_;
}

/* static */
void Ui::OnIncrement(int8_t increment) {
  if (!editing_) {
    // Scroll to the next control.
    active_control_ += increment;
    if (active_control_ < 0) {
      active_control_ = 0;
    } else if (active_control_ >= page_info_.num_parameters) {
      active_control_ = page_info_.num_parameters - 1;
    }
    // Scroll the screen if necessary.
    if (active_control_ >= scrolling_ + kLcdHeight) {
      scrolling_ = active_control_ - kLcdHeight + 1;
    } else if (active_control_ < scrolling_) {
      scrolling_ = active_control_;
    }
  } else {
    const Parameter& p = parameter_manager.parameter(
        page_info_.first_parameter + active_control_);
    parameter_manager.Increment(p, increment);
  }
}

/* static */
void Ui::RedrawScreen() {
  if (page_ < UI_PAGE_AUDIO_IN) {
    RedrawScreenOutput();
  } else if (page_ == UI_PAGE_AUDIO_IN) {
    RedrawScreenAudioIn();
  } else if (page_ == UI_PAGE_GATE_IN) {
    RedrawScreenGateIn();
  }
}

/* static */
void Ui::RedrawScreenOutput() {
  char line_buffer[kLcdWidth + 1];

  for (uint8_t i = 0; i < kLcdHeight; ++i) {
    uint8_t control_id = i + scrolling_;
    
    memset(line_buffer, ' ', sizeof(line_buffer));
    line_buffer[kLcdWidth] = '\0';
    if (control_id == active_control_ && !editing_) {
      line_buffer[0] = '>';
    }
    if (page_ <= UI_PAGE_AUDIO_OUT) {
      const Parameter& p = parameter_manager.parameter(
          page_info_.first_parameter + control_id);
      p.Format(
          parameter_manager.GetValue(p),
          editing_ && control_id == active_control_ ? '>' : ' ',
          line_buffer + 1,
          kLcdWidth - 1);
    }
    display.Print(i, line_buffer);
  }
}

/* static */
void Ui::RedrawScreenAudioIn() {
  char line_buffer[kLcdWidth + 1];
  line_buffer[0] = (active_control_ == 0 && !editing_) ? '>' : ' ';
  frequency_counter.Print(line_buffer + 1, kLcdWidth - 1);
  display.Print(0, line_buffer);
}

/* static */
void Ui::RedrawScreenGateIn() {
  char line_buffer[kLcdWidth + 1];
  line_buffer[0] = (active_control_ == 0 && !editing_) ? '>' : ' ';
  pulse_counter.PrintPeriod(line_buffer + 1, kLcdWidth - 1);
  display.Print(0, line_buffer);

  memset(line_buffer, ' ', sizeof(line_buffer));
  line_buffer[0] = (active_control_ == 1 && !editing_) ? '>' : ' ';
  pulse_counter.PrintDuration(line_buffer + 1, kLcdWidth - 1);
  display.Print(1, line_buffer);
}

/* static */
void Ui::ChangePage(UiPage page) {
  page_ = page;
  memcpy_P(&page_info_, &page_registry[page], sizeof(PageInfo));
  scrolling_ = 0;
  active_control_ = 0;
  editing_ = false;
}

/* static */
void Ui::Poll() {
  ++cycle_;
  // I
  int8_t increment = encoder_.Read();
  if (increment != 0) {
    queue_.AddEvent(CONTROL_ENCODER, 0, increment);
  }
  if (encoder_.clicked()) {
    queue_.AddEvent(CONTROL_ENCODER_CLICK, 0, 1);
  }
  
  if ((cycle_ & 0x3) == 0) {
    switches_.Read();
    for (uint8_t i = 0; i < 6; ++i) {
      if (switches_.raised(i)) {
        queue_.AddEvent(CONTROL_SWITCH, 5 - i, 1);
      }
    }
    uint8_t leds_byte = 1 << page_;
    if (gate_) {
      leds_byte |= 1 << UI_PAGE_GATE_IN;
      gate_ = false;
    }
    leds_.Write(leds_byte);
  }
  lcd.Tick();
}

/* static */
void Ui::DoEvents() {
  display.Tick();
  
  bool redraw = false;
  while (queue_.available()) {
    Event e = queue_.PullEvent();
    queue_.Touch();
    
    switch (e.control_type) {
      case CONTROL_ENCODER_CLICK:
        OnClick();
        break;
        
      case CONTROL_ENCODER:
        OnIncrement(e.value);
        break;
        
      case CONTROL_SWITCH:
        ChangePage(static_cast<UiPage>(e.control_id));
        break;
    }
    redraw = true;
  }
  
  if (queue_.idle_time_ms() > 1000) {
    queue_.Touch();
    redraw = true;
  }

  if (queue_.idle_time_ms() > 100 && page_ >= UI_PAGE_AUDIO_IN) {
    // Redraw measurement page at 10 Hz.
    queue_.Touch();
    redraw = true;
  }
  
  if (redraw) {
    display.Clear();
    RedrawScreen();
  }
}

}  // namespace ambika
