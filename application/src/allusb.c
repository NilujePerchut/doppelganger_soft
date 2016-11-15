/*
UPCB - Universal Programmed Controller Board 
Copyright (C) 2007  Marcus Post marcus@marcuspost.com
Based on USB code framework 'lab2' Copyright (C) 2005 Bradley A. Minch

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <pic18fregs.h>
#include "allusb.h"

/* #pragma udata USB_BUFFER_DESCRIPTOR_TABLE=0x0400*/
#pragma udata usbram4 BDPTR
BUFDESC_t BDPTR[32];

/* #pragma udata usbram5=0x0500 */
#pragma udata usbram5 USBBUFFER5
unsigned char USBBUFFER5[0x100];

unsigned char USB_curr_config;
unsigned char USB_idle_rate;
unsigned char USB_USWSTAT;
unsigned char USB_device_status;
unsigned char USB_protocol;
volatile unsigned char USB_dev_req;

volatile unsigned char USB_buffer_data[8];
volatile unsigned char USB_error_flags;
unsigned char USB_address_pending;
unsigned char *USB_desc_ptr;
unsigned char USB_bytes_left;
unsigned char USB_packet_length;
volatile unsigned char USB_USTAT;
struct BUFDESC USB_buffer_desc;

/* Dummy functions to force use of BDPTR and USBBUFFER5
 * Without theses functionsn the optimizer will supress the BDPTR and USBBUFFER5
 * var from the scope and then cause a compilation error */
BUFDESC_t* get_bdptr_at_index(int i)
{
	return &BDPTR[i];
}

unsigned char* get_usbbuffer5_at_index(int i)
{
	return &USBBUFFER5[i];
}
