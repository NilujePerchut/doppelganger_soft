/**********************************************************************
	Title:			Doppelganger
	File:			utils.h
	Descr.:			Just a bunch of useful functions (headers)
	Stolen from:	No one I can think of
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#define SYSTEM_CPU_FREQUENCY 48000000L
#define SYSTEM_PERIPH_FREQUENCY (SYSTEM_CPU_FREQUENCY>>2)

void delayms(unsigned short milliseconds);
int timeout(void (*f)(void), unsigned short milliseconds);
