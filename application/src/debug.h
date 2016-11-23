/**********************************************************************
	Title:			Doppelganger
	File:			debug.h
	Descr.:			Serial debugging related headers
	Stolen from:	M-STACK USB framework
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#define GPIO_INPUT		1
#define GPIO_OUTPUT		0

#define EUSART_ASYNC	0	/* Asynchronus mode */
#define EUSART_SYNC		1	/* Synchronous mode */

#define EUSART_DISABLED		0
#define EUSART_ENABLED		1

#define EUSART_IDLE_HIGHT	0
#define EUSART_IDLE_LOW		1

#define EUSART_TX_DISABLED   0
#define EUSART_TX_ENABLED   1

void debug_init(void);
void debug_putchr(char c);
void debug_print(const char *str);
void debug_print_val(unsigned short c);

#endif
