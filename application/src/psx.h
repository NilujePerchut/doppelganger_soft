/**********************************************************************
	Title:			Doppelganger
	File:			psx.h
	Descr.:			PS1, PS2 protocol handlers (headers)
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#define PSX_DATA					SERIAL_SDO
#define PSX_COMMAND					SERIAL_SDI
#define PSX_ATT						SERIAL_CS
#define PSX_CLOCK					SERIAL_SCK
#define PSX_ACK						USB_N

#define PSX_DATA_TRIS				SERIAL_SDO_TRIS
#define PSX_COMMAND_TRIS			SERIAL_SDI_TRIS
#define PSX_ATT_TRIS				SERIAL_CS_TRIS
#define PSX_CLOCK_TRIS				SERIAL_SCK_TRIS
#define PSX_ACK_TRIS				USB_N_TRIS

#define SPI_SLAVE_MODE_WITH_CS		0x04

/* Byte 1 */
#define SPI_DATA_IDLE				0xFF
#define SPI_COMMAND_NEW_PACKET		0x80	/* 0x80 == 0x01 LSB First */
/* Byte 2 */
#define SPI_COMMAND_MAIN_COMMAND	0x42	/* 0x42 == 0x42 LSB First */
#define SPI_DATA_DEVICE_MODE		0x82	/* 0x82 == 0x41 LSB First */
/* Byte 3 */
#define SPI_DATA_MAGIC				0x5A	/* 0x5A == 0x5A LSB First */

void psxApp(void);
