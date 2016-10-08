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

#ifndef KEY_MAP_H
#define KEY_MAP_H

/* This file aims to define correspondance between io pins and controls
 * This mapping is valid against Doppelganger RevA0 */

/* User Controls
 * On RevA0 the pcb overlay is wrong. R3(RA7) and Circle(RC2) are inverted.
 * This is also the case for Square(RC0) and Triangle(RA6).
 * The following code already corrects this */
#define PCB_SQUARE		PORTAbits.RA7
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

/* Serial interface */
#define SERIAL_CS		PORTAbits.RA5
#define SERIAL_SDI		PORTBbits.RB0
#define SERIAL_SDO		LATBbits.LATB3
#define SERIAL_SCK		PORTBbits.RB1
#define SERIAL_CS_TRIS	TRISAbits.RA5
#define SERIAL_SDI_TRIS	TRISBbits.RB0
#define SERIAL_SDO_TRIS	TRISBbits.RB3
#define SERIAL_SCK_TRIS	TRISBbits.RB1

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
