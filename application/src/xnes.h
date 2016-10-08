/*
 * USB HID Joystick with old consoles support
 *
 * This file may be used by anyone for any purpose and may be used as a
 * starting point making your own application using M-Stack.
 *
 * It is worth noting that M-Stack itself is not under the same license as
 * this file.
 *
 * M-Stack is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  For details, see sections 7, 8, and 9
 * of the Apache License, version 2.0 which apply to this file.  If you have
 * purchased a commercial license for this software from Signal 11 Software,
 * your commerical license superceeds the information in this header.
 *
 * Alan Ott
 * Signal 11 Software
 * 2013-08-13
 */

#define XNES_SCK            SERIAL_SCK
#define XNES_SDI            SERIAL_SDI
#define XNES_SO             SERIAL_SDO
#define XNES_LATCH          SERIAL_CS

#define XNES_SCK_TRIS       SERIAL_SCK_TRIS
#define XNES_SDI_TRIS       SERIAL_SDI_TRIS
#define XNES_SO_TRIS        SERIAL_SDO_TRIS
#define XNES_LATCH_TRIS     SERIAL_CS_TRIS

enum XNES_TYPES {
	XNES_TYPE_UNDETECTED = 0,
	XNES_TYPE_SNES,
	XNES_TYPE_NES,
	/* Do not add anything after */
	XNES_TYPE_MAX
};

#define XNES_MAX_KEYS       16  /* SNES + IDLE bits */
#define XNES_SNES_KEY_NUM   16  /* SNES Keys Number */
#define XNES_NES_KEY_NUM    8   /* NES Keys Number */

void xnesApp(void);
