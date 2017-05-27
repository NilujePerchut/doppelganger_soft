/**********************************************************************
	Title:			Doppelganger
	File:			utils.c
	Descr.:			Just a bunch of useful functions
	Stolen from:	No one I can think of
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#include <pic18fregs.h>
#include <delay.h>
#include "utils.h"

#define TIMEOUT_TIMER0_PRESCALER 64
#define TIMEOUT_TIMER0_1MS ((SYSTEM_PERIPH_FREQUENCY/TIMEOUT_TIMER0_PRESCALER)\
							/ 1000)

volatile int timeout_result;

void delayms(unsigned short milliseconds)
{
	unsigned short n100tcy;
	n100tcy = SYSTEM_PERIPH_FREQUENCY/100000;
	while (milliseconds--)
		delay100tcy(n100tcy-3);
}

int timeout(void (*f)(void), unsigned short milliseconds)
{
	unsigned short n1ms;
	unsigned short data_ptr_org, data_ptr_new;

	data_ptr_org = 0;
	data_ptr_new = 0;

	/* Use timer 0 to count the timeout duration */
	T0CONbits.TMR0ON = 0;
	T0CONbits.T08BIT = 0; /* 16 bits */
	T0CONbits.T0CS = 0; /* Use Fosc */
	T0CONbits.T0SE = 1; /* Use Rising edge */
	T0CONbits.PSA = 0; /* Use prescaler */
	T0CONbits.T0PS = 5; /* 1:64 prescaler */

	data_ptr_org = FSR1H << 8 | FSR1L;

	timeout_result = 0;
	n1ms = 65535 - milliseconds * TIMEOUT_TIMER0_1MS;
	TMR0H = n1ms >> 8;
	TMR0L = n1ms && 0xFF;

	T0CONbits.TMR0ON = 1;
	INTCONbits.TMR0IE = 1;
	INTCONbits.GIE = 1;

	f();

	INTCONbits.TMR0IE = 0;
	T0CONbits.TMR0ON = 0;
	INTCONbits.GIE = 0;

	/* PREINC1 all the PREDEC1 inserted by f */
	data_ptr_new = FSR1H << 8 | FSR1L;
	while (data_ptr_org != data_ptr_new) {
		__asm
			MOVFF   PREINC1, r0x00
		__endasm;
		data_ptr_new = FSR1H << 8 | FSR1L;
	}

	return timeout_result;
}
