#!/usr/bin/python2.5
#
# Copyright 2013 Emilie Gillet..
#
# Author: Emilie Gillet (emilie.o.gillet@gmail.com)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# -----------------------------------------------------------------------------
#
# Master resources file.

header = """// Copyright 2013 Emilie Gillet..
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
// Resources definitions.
//
// Automatically generated with:
// make resources
"""

namespace = 'module_tester'
target = 'module_tester'
modifier = 'PROGMEM'
types = ['uint8_t', 'uint16_t']
includes = """
#include "avrlib/base.h"

#include <avr/pgmspace.h>
"""
create_specialized_manager = True

import numpy

import characters
import lookup_tables
import strings
import waveforms

resources = [
  (strings.strings,
   'string', 'STR', 'prog_char', str, False),
  (lookup_tables.lookup_tables,
   'lookup_table', 'LUT', 'prog_uint16_t', int, False),
  (lookup_tables.lookup_tables_32,
   'lookup_table_32', 'LUT', 'prog_uint32_t', int, False),
  (characters.characters, 'character', 'CHR', 'prog_uint8_t', int, True),
  (waveforms.waveforms, 'waveform', 'WAV', 'prog_uint8_t', int, True),
]
