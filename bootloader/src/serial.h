/**********************************************************************
	Title:			Doppelganger Bootloader
	File:			serial.h
	Descr.:			Serial debugging functions (headers)
	Stolen from:	Pinguino
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#ifndef _SERIAL_H_
#define _SERIAL_H_

void SerialInit(u32);
void SerialPrintChar(char);
void SerialPrint(const char *);
void SerialPrintLN(const char *);
void SerialPrintNumber(s16, u8);

#endif // _SERIAL_H_
