/*
 * USB HID Joystick with old consoles support
 *
 * This file may be used by anyone for any purpose and may be used as a
 * starting point making your own application using M-Stack.
 *
 * It is worth noting that M-Stack itself is not under the same license as
 * this file.
 *
 * M-Stack is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  For details, see sections 7, 8, and 9
 * of the Apache License, version 2.0 which apply to this file.  If you have
 * purchased a commercial license for this software from Signal 11 Software,
 * your commerical license superceeds the information in this header.
 *
 * Alan Ott
 * Signal 11 Software
 * 2013-08-13
 */

#include <pic18fregs.h>
#include <string.h>
#include "debug.h"
#include "key_map.h"
#include "xnes.h"

unsigned char type;
unsigned char type_max;
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
	unsigned char i;
	unsigned char cur_sck;
	unsigned char last_sck;

	debug_print("Enters xnesDetect\r\n");

	/* Wait a cycle and count the number of clk ticks */

	/* Wait for latch pulse */
	debug_print("Wait pulse\r\n");
	i = 0;
	XNES_SO = 0;
	while(XNES_LATCH);
	while(!XNES_LATCH);
	while(XNES_LATCH);

	last_sck = XNES_SCK;
	while (!XNES_LATCH) {
		cur_sck = XNES_SCK;
		if ((last_sck==0) && (cur_sck==1)) {
			i++;
		}

		last_sck = cur_sck;
	}
	debug_print("Exits detect loop\r\n");
	switch(i) {
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
			debug_print_val(i);
			type = XNES_TYPE_UNDETECTED;
			type_max = 0;
			break;
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

static void xnesFillMap(unsigned char type)
{
	memset(mapping, 1, XNES_MAX_KEYS);

	switch(type){
		case XNES_TYPE_UNDETECTED:
			return;

		case XNES_TYPE_SNES:
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
			break;
		case XNES_TYPE_NES:
			mapping[0] = PCB_CIRCLE; /* A = MK = circle */
			mapping[1] = PCB_CROSS; /* B = LK = cross  */
			break;
	}

	/* Common */
	mapping[2] = PCB_SELECT; /* SELECT */
	mapping[3] = PCB_START; /* START */
	mapping[4] = PCB_UP; /* UP */
	mapping[5] = PCB_DOWN; /* DOWN */
	mapping[6] = PCB_LEFT; /* LEFT */
	mapping[7] = PCB_RIGHT; /* RIGHT */
}

/* lets assume this is a snes for now */
void xnesApp(void)
{
	unsigned char i;

	debug_print("Enters xnesApp\r\n");
	memset(mapping, 1, XNES_MAX_KEYS);
	type = XNES_TYPE_UNDETECTED;
	type_max = 0;
	xnesInitPins();
	xnesDetect();

	debug_print("Enters main loop for xnes\r\n");
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

		xnesFillMap(type);

		/* Wait a rising edge of latch */
		while(!XNES_LATCH);
	}
}
