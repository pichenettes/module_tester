Eurorack module tester.

This project is a test signal generator for Eurorack synth modules. It also provides frequency/clock measurement tools, and a +/12V and +5V power supply - making it a compact "all in one" box for powering and debugging a module.

Original developer: Olivier Gillet (ol.gillet@gmail.com)

The firmware is released under a GPL3.0 license. The PCB layouts and schematics are released under a Creative Commons cc-by-sa 3.0 license.

To build code and flash a blank 644p:
make -f module_tester/makefile bootstrap
