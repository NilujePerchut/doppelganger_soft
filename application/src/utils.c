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

#include <delay.h>
#include "utils.h"

void delayms(unsigned short milliseconds)
{
	unsigned short n100tcy;
	n100tcy = SYSTEM_PERIPH_FREQUENCY/100000;
	while (milliseconds--)
		delay100tcy(n100tcy-3);
}
