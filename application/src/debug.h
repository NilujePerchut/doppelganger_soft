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

#ifndef _DEBUG_H
#define _DEBUG_H

#define GPIO_INPUT		1
#define GPIO_OUTPUT		0

#define EUSART_ASYNC	0	/* Asynchronus mode */
#define EUSART_SYNC		1	/* Synchronous mode */

#define EUSART_DISABLED		0
#define EUSART_ENABLED		1

#define EUSART_IDLE_HIGHT	0
#define EUSART_IDLE_LOW		1

#define EUSART_TX_DISABLED   0
#define EUSART_TX_ENABLED   1

void debug_init(void);
void debug_putchr(char c);
void debug_print(const char *str);
void debug_print_val(unsigned short c);

#endif
