/**********************************************************************
	Title:			Doppelganger
	File:			key_map.h
	Descr.:			Joystick and buttons mapping
	Stolen from:	No one I can think of
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#ifndef KEY_MAP_H
#define KEY_MAP_H

/* This file aims to define correspondance between io pins and controls
 * This mapping is valid against Doppelganger RevA0 */

/* User Controls*/
#define PCB_SQUARE		PORTAbits.RA6
#define PCB_TRIANGLE	PORTCbits.RC0
#define PCB_CIRCLE		PORTAbits.RA7
#define PCB_CROSS		PORTAbits.RA4
#define PCB_UP			PORTDbits.RD5
#define PCB_DOWN		PORTDbits.RD6
#define PCB_LEFT		PORTDbits.RD7
#define PCB_RIGHT		PORTBbits.RB2
#define PCB_L1			PORTEbits.RE0
#define PCB_L2			PORTEbits.RE2
#define PCB_L3			PORTBbits.RB4
#define PCB_R1			PORTCbits.RC1
#define PCB_R2			PORTEbits.RE1
#define PCB_R3			PORTCbits.RC2
#define PCB_START		PORTBbits.RB5
#define PCB_SELECT		PORTDbits.RD4
#define PCB_HOME		PORTAbits.RA0
#define PCB_DISABLE		PORTAbits.RA1

/* Std PS3 controller mapping */
#define STD_START		PCB_START
#define STD_SELECT		PCB_SELECT
#define STD_UP			PCB_UP
#define STD_DOWN		PCB_DOWN
#define STD_LEFT		PCB_LEFT
#define STD_RIGHT		PCB_RIGHT
#define STD_LP			PCB_SQUARE
#define STD_MP			PCB_TRIANGLE
#define STD_HP			PCB_R1
#define STD_3P			PCB_L1
#define STD_LK			PCB_CROSS
#define STD_MK			PCB_CIRCLE
#define STD_HK			PCB_R2
#define STD_3K			PCB_L2

/* Serial interface */
#define SERIAL_CS		PORTAbits.RA5
#define SERIAL_SDI		PORTBbits.RB0
#define SERIAL_SDO		PORTBbits.RB3
#define SERIAL_SCK		PORTBbits.RB1
#define SERIAL_CS_TRIS	TRISAbits.RA5
#define SERIAL_SDI_TRIS	TRISBbits.RB0
#define SERIAL_SDO_TRIS	TRISBbits.RB3
#define SERIAL_SCK_TRIS	TRISBbits.RB1

/* USB interface */
#define USB_N			PORTAbits.RA2
#define USB_P			PORTAbits.RA3
#define USB_N_TRIS		TRISAbits.RA2
#define USB_P_TRIS		TRISAbits.RA3

/* LEDs */
#define LED_L			LATDbits.LATD0
#define LED_R			LATDbits.LATD1

/* USB Source Selector */
enum usb_src {
	USB_SOURCE_NONE = 0,
	USB_SOURCE_OLDIES,
	USB_SOURCE_BROOK,
};
#define USB_SRC_OLDIES	LATDbits.LATD3
#define USB_SRC_BROOK	LATDbits.LATD2

/* Input direction */
#define INPUT			1
#define OUTPUT			0

/* This macro defines the key condition used to disable the brook at startup */
#define OLDIES_STARTUP	(!PCB_DOWN && !PCB_LEFT && !PCB_L3)

#endif
