/**********************************************************************
	Title:			Doppelganger
	File:			saturn.h
	Descr.:			Saturn protocol handler (header)
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/
#include "key_map.h"

/* UPCB Sega Saturn module
	data from GameSX.com
	 1       9
	 _________
	/         \
	|=========| (at the Sega Saturn)
	+---------+

	Pin	Name	Function   cable color
	1	VCC		+5v (Out)
	2	D1		Data 1
	3	D0		Data 0
	4	S0		Select 0
	5	S1		Select 1
	6	5v		+5v (Inp)
	7	D3		Data 3
	8	D2		Data 2
	9	GND		Ground

	  ctrl		Saturn signal	RJ45
	1 blanc		--> Ground		--> 1		1	Ground	Blanc
	2 gris		--> D2			--> 7		2	D0		Noir
	3 jaune		--> D3			--> 3		3	D3		Jaune
	4 marron	--> 5v in		--> NC		4	D1		Vert
	5 rouge		--> S1			--> 6		5	S0		Orange
	6 orange	--> S0			--> 5		6	S1		Rouge
	7 noir		--> D0			--> 2		7	D2		Gris
	8 vert		--> D1			--> 4		8	5V		Bleu
	9 bleu		--> 5V out		--> 8
*/

#define SELECT_MASK	0x0C /* RA2 & RA3 */

#define SATURN_D0 SERIAL_SCK
#define SATURN_D1 SERIAL_SDO
#define SATURN_D2 SERIAL_CS
#define SATURN_D3 SERIAL_SDI
#define SATURN_S0 USB_N
#define SATURN_S1 USB_P

#define SATURN_D0_OUT	LATBbits.LATB1
#define SATURN_D1_OUT	LATBbits.LATB3
#define SATURN_D2_OUT	LATAbits.LATA5
#define SATURN_D3_OUT	LATBbits.LATB0

#define SATURN_TRIS_D0 SERIAL_SCK_TRIS
#define SATURN_TRIS_D1 SERIAL_SDO_TRIS
#define SATURN_TRIS_D2 SERIAL_CS_TRIS
#define SATURN_TRIS_D3 SERIAL_SDI_TRIS
#define SATURN_TRIS_S0 USB_N_TRIS
#define SATURN_TRIS_S1 USB_P_TRIS

/* 1st digit : S0, 2nd digit : S1 */
#define CYCLE_HH 0x0C
#define CYCLE_LL 0x00
#define CYCLE_HL 0x04
#define CYCLE_LH 0x08

#define APPLY_LATS do { LATA = pa_next; LATB = pb_next; } while(0)

/* Fill the nexp_pa and next_pb */
#define FILL_NEXT(DZERO, DONE, DTWO, DTHREE)	do { pa_next = (DTWO<<5); \
								pb_next = (DZERO<<1) | (DONE<<3) | (DTHREE); } while(0)


#define STATE_RECHECK	pa_next = 0xFF;						\
						pb_next = 0xFF;						\
						LED_L = 1;							\
						if(SATURN_S0) {						\
							if(SATURN_S1) {					\
								LED_L = 0;					\
								goto DO_HH;					\
							} else {						\
								LED_L = 0;					\
								goto DO_HL;					\
							}								\
						} else {							\
							if(SATURN_S1) {					\
								LED_L = 0;					\
								goto DO_LH;					\
							} else {						\
								LED_L = 0;					\
								goto DO_LL;					\
							}								\
						} \

void saturnApp(void);
