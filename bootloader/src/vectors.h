/**********************************************************************
	Title:			Doppelganger Bootloader
	File:			vectors.h
	Descr.:			PIC interrupt vectors (headers)
	Stolen from:	Pinguino
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#ifndef _VECTORS_H
#define _VECTORS_H

extern void main(void);

#ifdef __XC8__

    #ifdef _PIC14E

        void interrupt PIC16F_isr(void);

    #else

        void interrupt high_priority high_priority_isr(void);
        void interrupt low_priority  low_priority_isr(void);

    #endif

#else // SDCC

    extern int stack_end;

    #ifdef __SDCC_pic14

    void startup(void) __naked;
    void reset_isr(void) __naked __interrupt 0;
    void PIC16F_isr(void) __naked __interrupt 1;

    #else

    void           startup(void) __naked;
    void         reset_isr(void) __naked __interrupt 0;
    void high_priority_isr(void) __naked __interrupt 1;
    void  low_priority_isr(void) __naked __interrupt 2;

    #endif

#endif

#endif /* _VECTORS_H */
