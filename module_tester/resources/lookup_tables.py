#!/usr/bin/python2.5
#
# Copyright 2013 Olivier Gillet..
#
# Author: Olivier Gillet (ol.gillet@gmail.com)
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
# Lookup table definitions.

import numpy

lookup_tables = []
lookup_tables_32 = []
sample_rate = 20000000 / 510.0
control_rate = sample_rate / 4



"""----------------------------------------------------------------------------
Phase increment for tempo.
----------------------------------------------------------------------------"""

width = 1 << 32
tempo_values = numpy.arange(40, 241.0)
lookup_tables_32.append(
    ('tempo_phase_increment', width * tempo_values * 8 / (60 * control_rate))
)



"""----------------------------------------------------------------------------
Phase increment for gate.
----------------------------------------------------------------------------"""

period_values = numpy.array(
    [0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0])
lookup_tables_32.append(
    ('gate_phase_increment', width / (control_rate * period_values))
)



"""----------------------------------------------------------------------------
Phase increment for CV source.
----------------------------------------------------------------------------"""

period_values = numpy.array(
    [0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0])
lookup_tables_32.append(
    ('cv_phase_increment', width / (control_rate * period_values))
)



"""----------------------------------------------------------------------------
Phase increment for audio source.
----------------------------------------------------------------------------"""

frequencies = 440 * 2 ** ((12 + numpy.arange(0, 128.0) - 69) / 12.0)
frequencies = [10, 20, 50, 100, 200, 500, 1000, 2000, 5000] + list(frequencies)
frequencies = numpy.array(frequencies)
lookup_tables_32.append(
    ('audio_phase_increment', width * frequencies / sample_rate)
)
lookup_tables.append(
    ('audio_midi_note', 69 + 12 * numpy.log2(frequencies / 440.0))
)

envelope = numpy.arange(0, 257) / 256.0
envelope[-1] = 1.0
envelope = (1 - envelope) ** 4
lookup_tables.append(
    ('envelope', 4095 * envelope)
)



"""----------------------------------------------------------------------------
Pulse durations
----------------------------------------------------------------------------"""

durations = numpy.array([0.001, 0.002, 0.005, 0.01, 0.0, 0.0, 0.0, 0.0])
lookup_tables.append(
    ('pulse_duration', durations * control_rate)
)

pulse_ratios = numpy.array([0, 0, 0, 0, 0.05, 0.10, 0.20, 0.5]) * 65536
lookup_tables.append(
    ('pulse_ratio', pulse_ratios)
)

lookup_tables.append(
    ('clock_prescaler', [24, 6, 3, 1])
)



"""----------------------------------------------------------------------------
Sine wave for CV source.
----------------------------------------------------------------------------"""

t = numpy.arange(0, 257) / 256.0
sine = numpy.sin(2 * numpy.pi * t)

lookup_tables.append(
    ('sine', (sine + 1.0) * 32767)
)
