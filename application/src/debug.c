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
#include "debug.h"

/* Init the debug on serial link */
void debug_init(void)
{
	unsigned int i;

	/* Step 1 baudrate generator = 115200 */
	TXSTA1bits.SYNC = EUSART_ASYNC;
	BAUDCON1bits.BRG16 = 1;
	TXSTA1bits.BRGH = 1;
	/* This is a bug fix. The speed seems to be multiplied by 3 after a few
	 * byte time. So specify 115200 / 3 and wait before use the serial port */
	SPBRG1 = 103;	/* Low */
	SPBRGH1 = 0;	/* High */

	/* Step 2 */
	TRISCbits.RC6 = GPIO_INPUT;	/* Rx */
	TRISCbits.RC7 = GPIO_INPUT;	/* Tx */

	/* Step 3 */
	RCSTA1bits.SPEN = EUSART_ENABLED;

	/* Step 4 */
	TXSTA1bits.TXEN = EUSART_TX_ENABLED;

	/* Wait until the x3 bug happen */
	for (i=0; i<60000; i++);
}

void debug_putchr(char c)
{
	while(!TXSTA1bits.TRMT);
	TXREG1 = c;
}

void debug_print(const char *str)
{
	int i;
	for (i=0; str[i]; i++)
		debug_putchr(str[i]);
}

#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)
void debug_print_val(unsigned short c)
{
	char str[7];
	unsigned char tmp;

	tmp = (c & 0xF000)>>12;
	str[0] = TO_HEX(tmp);
	tmp = (c & 0x0F00)>>8;
	str[1] = TO_HEX(tmp);
	tmp = (c & 0x00F0)>>4;
	str[2] = TO_HEX(tmp);
	tmp = c & 0x000F;
	str[3] = TO_HEX(tmp);
	str[4] = '\r';
	str[5] = '\n';
	str[6] = '\0';

	debug_print(str);
}
