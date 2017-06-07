/**********************************************************************
	Title:			Doppelganger
	File:			xnes.h
	Descr.:			SNES and NES protocol handlers (headers)
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#define FOUND_XNES					1
/* The timeout duration is >> to a snes polling period in arder to account
 * of the snes / nes statup */
#define XNES_AUTO_DETECT_TIMEOUT_MS	500

#define XNES_SCK					SERIAL_SCK
#define XNES_SDI					SERIAL_SDI
#define XNES_SO						SERIAL_SDO
#define XNES_LATCH					SERIAL_CS

#define XNES_SCK_TRIS				SERIAL_SCK_TRIS
#define XNES_SDI_TRIS				SERIAL_SDI_TRIS
#define XNES_SO_TRIS				SERIAL_SDO_TRIS
#define XNES_LATCH_TRIS				SERIAL_CS_TRIS

#define DETECT_TRIALS_MAX			5

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
int xnesAutoDetect(void);
