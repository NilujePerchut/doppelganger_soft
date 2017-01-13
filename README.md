# Doppelganger RevA0
_Niluje (c) 2017_

Joystick interface to Brook PCB and old video game systems.
This is the software part of the project, compatible with the Doppelganger board revA0.

## Video game systems supported

This project currently supports the following systems:
* PC (through Brook)
* PS3 (through Brook)
* PS4 (through Brook)
* NES
* SNES
* Saturn

## Software Usage

* If the Doppelganger board is supplied with no buttons pressed, the brook interface is selected.
* If the UP button is pressed at startup, the Doppelganger starts in bootloader mode.
* If the DOWN and LEFT buttons are pressed at startup, the Doppelganger starts in NES/SNES mode
* If the DOWN and RIGHT buttons are pressed at startup, the Doppelganger starts in Saturn mode

## RJ45 cabling

SNES/NES:

| xNES pin | RJ45 pin | RJ45 function |
|----------|----------|---------------|
| LATCH    |        7 | /SS           |
| CLOCK    |        2 | SCK           |
| DATA     |        4 | SDO           |
| VCC      |        8 | VCC           |
| GND      |        1 | GND           |

USB:

| USB pin | RJ45 pin | RJ45 function |
|---------|----------|---------------|
| D+      |        6 | D+            |
| D-      |        5 | D-            |
| VCC     |        8 | VCC           |
| GND     |        1 | GND           |

SATURN:

| Saturn pin | RJ45 pin | RJ45 function |
|------------|----------|---------------|
| S0         |        5 | D-   	        |
| S1         |        6 | D+            |
| D0         |        2 | SCK           |
| D1         |        4 | SDO           |
| D2         |        7 | /SS           |
| D3         |        3 | SDI           |
| VCC        |        8 | VCC           |
| GND        |        1 | GND           |

Note: the Saturn support needs a hardware bug fix. Please refer to the hardware project bug 2.

## Acknowelegement

Shamelessy stolen from:

* Pierre Gaufillet for the [Vasco PUF](https://github.com/pgaufillet/vasco). Used in the toolchain installation makefile.
* Regis Blanchot for the [Pinguino](http://pinguino.cc). Used in the bootloader and some compilation scripts
* Alan Ott for the [M-Stack](http://www.signal11.us/oss/m-stack/) USB framework
* Toodle for the [MC-Cthulu](http://forums.shoryuken.com/discussion/46572/the-official-cthulhu-and-chimp-thread-try-our-new-dreamcast-flavor), the UPCB and ... the main idea !
* Fluxcorenz: for the [UPCB](https://github.com/fluxcorenz/UPCB) on github

I tried (as much as possible) to mentioned the inspiration source in each source file header.

## Install

See the INSTALL.md file in the same directory.

## TODO

* Game system auto-dectect at startup
* Support for more systems

## Licence

Since Doppelganger is built on the top of several projects with different licenses, I choosed what seems to be the only compatible solution : GPLV3. This choice was made according to the following [wikipedia page](https://en.wikipedia.org/wiki/License_compatibility). If any of the original authors does not agree with that choice, please contact me: debarge.cedric AT gmail.com.
