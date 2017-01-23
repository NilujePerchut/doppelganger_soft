/**********************************************************************
	Title:			Doppelganger
	File:			psx.c
	Descr.:			PS1 PS2, protocol handlers
	Stolen from:	UPCB
	Comment:
***********************************************************************
	This file is part of Doppelganger:
		http://thewireddoesntexist.org/doppelganger
	Released under the GPLV3 license (see LICENCE file)
***********************************************************************/

#include <pic18fregs.h>
#include <delay.h>
#include "debug.h"
#include "key_map.h"
#include "psx.h"

static void init_spi(void)
{
	/* Disable all interruptions */
	INTCONbits.GIE = 0;
	INTCONbits.PEIE = 0;

	/* Reset to PON state */
	SSPSTAT  = 0x00;
	SSPCON   = 0x00;

	/* Selects mode */
	SSP1CON1bits.SSPM = SPI_SLAVE_MODE_WITH_CS;
	SSP1STATbits.CKE = 0;
	SSP1CON1bits.CKP = 1;

	PSX_ACK_TRIS	 = INPUT; /* For the moment. See psx_ack function */
	PSX_ATT_TRIS	 = INPUT;
	PSX_CLOCK_TRIS	 = INPUT;
	SSP1STATbits.SMP = 0;
	PSX_DATA_TRIS	 = OUTPUT;

	PSX_COMMAND_TRIS = INPUT;
	PSX_DATA_TRIS = OUTPUT;

	SSP1CON1bits.SSPEN = 1;
}

static void psx_ack(void)
{
	/* Generates an ACK cycle */
	PSX_ACK = 0;
	PSX_ACK_TRIS = OUTPUT;
	delay10tcy(3);
	PSX_ACK_TRIS = INPUT;
}

static void psx_main(void)
{
	unsigned char buttons;
	volatile unsigned char tmp;

	while(1) {
		/* Sit and sping until the PSX decides it wants to talk to us.
		The ATT line should be low for the duration of the transmission */
		while(PSX_ATT);

		LED_R = 1;
		/* Byte 1: Wait for the data to be received*/
		SSPBUF=SPI_DATA_IDLE;
		while (!SSPSTATbits.BF && !PSX_ATT);
		if ((SSPBUF != SPI_COMMAND_NEW_PACKET) || PSX_ATT)
			continue;

		/* Byte 2: Pre-Load the Playstation Digital Pad ID and
		 * wait for the data */
		SSPBUF = SPI_DATA_DEVICE_MODE;
		psx_ack();
		LED_R = 0;
		while (!SSPSTATbits.BF && !PSX_ATT);
		if ((SSPBUF != SPI_COMMAND_MAIN_COMMAND) || PSX_ATT)
			continue;

		/* At this point, the received command is valid. We just need to send
		 * data */
		SSPBUF = SPI_DATA_MAGIC;
		psx_ack();

		/* Map the first button byte */
		buttons = 0xFF;
		if (!PCB_SELECT)
			buttons &= 0x7F;
		if (!PCB_L3)
			buttons &= 0xBF;
		if (!PCB_R3)
			buttons &= 0xDF;
		if (!PCB_START)
			buttons &= 0xEF;
		if (!PCB_UP)
			buttons &= 0xF7;
		if (!PCB_RIGHT)
			buttons &= 0xFB;
		if (!PCB_DOWN)
			buttons &= 0xFD;
		if (!PCB_LEFT)
			buttons &= 0xFE;

		LED_R = 1;
		while (!SSPSTATbits.BF && !PSX_ATT);
		tmp = SSPBUF; /* To reset BF */
		/* all done, load the contructed byte into the buffer */
		SSPBUF = buttons;
		psx_ack();

		buttons=0xFF;
		if (!PCB_L2)
			buttons &= 0x7F; /* map extra1 to L2 */
		if (!PCB_L1)
			buttons &= 0xDF; /* map extra0 to L1 */
		if (!PCB_R2)
			buttons &= 0xBF; /* map RH to R2 */
		if (!PCB_R1)
			buttons &= 0xEF; /* map fierce to R1 */
		if (!PCB_TRIANGLE)
			buttons &= 0xF7; /*strong to tri */
		if (!PCB_CIRCLE)
			buttons &= 0xFB; /* forward to O */
		if (!PCB_CROSS)
			buttons &= 0xFD; /* short to X */
		if (!PCB_SQUARE)
			buttons &= 0xFE; /* jab to square */

		LED_R = 0;
		while (!SSPSTATbits.BF && !PSX_ATT);
		/* X-Changer-CD skips the last byte, In this case we need to cancel the
		 * ack. */
		delay10tcy(7); /* around  5 us  */
		if (PSX_ATT)
			/* X-Changer-CD case */
			continue;
		tmp = SSPBUF; /* To reset BF */

		SSPBUF=buttons;
		psx_ack();

		LED_R = 1;
		while (!SSPSTATbits.BF && !PSX_ATT);
		while(!PSX_ATT);
	}
}

void psxApp(void)
{
	debug_print("PSX init done\r\n");
	init_spi();
	debug_print("SPI init done\r\n");
	psx_main();
}
