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
// Resources definitions.
//
// Automatically generated with:
// make resources


#ifndef MODULE_TESTER_RESOURCES_H_
#define MODULE_TESTER_RESOURCES_H_


#include "avrlib/base.h"

#include <avr/pgmspace.h>


#include "avrlib/resources_manager.h"

namespace module_tester {

typedef uint8_t ResourceId;

extern const prog_char* const string_table[];

extern const prog_uint16_t* const lookup_table_table[];

extern const prog_uint32_t* const lookup_table_32_table[];

extern const prog_uint8_t* const character_table[];

extern const prog_uint8_t* const waveform_table[];

extern const prog_uint16_t lut_audio_midi_note[] PROGMEM;
extern const prog_uint16_t lut_envelope[] PROGMEM;
extern const prog_uint16_t lut_pulse_duration[] PROGMEM;
extern const prog_uint16_t lut_pulse_ratio[] PROGMEM;
extern const prog_uint16_t lut_clock_prescaler[] PROGMEM;
extern const prog_uint16_t lut_sine[] PROGMEM;
extern const prog_uint32_t lut_tempo_phase_increment[] PROGMEM;
extern const prog_uint32_t lut_gate_phase_increment[] PROGMEM;
extern const prog_uint32_t lut_cv_phase_increment[] PROGMEM;
extern const prog_uint32_t lut_audio_phase_increment[] PROGMEM;
extern const prog_uint8_t chr_special_characters[] PROGMEM;
extern const prog_uint8_t wav_sine[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_0[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_1[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_2[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_3[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_4[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_square_5[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_0[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_1[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_2[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_3[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_4[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_saw_5[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_0[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_1[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_2[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_3[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_4[] PROGMEM;
extern const prog_uint8_t wav_bandlimited_triangle_5[] PROGMEM;
#define STR_TEMPO 0  // tempo
#define STR_RESOL_ 1  // resol.
#define STR_PULSE 2  // pulse
#define STR_MIDI 3  // midi
#define STR_PERIOD 4  // period
#define STR_MODE 5  // mode
#define STR_SHAPE 6  // shape
#define STR_RANGE 7  // range
#define STR_FREQ_ 8  // freq.
#define STR_ENVEL_ 9  // envel.
#define STR_OFF 10  // off
#define STR_ON 11  // on
#define STR_NOTE 12  // note
#define STR_GATE 13  // gate
#define STR_1_PPQN 14  // 1 ppqn
#define STR_4_PPQN 15  // 4 ppqn
#define STR_8_PPQN 16  // 8 ppqn
#define STR_24_PPQN 17  // 24 ppqn
#define STR_1MS 18  // 1ms
#define STR_2MS 19  // 2ms
#define STR_5MS 20  // 5ms
#define STR_10MS 21  // 10ms
#define STR_5_ 22  // 5%
#define STR_10_ 23  // 10%
#define STR_20_ 24  // 20%
#define STR_50_ 25  // 50%
#define STR__10MS 26  // 10ms
#define STR_20MS 27  // 20ms
#define STR_50MS 28  // 50ms
#define STR_100MS 29  // 100ms
#define STR_200MS 30  // 200ms
#define STR_500MS 31  // 500ms
#define STR_1S 32  // 1s
#define STR_2S 33  // 2s
#define STR_5S 34  // 5s
#define STR_10S 35  // 10s
#define STR_TRIANGLE 36  // triangle
#define STR_SQUARE 37  // square
#define STR_RAMP_UP 38  // ramp up
#define STR_RAMP_DN 39  // ramp dn
#define STR_SINE 40  // sine
#define STR_1OCT_ARP 41  // 1oct arp
#define STR_2OCT_ARP 42  // 2oct arp
#define STR_CHRM_SCL 43  // chrm.scl
#define STR_C1_NOTE 44  // c1 note
#define STR_C3_NOTE 45  // c3 note
#define STR__OFF 46  // off
#define STR__GATE 47  // gate
#define STR_TRIGGER 48  // trigger
#define STR___OFF 49  // off
#define STR__NOTE 50  // note
#define STR_VELOCITY 51  // velocity
#define STR_PTCHBEND 52  // ptchbend
#define STR_MODWHEEL 53  // modwheel
#define STR_SAW 54  // saw
#define STR__SQUARE 55  // square
#define STR__TRIANGLE 56  // triangle
#define STR__SINE 57  // sine
#define STR_NOISE 58  // noise
#define STR_10HZ 59  // 10Hz
#define STR_20HZ 60  // 20Hz
#define STR_50HZ 61  // 50Hz
#define STR_100HZ 62  // 100Hz
#define STR_200HZ 63  // 200Hz
#define STR_500HZ 64  // 500Hz
#define STR_1_0KHZ 65  // 1.0kHz
#define STR_2_0KHZ 66  // 2.0kHz
#define STR_5_0KHZ 67  // 5.0kHz
#define STR_11V 68  // 1V
#define STR_12V 69  // 2V
#define STR_15V 70  // 5V
#define STR_S1V 71  // +1V
#define STR_S2V 72  // +2V
#define STR_S5V 73  // +5V
#define STR____OFF 74  // off
#define STR___NOTE 75  // note
#define STR___GATE 76  // gate
#define STR__ON 77  // on
#define LUT_AUDIO_MIDI_NOTE 0
#define LUT_AUDIO_MIDI_NOTE_SIZE 137
#define LUT_ENVELOPE 1
#define LUT_ENVELOPE_SIZE 257
#define LUT_PULSE_DURATION 2
#define LUT_PULSE_DURATION_SIZE 8
#define LUT_PULSE_RATIO 3
#define LUT_PULSE_RATIO_SIZE 8
#define LUT_CLOCK_PRESCALER 4
#define LUT_CLOCK_PRESCALER_SIZE 4
#define LUT_SINE 5
#define LUT_SINE_SIZE 257
#define LUT_TEMPO_PHASE_INCREMENT 0
#define LUT_TEMPO_PHASE_INCREMENT_SIZE 201
#define LUT_GATE_PHASE_INCREMENT 1
#define LUT_GATE_PHASE_INCREMENT_SIZE 10
#define LUT_CV_PHASE_INCREMENT 2
#define LUT_CV_PHASE_INCREMENT_SIZE 11
#define LUT_AUDIO_PHASE_INCREMENT 3
#define LUT_AUDIO_PHASE_INCREMENT_SIZE 137
#define CHR_SPECIAL_CHARACTERS 0
#define CHR_SPECIAL_CHARACTERS_SIZE 56
#define WAV_SINE 0
#define WAV_SINE_SIZE 257
#define WAV_BANDLIMITED_SQUARE_0 1
#define WAV_BANDLIMITED_SQUARE_0_SIZE 257
#define WAV_BANDLIMITED_SQUARE_1 2
#define WAV_BANDLIMITED_SQUARE_1_SIZE 257
#define WAV_BANDLIMITED_SQUARE_2 3
#define WAV_BANDLIMITED_SQUARE_2_SIZE 257
#define WAV_BANDLIMITED_SQUARE_3 4
#define WAV_BANDLIMITED_SQUARE_3_SIZE 257
#define WAV_BANDLIMITED_SQUARE_4 5
#define WAV_BANDLIMITED_SQUARE_4_SIZE 257
#define WAV_BANDLIMITED_SQUARE_5 6
#define WAV_BANDLIMITED_SQUARE_5_SIZE 257
#define WAV_BANDLIMITED_SQUARE_6 7
#define WAV_BANDLIMITED_SQUARE_6_SIZE 257
#define WAV_BANDLIMITED_SAW_0 8
#define WAV_BANDLIMITED_SAW_0_SIZE 257
#define WAV_BANDLIMITED_SAW_1 9
#define WAV_BANDLIMITED_SAW_1_SIZE 257
#define WAV_BANDLIMITED_SAW_2 10
#define WAV_BANDLIMITED_SAW_2_SIZE 257
#define WAV_BANDLIMITED_SAW_3 11
#define WAV_BANDLIMITED_SAW_3_SIZE 257
#define WAV_BANDLIMITED_SAW_4 12
#define WAV_BANDLIMITED_SAW_4_SIZE 257
#define WAV_BANDLIMITED_SAW_5 13
#define WAV_BANDLIMITED_SAW_5_SIZE 257
#define WAV_BANDLIMITED_SAW_6 14
#define WAV_BANDLIMITED_SAW_6_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_0 15
#define WAV_BANDLIMITED_TRIANGLE_0_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_1 16
#define WAV_BANDLIMITED_TRIANGLE_1_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_2 17
#define WAV_BANDLIMITED_TRIANGLE_2_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_3 18
#define WAV_BANDLIMITED_TRIANGLE_3_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_4 19
#define WAV_BANDLIMITED_TRIANGLE_4_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_5 20
#define WAV_BANDLIMITED_TRIANGLE_5_SIZE 257
#define WAV_BANDLIMITED_TRIANGLE_6 21
#define WAV_BANDLIMITED_TRIANGLE_6_SIZE 257
typedef avrlib::ResourcesManager<
    ResourceId,
    avrlib::ResourcesTables<
        string_table,
        lookup_table_table> > ResourcesManager; 

}  // namespace module_tester

#endif  // MODULE_TESTER_RESOURCES_H_
