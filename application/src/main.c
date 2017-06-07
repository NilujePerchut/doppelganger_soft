/**********************************************************************
	Title:			Doppelganger
	File:			main.c Descr.:			Main program file
	Stolen from:	No one I can think of
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#include <pic18fregs.h>
#include "key_map.h"
#include "debug.h"
#include "utils.h"
#include "allusb.h"
#include "ps3usb.h"
#include "xnes.h"
#include "saturn.h"
#include "psx.h"
#include "ps3usb.h"

#pragma config XINST=OFF

extern volatile unsigned char sck_count;

static void SetUsbSource(enum usb_src src)
{
	/* Reset both USB selectors to avoid conflicts */
	TRISDbits.RD0 = OUTPUT;
	TRISDbits.RD1 = OUTPUT;
	TRISDbits.RD2 = OUTPUT;
	TRISDbits.RD3 = OUTPUT;
	LED_L = 0;
	LED_R = 0;
	USB_SRC_BROOK = 0;
	USB_SRC_OLDIES = 0;
	switch (src) {
		case USB_SOURCE_OLDIES:
			debug_print("Set oldies mode\r\n");
			LED_L = 1;
			LED_R = 0;
			USB_SRC_OLDIES = 1;
			USB_SRC_BROOK = 0;
			break;
		case USB_SOURCE_BROOK:
			debug_print("Set brook mode\r\n");
			LED_L = 0;
			LED_R = 1;
			USB_SRC_OLDIES = 0;
			USB_SRC_BROOK = 1;
			break;
		case USB_SOURCE_NONE:
			break;
		default:
			/* Nothing to do */
			debug_print("Found unknown mode\r\n");
			break;
	}
}

void isr_high(void) __interrupt 1
{
	if (INTCON3bits.INT1IF) {
		/* Only xSNES for the moment */
		sck_count++;
		INTCON3bits.INT1IF = 0;
	} else if (INTCONbits.TMR0IF) {
		INTCONbits.TMR0IF = 0;

		/* Detection timer expires. Just pop off the ISR return address from
		 * the stack to return to the timeout call. This must be done with
		 * interruption disabled */
		INTCONbits.GIE = 0;
		__asm
			pop
			/* pop */
		__endasm;
		INTCONbits.GIE = 1;
		/* Should return to the next instruction following the timeout call */
	}
}

static void IoInit(void)
{
	/* Set all input to digital mode */
	ANSELA = 0x00;
	ANSELB = 0x00;
	ANSELC = 0x00;
	ANSELD = 0x00;
	ANSELE = 0x00;
	ADCON1 = 0x0F;		//All pins as digital
	TRISA = 0xFF;		//set PortA in input
	TRISB = 0xFF;		//set PortB in input
	TRISC = 0xFF;		//set PortC in input
	TRISD = 0xF0;		//set PortD in input
	TRISE = 0x07;		//set PortE in input
}

int detect_process(void)
{
	/* This is the system detection function. Since some systems take several
	 * seconds to start sending something on their controller port, let order
	 * the detection to maximize the number of systems in the smallest amount
	 * of time */
	int res;

	/* NES/SNES */
	res = xnesAutoDetect();
	IoInit();
	if (res)
		return res;

	/* USB */

	/* PS1/PS2 */

	/* Saturn */

	return -1;
}

void main(void)
{
	int res;

	IoInit();

	debug_init();

	/* Disable USB by default */
	SetUsbSource(USB_SOURCE_NONE);

	/* If a mode is forced got for it */
	if (XNES_STARTUP) {
		LED_L = 1;
		xnesApp();
	} else if (SATURN_STARTUP) {
		LED_L = 1;
		saturnApp();
	} else if (PSX_STARTUP) {
		LED_L = 1;
		psxApp();
	} else if (PC_PS3_STARTUP) {
		LED_L = 1;
		SetUsbSource(USB_SOURCE_OLDIES);
		ps3usbApp();
	} else {
		/* First go for autodetection */
		res = detect_process();
		switch (res) {
			case FOUND_XNES:
				LED_L = 1;
				xnesApp();
				break;
			case -1:
			default:
				/* Fall back on Brook mode if autodetection failed */
				SetUsbSource(USB_SOURCE_BROOK);
				while(1); /* Just wait here forever */
		}
	}
}
