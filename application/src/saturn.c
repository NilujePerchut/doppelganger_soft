/**********************************************************************
	Title:			Doppelganger
	File:			saturn.c
	Descr.:			Saturn protocol handler
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#include <pic18fregs.h>
#include "key_map.h"
#include "saturn.h"
#include "utils.h"
#include "debug.h"

static inline void detect_pass(void)
{
	if (SATURN_S0 && SATURN_S1) {
		SATURN_D0_OUT = 0;
		SATURN_D1_OUT = 0;
		/*SATURN_D2_OUT = 1;
		SATURN_D3_OUT = 1;*/
	} else {
		SATURN_D0_OUT = 1;
		SATURN_D1_OUT = 1;
		/*SATURN_D2_OUT = 1;
		SATURN_D3_OUT = 1;*/
	}
}

static inline void Saturn_Init(void) {
	/* Disable all interruptions */
	INTCONbits.GIE = 0;

	SATURN_TRIS_S0 = INPUT;
	SATURN_TRIS_S1 = INPUT;

	/* Set D0 and D1 to 0 at startup */
	SATURN_D0_OUT = 0;
	SATURN_D1_OUT = 0;
	SATURN_D2_OUT = 1;
	SATURN_D3_OUT = 1;
	SATURN_TRIS_D0 = OUTPUT;
	SATURN_TRIS_D1 = OUTPUT;
	SATURN_TRIS_D2 = OUTPUT;
	SATURN_TRIS_D3 = OUTPUT;

	while( !(!SATURN_S0 && !SATURN_S1) )
		detect_pass();
 }

static inline void naive_approch(void)
{
	unsigned char pa_next, pb_next;

	while (1) {
		/* State is currently LL, prepare for HH */
		if ((PORTA & SELECT_MASK) != CYCLE_LL) {
			STATE_RECHECK
		}
		FILL_NEXT(0, 0, 1, PCB_L1);
		while ((PORTA & SELECT_MASK) == CYCLE_LL);
	DO_HH:
		APPLY_LATS;

		/* State is currently HH, prepare for LH */
		if ((PORTA & SELECT_MASK) != CYCLE_HH) {
			STATE_RECHECK
		}
		FILL_NEXT(PCB_UP, PCB_DOWN, PCB_LEFT, PCB_RIGHT);
		while((PORTA & SELECT_MASK) == CYCLE_HH);
	DO_LH:
		APPLY_LATS;

		/* State is currently LH, prepare for HL */
		if ((PORTA & SELECT_MASK) != CYCLE_LH) {
			STATE_RECHECK
		}
		FILL_NEXT(PCB_CIRCLE, PCB_R2, PCB_CROSS, PCB_START);
		while((PORTA & SELECT_MASK) == CYCLE_LH);
	DO_HL:
		APPLY_LATS;

		/* State is currently HL, prepare for LL */
		if ((PORTA & SELECT_MASK) != CYCLE_HL) {
			STATE_RECHECK
		}
		FILL_NEXT(PCB_R1, PCB_TRIANGLE, PCB_SQUARE, PCB_L2);
		while((PORTA & SELECT_MASK) == CYCLE_HL);
	DO_LL:
		APPLY_LATS;
	}
}

void saturnApp(void) {
	Saturn_Init();
	naive_approch();
}
