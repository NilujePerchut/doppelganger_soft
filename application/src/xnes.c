/**********************************************************************
	Title:			Doppelganger
	File:			xnes.c
	Descr.:			SNES and NES protocol handlers
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#include <pic18fregs.h>
#include <string.h>
#include "debug.h"
#include "key_map.h"
#include "xnes.h"

unsigned char type;
unsigned char type_max;
volatile unsigned char sck_count;
unsigned char mapping[XNES_MAX_KEYS];

static void xnesInitPins(void)
{
	debug_print("Enters xnesInitPins\r\n");
	XNES_LATCH_TRIS = INPUT;
	XNES_SDI_TRIS = INPUT;
	XNES_SCK_TRIS = INPUT;
	XNES_SO = 1; /* Idle State */
	XNES_SO_TRIS = OUTPUT;
	/* Disable all interruptions */
	INTCONbits.GIE = 0;
}

static void xnesDetect(void)
{
	unsigned char tries;
	debug_print("Enters xnesDetect\r\n");
	type = XNES_TYPE_UNDETECTED;

	/* Configure the INT1 interruption */
	INTCON2bits.INTEDG1 = 1; /* Rising Edge of INT1*/
	INTCON3bits.INT1IE = 1; /* Enables INT1 */
	INTCON3bits.INT1P = 1; /* High priority */

	/* Wait for latch pulse */
	tries = 0;
	while ((XNES_TYPE_UNDETECTED == type) && (tries<DETECT_TRIALS_MAX)){
		debug_print("Wait pulse\r\n");
		while(XNES_LATCH);
		while(!XNES_LATCH);
		while(XNES_LATCH);

		/* Try a new version with interruptions */
		sck_count = 0;
		INTCONbits.GIE = 1;
		/* Wait a cycle and count the number of clk ticks */
		while (!XNES_LATCH);
		INTCONbits.GIE = 0;

		debug_print("Exits detect loop for ");
		debug_print_val(sck_count);
		switch(sck_count) {
			case (XNES_SNES_KEY_NUM):
				debug_print("Found SNES\r\n");
				type = XNES_TYPE_SNES;
				type_max = XNES_SNES_KEY_NUM;
				break;
			case (XNES_NES_KEY_NUM):
				debug_print("Found NES\r\n");
				type = XNES_TYPE_NES;
				type_max = XNES_NES_KEY_NUM;
				break;
			default:
				debug_print("Found unknown\r\n");
				debug_print_val(sck_count);
				type = XNES_TYPE_UNDETECTED;
				type_max = 0;
				break;
		}
		tries++;
	}
}

static unsigned char xnesSendBit(unsigned char data)
{
	while (!XNES_SCK) {
		if (XNES_LATCH)
			return 1;
	}
	XNES_SO=data;
	while (XNES_SCK) {
		if (XNES_LATCH)
			return 1;//wait for falling edge
	}
	return 0;
}

static void snesFillMap(void)
{
	memset(mapping, 1, XNES_MAX_KEYS);

	mapping[0] = PCB_CROSS; /* B = LK = cross  */
	mapping[1] = PCB_SQUARE; /* Y = LP = square */
	mapping[8] = PCB_CIRCLE; /* A = MK = circle */
	mapping[9] = PCB_TRIANGLE; /* X = MP = triangle */
	mapping[10] = PCB_R1; /* L = HP = R1 */
	mapping[11] = PCB_R2; /* R = HK = R2 */
	/* Check for 3P or 3k */
	if (!PCB_L1) {
		/* 3P */
		mapping[1] = 0;
		mapping[9] = 0;
		mapping[10] = 0;
	}
	if (!PCB_L2) {
		/* 3P */
		mapping[0] = 0;
		mapping[8] = 0;
		mapping[11] = 0;
	}

	/* Common */
	if (!PCB_DISABLE) {
		mapping[2] = PCB_SELECT; /* SELECT */
		mapping[3] = PCB_START; /* START */
	}
	mapping[4] = PCB_UP; /* UP */
	mapping[5] = PCB_DOWN; /* DOWN */
	mapping[6] = PCB_LEFT; /* LEFT */
	mapping[7] = PCB_RIGHT; /* RIGHT */
}

static void process_nes(void)
{
	unsigned char bU, bD, bL, bR, bSt, bSe, bB, bA;
	bU=bD=bL=bR=bSt=bSe=bB=bA=1;
	XNES_SO=1;

	while (!XNES_LATCH) {} //latch high=start of data

	while(1) {
		//Latch should be high at this point.
		//send the first output (A) until the first clock.
		XNES_SO=bA;
		while(XNES_SCK) {} //wait until clock is low (value read by NES)

		//now wait for the nes clocks, and send the remaining
		//outputs in order (B,Sel,St,U,D,L,R)
		while(!XNES_SCK) {} //clock high = set data
		XNES_SO=bB; //start sending next bit of data
		while(XNES_SCK) {} //clock low = NES reads data

		while(!XNES_SCK) {}
		XNES_SO=bSe;
		while(XNES_SCK) {}

		while(!XNES_SCK) {}
		XNES_SO=bSt;
		while(XNES_SCK) {}

		while(!XNES_SCK) {}
		XNES_SO=bU;
		while(XNES_SCK) {}

		while(!XNES_SCK) {}
		XNES_SO=bD;
		while(XNES_SCK) {}

		while(!XNES_SCK) {}
		XNES_SO=bL;
		while(XNES_SCK) {}

		while(!XNES_SCK) {}
		XNES_SO=bR;
		while(XNES_SCK) {} //the final clock signals the end of this data.

		XNES_SO=0;

		while (1) { //Now just buffer the inputs until we're latched again
			bA=PCB_CIRCLE; if (XNES_LATCH) break;
			bB=PCB_CROSS; if (XNES_LATCH) break;

			if (!PCB_DISABLE) {
				bSe=PCB_SELECT; if (XNES_LATCH) break;
				bSt=PCB_START; if (XNES_LATCH) break;
			} else {
				bSe=1; if (XNES_LATCH) break;
				bSt=1; if (XNES_LATCH) break;
			}
			bU=PCB_UP; if (XNES_LATCH) break;
			bD=PCB_DOWN; if (XNES_LATCH) break;
			bL=PCB_LEFT; if (XNES_LATCH) break;
			bR=PCB_RIGHT; if (XNES_LATCH) break;
		}
	}
}

static void process_snes(void)
{
	unsigned char i;

	debug_print("Enters main loop for snes\r\n");
	/* Wait a rising edge of latch */
	while(!XNES_LATCH);

	while(1) {
		XNES_SO = 1;

		while(XNES_LATCH);

		i = 0;
		XNES_SO = mapping[i]; /* B is issue at latch falling edge */
		while(XNES_SCK); /* Wait until SCK falling edge */

		for (i=1;i<type_max;i++) {
			if (xnesSendBit(mapping[i]))
				break;
		}
		if (i<type_max)
			continue;

		snesFillMap();

		/* Wait a rising edge of latch */
		while(!XNES_LATCH);
	}
}

/* lets assume this is a snes for now */
void xnesApp(void)
{
	debug_print("Enters xnesApp\r\n");
	memset(mapping, 1, XNES_MAX_KEYS);
	type = XNES_TYPE_UNDETECTED;
	type_max = 0;
	xnesInitPins();
	xnesDetect();

	if (XNES_TYPE_NES == type)
		process_nes();
	else
		process_snes();

}
