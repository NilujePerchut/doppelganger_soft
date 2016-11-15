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
#include "debug.h"
#include "ps3usb.h"
#include "allusb.h"
#include "key_map.h"

/* From allusb.h */
extern volatile BUFDESC_t BDPTR[32];
extern volatile unsigned char USBBUFFER5[0x100];

extern volatile unsigned char USB_USTAT;
extern volatile unsigned char USB_dev_req;
extern unsigned char USB_bytes_left;
extern volatile unsigned char USB_buffer_data[8];
extern unsigned char USB_packet_length;
extern unsigned char USB_curr_config;
extern unsigned char USB_idle_rate;
extern unsigned char USB_USWSTAT;
extern unsigned char USB_device_status;
extern unsigned char USB_protocol;
extern volatile unsigned char USB_error_flags;
extern unsigned char USB_address_pending;
extern unsigned char *USB_desc_ptr;
extern struct BUFDESC USB_buffer_desc;

unsigned char PS3USB_USBbuffer[PS3USB_REPORT_SIZE];
unsigned char SETADDR_BEFORE_GETDESCR;
unsigned char GETDESCR_BEFORE_SETADDR;
unsigned char PS3_DETECTED;

//Updated for VSHG - checked
const unsigned char PS3USB_Device_Descriptor[] = {
	0x12,		//size of this descriptor in bytes
	DEVICE,		//descriptor type = PS3USB_Device_Descriptor
	0x10, 0x01,	//USB Spec 2.0
	0x00,		//class code
	0x00,		//subclass code
	0x00,		//protocol code
	0x40,		//EPO buffer size
	0xC4, 0x10,	//vendor ID
	0xC0, 0x82, //product ID
	//0xC5, 0x11,	//vendor ID
	//0xC1, 0x83, //product ID
	0x06, 0x00, //device release number
	0x01,		//manufacturer string index
	0x02,		//product string index
	0x00,		//device serial number string index
	0x01		//# of possible configurations
};

const unsigned char PCUSB_Device_Descriptor[] = {
	0x12,		//size of this descriptor in bytes
	DEVICE,		//descriptor type = PS3USB_Device_Descriptor
	0x10, 0x01,	//USB Spec 2.0
	0x00,		//class code
	0x00,		//subclass code
	0x00,		//protocol code
	0x40,		//EPO buffer size
	0xD8, 0x04,	//vendor ID 04D8
	REVISION_BYTE, CONFIG_BYTE, //product ID
	0x06, 0x00, //device release number
	0x01,		//manufacturer string index
	0x02,		//product string index
	0x00,		//device serial number string index
	0x01		//# of possible configurations
};


//partially updated to match VSHG; checked
const unsigned char PS3USB_Configuration_Descriptor[] = {
	0x09,			//size of this desc in bytes
	CONFIGURATION, //type of desc
	0x22,	0x00,		//size of total descriptor
	0x01,			//# of interfaces
	0x01,			//index number of this config
	0x00,			//config string index
	0x80,			//attributes:Default
	0x32,			//power consumption * 2mA
	//Interface portion
	0x09,			//size of this desc in bytes
	INTERFACE,		//type of desc
	0x00,			//interface number (ordinal)
	0x00,			//alternate setting number
	0x01,			//number of endpoints (not including EP0)
	0x03,			//Interface class - HID
	0x00,			//sublass
	0x00,			//protocol code - mouse
	0x00,			//interface string index

	//HID portion
	0x09,			//size of this descriptor in bytes
	HID,			//type of descriptor: HID
	0x11, 0x01,	//HID spec release number
	0x00,			//country code (0x00 for not supported)
	0x01,			//number of class descriptors
	REPORT,			//Report descriptor type
	0x70, 0x00,		//size of report descriptor

	//endpoint portion
	0x07,		//size of desc in bytes
	ENDPOINT,	//type od desc
	0x81,		//endpoint address EP1 IN
	0x03,		//transfer style: interrupt
	0x40, 0x00,	//max packet size : 64 bytes
	0x0A,		//interval: 10ms
};

//HID report from VSHG; need to double check.
const unsigned char PS3USB_Report_Descriptor[] = {
	0x05, 0x01,					// USAGE_PAGE (Generic Desktop)
	0x09, 0x05,					//Usage gamepad
	0xA1, 0x01,					// COLLECTION (Application)
	0x15, 0x00,					//   LOGICAL_MINIMUM (0)
	0x25, 0x01,					//   LOGICAL_MAXIMUM (1) --10
	0x35, 0x00,					//   PHYSICAL_MINIMUM (0)
	0x45, 0x01,						 //	PHYSICAL_MAXIMUM (1)
	0x75, 0x01,						//   REPORT_SIZE (1)
	0x95, 0x0d,					//   REPORT_COUNT (13)
	0x05, 0x09,					//   USAGE_PAGE (Button) --20
	0x19, 0x01,						//   USAGE_MINIMUM (Button 1)
	0x29, 0x0d,						//   USAGE_MAXIMUM (Button 13)
	0x81, 0x02,						//   INPUT (Data,Var,Abs)
	0x95, 0x03,						//     REPORT_COUNT (3)
	0x81, 0x01,						//Input something or other --30
	0x05, 0x01,					// USAGE_PAGE (Generic Desktop)
	0x25, 0x07,						//   LOGICAL_MAXIMUM (7)
	0x46, 0x3b, 0x01,				//physical maximum 315
	0x75, 0x04,						//Report size 4    --39
	0x95, 0x01,						//Report count 1
	0x65, 0x14,					//unit: English angular position
	0x09, 0x39,						//usage: hat switch
	0x81, 0x42,					// Input something or other
	0x65, 0x00,						//Unit:none    --49
	0x95, 0x01,						//report count 1
	0x81, 0x01,						//Input something or other
	0x26, 0xff, 0x00,				// LOGICAL_MAXIMUM (255)
	0x46, 0xff, 0x00,			// PHYSICAL_MAXIMUM (255)  --59
	0x09, 0x30,
	0x09,			//--62
	0x31, 0x09,
	0x32, 0x09,
	0x35, 0x75,
	0x08, 0x95,
	0x04, 0x81,   //--72
	0x02, 0x06,
	0x00, 0xff,
	0x09, 0x20,
	0x09, 0x21,
	0x09, 0x22,   //82
	0x09, 0x23,
	0x09, 0x24,
	0x09, 0x25,
	0x09, 0x26,   //90
	0x09, 0x27,
	0x09, 0x28,
	0x09, 0x29,
	0x09, 0x2a,
	0x09, 0x2b,  //100
	0x95, 0x0c,
	0x81, 0x02,
	0x0a, 0x21,
	0x26, 0x95,
	0x08, 0xb1,
	0x02, 0xc0   //--112 == 0x70
};

//checked with VSHG
const unsigned char PS3USB_String0[] = {
	4,	// bLength
	STRING,	// bDescriptorType
	0x09,	// wLANGID[0] (low byte)
	0x04	// wLANGID[0] (high byte)
};

//from VSHG: "SEGA"
const unsigned char PS3USB_String1[] = {
	10, STRING, 'S', 0, 'E', 0, 'G', 0, 'A', 0
};

//From VSHG
const unsigned char PS3USB_String2[] = {
	48, STRING, 'V', 0, 'I', 0, 'R', 0, 'T', 0, 'U', 0,	//12
	'A', 0, ' ', 0, 'S', 0, 'T', 0, 'I', 0, 'C', 0,		//24
	'K', 0, ' ', 0, 'H', 0, 'i', 0, 'g', 0, 'h', 0,     //36
	' ', 0, 'G', 0, 'r', 0, 'a', 0, 'd', 0, 'e', 0		//48
};

const unsigned char PCUSB_String0[] = {
	4,		// bLength
	STRING,	// bDescriptorType
	0x09,	// wLANGID[0] (low byte)
	0x04	// wLANGID[0] (high byte)
};

const unsigned char PCUSB_String1[] = {
	24,													// bLength  --1
	STRING,												// bDescriptorType --2
	'N', 0x00,'i', 0x00,'l', 0x00,						//--8
	'u', 0x00,'j', 0x00,'e', 0x00, ' ', 0x00,			//-16
	'2', 0x00,'0', 0x00,'0', 0x00, '7', 0x00			//-24
};

const unsigned char PCUSB_String2[] = {
	36,	// bLength
	STRING,	// bDescriptorType
	'D', 0x00,'o', 0x00,'p', 0x00,  // -8
	'p', 0x00,'e', 0x00,'l', 0x00,'g', 0x00,	//-16
	'a', 0x00,'n', 0x00,'g', 0x00,'e', 0x00,'r', 0x00,	//-26
	' ', 0x00, 'v', 0x00, MAJOR_REVISION, 0x00, '.', 0x00,
    MINOR_REVISION, 0x00,  //-36
};

void PS3USB_InitUSB(void)
{
	debug_print("Enters PS3USB InitUSB\r\n");
	UIE = 0x00;					// mask all USB interrupts
	UIR = 0x00;					// clear all USB interrupt flags
	UCFG = 0x14;				// we need full speed
	UCON = 0x08;				// enable the USB module and its
                                // supporting circuitry
	USB_curr_config = 0x00;
	USB_idle_rate = 0x00;
	USB_USWSTAT = 0x00;			// default to powered state
	USB_device_status = 0x01;
	USB_protocol = 0x01;		// default to report protocol initially
	USB_dev_req = NO_REQUEST;	// No device requests in process
	while (UCONbits.SE0);		// wait for the first SE0 to end
}

void PS3USB_ServiceUSB(void)
{
	BUFDESC_t *buf_desc_ptr;

	if (UIRbits.UERRIF) {
		//debug_print("ERR1\r\n");
		//If there's been a USB error of some sort...
		//Clear out all of the error flags
		UEIR = 0x00;
	} else if (UIRbits.SOFIF) {
		//if there's a start of frame error clear it
		UIRbits.SOFIF = 0;
	} else if (UIRbits.IDLEIF) {
		//debug_print("idle detected\r\n");
		//if there's an idle detect (3ms of silence)
		//clear the flag and suspend the USB module
		UIRbits.IDLEIF = 0;
		UCONbits.SUSPND = 1;
	} else if (UIRbits.ACTVIF) {
		//debug_print("activity detected\r\n");
		//activity on the D+/D- lines
		UIRbits.ACTVIF = 0;     //clear the flag
		UCONbits.SUSPND = 0;    //and wakeup from suspend
		/* what if not suspended?
		 * does the ACTIVF not get set for good communication? or
		 * is the Interrupt enable for the ACTVIF flag only enabled on suspend?
		 * Because the IDLE doesn't look like it sets it. */
	} else if (UIRbits.STALLIF) {
		//debug_print("stall handshake rcved\r\n");
		// If a STALL handshake received..
		// Clear it
		UIRbits.STALLIF = 0;
	} else if (UIRbits.URSTIF) {
		//debug_print("we need to reset\r\n");
		/* If we're told to reset */
		/* drop everything. Set address to 0x00, flush every buffer enable the
		 * EP0 control buffer as normal. */
		USB_curr_config = 0x00;
		UIRbits.TRNIF = 0; // clear TRNIF four times to clear out the USTAT FIFO
		UIRbits.TRNIF = 0;
		UIRbits.TRNIF = 0;
		UIRbits.TRNIF = 0;
		UEP0 = 0x00; // clear all EP control registers to disable all endpoints
		UEP1 = 0x00;
		UEP2 = 0x00;
		UEP3 = 0x00;
		UEP4 = 0x00;
		UEP5 = 0x00;
		UEP6 = 0x00;
		UEP7 = 0x00;
		UEP8 = 0x00;
		UEP9 = 0x00;
		UEP10 = 0x00;
		UEP11 = 0x00;
		UEP12 = 0x00;
		UEP13 = 0x00;
		UEP14 = 0x00;
		UEP15 = 0x00;
		BD0O.bytecount = PS3USB_EP0_PACKET_SIZE;
		BD0O.address = &PS3USB_EP0_OUT_buffer; // EP0 OUT gets a buffer
		BD0O.status = 0x88;					   // set UOWN bit (USB can write)
		BD0I.address = &PS3USB_EP0_IN_buffer;  // EP0 IN gets a buffer
		BD0I.status = 0x08;					   // clear UOWN bit (MCU can write)
		UADDR = 0x00;						   // set USB Address to 0
		UIR = 0x00;							// clear all the USB interrupt flags
		UEP0 = ENDPT_CONTROL;		// EP0 is a control pipe and requires an ACK
		UEIE = 0xFF;						   // enable all error interrupts
		USB_USWSTAT = DEFAULT_STATE;
		USB_device_status = 0x01;	// self powered, remote wakeup disabled
	} else if (UIRbits.TRNIF) {
		//debug_print("transaction complete\r\n");
		//transcation complete flag. Some transaction has completed properly
		//need to check the stats to see what happened.

		buf_desc_ptr = (BUFDESC_t *)((unsigned char *)(&BD0O)+(USTAT&0x7C));
		/* mask out bits 0, 1, and 7 of USTAT for offset into the buffer
		 * descriptor table the masked buffer descriptor should tell us which
		 * endpoint something happened in. Copy the contents of the buffer
		 * descriptor to the global variable so we can examine it. */
		USB_buffer_desc.status = buf_desc_ptr->status;
		USB_buffer_desc.bytecount = buf_desc_ptr->bytecount;
		USB_buffer_desc.address = buf_desc_ptr->address;
		USB_USTAT = USTAT;		// save the USB status register
		UIRbits.TRNIF = 0;		// clear TRNIF interrupt flag
		USB_error_flags = 0x00;	// clear USB error flags

		/* Now that we have the details from the buffer descriptor, figure out
		 * what to do with it.*/
		switch (USB_buffer_desc.status&0x3C) {
			/* Extract PID bits to see what kind of token it is and then
			 * process it. */
			case TOKEN_SETUP:
				PS3USB_ProcessSetupToken();
				break;
			case TOKEN_IN:
				PS3USB_ProcessInToken();
				break;
			case TOKEN_OUT:
				PS3USB_ProcessOutToken();
				break;
			case TOKEN_ACK:
				break;
			default:
				//debug_print("unknown TOKEN\r\n");
				break;
		}
		if (USB_error_flags&0x01) {
			//debug_print("ERR4\r\n");
			/* if there was a Request Error get ready to receive the next Setup
			 * token and issue a protocol stall on EP0 */
			BD0O.bytecount = PS3USB_EP0_PACKET_SIZE;
			BD0I.status = 0x84;
			BD0O.status = 0x84;
		}
	}
}

void PS3USB_ProcessSetupToken(void)
{
	unsigned char n;
	/* Okay, if we're here, we successfully finished some chunk of a transfer,
	 * and the PID from the transfer told us it was a setup token.
	 *
	 * I dont like the hard coded 8 bytes here, but Setup packets are all
	 * 8 bytes according to spec. */

	//debug_print("Setup Token\r\n");

	for (n = 0; n<8; n++)
		USB_buffer_data[n] = USB_buffer_desc.address[n];

	/* Reset the EP0 OUT byte count */
	BD0O.bytecount = PS3USB_EP0_PACKET_SIZE;
	/* So the bytecount for a buffer descriptor counts DOWN? not to sure I like
	 * this. Need to read up on BD's in the datasheet. */

	/* Return the EP0 IN buffer to us (dequeue any pending requests) */
	BD0I.status = 0x08;
	BD0O.status = (!(USB_buffer_data[bmRequestType]&0x80) &&
					(USB_buffer_data[wLength] || USB_buffer_data[wLengthHigh]))
					? 0xC8 : 0x88;
		/* Set EP0 OUT UOWN back to USB and DATA0/DATA1 packet according to the
		 * request type.
		 * Translation: if the length is non-zero and the data was from the host
		 * to the device then set status to 0xC8; otherwise 0x88.
		 * Need more information on the details of the status byte.
		 * The Summary on datasheet page 178 doesn't help too much.
		 * The first bit, set on both to 1, is the ownership of the buffer.
		 * bits 6 and 3 I dont get yet.	*/
	/* Assuming there is nothing to dequeue, clear the packet disable bit */
	UCONbits.PKTDIS = 0;
	/* PKTDIS is supposedly automagically set when a setup token is received,
	 * this clears it so work may continue */
	/* clear the device request in process */
	USB_dev_req = NO_REQUEST;
	/* USB_dev_req seems to be a global flag, almost a state machine indicator.
	 * Holds the current type of request received.*/
	switch (USB_buffer_data[bmRequestType]&0x60) {
		/* Extract request type bits get bits 6 and 5 from the first byte.
		 * These bits hold the Type of request.
		 * Standard=00, class=01, vendor=10, reserved=11
		 * (defines are the number >>5)
		 * figure which it is, and process with that function. */

		/* Damn, so the setup packet has multiple sub packet possibilities.
		 * lots to keep track of. :( */
		case STANDARD:
			PS3USB_StandardRequests();
			break;
		case CLASS:
			PS3USB_ClassRequests();
			break;
		case VENDOR:
			PS3USB_VendorRequests();
			break;
		default:
			/* Set Request Error Flag */
			USB_error_flags |= 0x01;
	}
}

void PS3USB_StandardRequests(void)
{
	/* We've successfully got a token in, identified it as a Setup token,
	 * flushed and took over the EP0 buffer, and identified it as a Standard
	 * request. */
	unsigned char *UEP;
	unsigned char n;
	BUFDESC_t *buf_desc_ptr;

	//debug_print("StdRequests\r\n");

	switch (USB_buffer_data[bRequest]) {
		/* Finally checking the second bit, data[1] */
		case GET_STATUS:
			//debug_print("GET_STATUS\n\r");
			/* 0x00. Expects a two byte status result; only a couple of bits
			 * are actually defined. Different results based on if its to the
			 * device, interface or endpoint. */
			switch (USB_buffer_data[bmRequestType]&0x1F) {
				/* Extract request recipient bits pulls out the lowest 5 bits
				 * from the first byte to see if its for device, intf,
				 * or endpoint */
				case RECIPIENT_DEVICE:
					BD0I.address[0] = USB_device_status;
					/* bit 0 : true if device is self powered. bit 1: true if
					 * device can wake up host rest of bits always 0. Put the
					 * data in the buffer and send it off. */
					BD0I.address[1] = 0x00;
					BD0I.bytecount = 0x02;
					/* Send packet as DATA1, set UOWN bit */
					BD0I.status = 0xC8;
					break;
				case RECIPIENT_INTERFACE:
					/* 0x01. All 16 bits must be 0. */
					switch (USB_USWSTAT) {
						/* Weird. checking the current state of the device,
						 * I guess the check isn't done for the first device
						 * because its needed to be done during configuration */
						case ADDRESS_STATE:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
							break;
						case CONFIG_STATE:
							/* We're up and running fine. send out the message,
							 * as long as it is to one of the actual interfaces.
							 * throw an error if it is checking and interface
							 * beyond the existing one(s) */
							if (USB_buffer_data[wIndex]<PS3USB_NUM_INTERFACES) {
								BD0I.address[0] = 0x00;
								BD0I.address[1] = 0x00;
								BD0I.bytecount = 0x02;
								/* send packet as DATA1, set UOWN bit */
								BD0I.status = 0xC8;
							} else {
								/* set Request Error Flag */
								USB_error_flags |= 0x01;
							}
					}
					break;
				case RECIPIENT_ENDPOINT:
					/* Once again, responses based on current
					 * configured state.*/
					switch (USB_USWSTAT) {
						case ADDRESS_STATE:
							/* device has a configured address. */
							if ((USB_buffer_data[wIndex]&0x0F)==0x00) {
								/* get EP, strip off direction bit and see
								 * if it is EP0*/

								/* return the BSTALL bit of EP0 IN or OUT,
								 * whichever was requested */
								BD0I.address[0] =
									(((USB_buffer_data[wIndex]&0x80) ?
									  BD0I.status:BD0O.status)&0x04)>>2;
								BD0I.address[1] = 0x00;
								BD0I.bytecount = 0x02;
								/* send packet as DATA1, set UOWN bit */
								BD0I.status = 0xC8;
							} else {
							/* whoa whoa whoa. Was worried at first about no
							 * reponses other endpoints other than EP0, but in
							 * ADDRESS_STATE, EP1 hasnt been setup yet. So this
							 * part is fine. USB_error_flags |= 0x01;
							 * Set Request Error Flag
							 * TODO: Find out where the hell USB_error_flags
							 * actually causes something to happen. */
							}
							break;
						case CONFIG_STATE:
							/* Ah, system is fully up and operational. */
							UEP = (unsigned char *)&UEP0;
							/* set UEP to point at UEP0 register kinda neat how
							 * they're changing the pointer to point at the
							 * descriptor for that endpoint buffer. Covers all
							 * possible endpoints.  good stuff. */

							/* Send packet as DATA1, set UOWN bit */
							n = USB_buffer_data[wIndex]&0x0F;
							/* compute pointer to the buffer descriptor for the
							 *  specified EP */
							buf_desc_ptr = &BD0O+((n<<1)|((USB_buffer_data[wIndex]&0x80) ? 0x01:0x00));
							/* seems to expect the buffer descriptors to be in a
							 * versy specific order, apparently without pingpong
							 * buffers */

							/*checking the declarations in usb_defs.h confirms
							 * this. NOTE: Buffer descriptors MUST be delared
							 * together sequentially! otherwise this will break.*/

							/* If the specified EP is enabled for transfers in
							 * the specified direction... */
							if (UEP[n]&((USB_buffer_data[wIndex]&0x80) ? 0x02:0x04)) {
								/* ...return the BSTALL bit of the specified EP */
								BD0I.address[0] = ((buf_desc_ptr->status)&0x04)>>2;
								BD0I.address[1] = 0x00;
								BD0I.bytecount = 0x02;
								/* send packet as DATA1, set UOWN bit */
								BD0I.status = 0xC8;
							} else {
								/* set Request Error Flag */
								USB_error_flags |= 0x01;
							}
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					break;
				default:
					/* set Request Error Flag */
					USB_error_flags |= 0x01;
			}
			break;
		case CLEAR_FEATURE: //BOOKMARK
			//debug_print("CLEAR_STATUS\n\r");
		case SET_FEATURE:
			//debug_print("SET_STATUS\n\r");
			/* notice the lack of Break; this chunk handles both the CLEAR and
			 * SET versions of the request. */
			switch (USB_buffer_data[bmRequestType]&0x1F) {
				/* set Request Error Flag */
				case RECIPIENT_DEVICE:
					/*only request for devices should be remote wakeup. */
					switch (USB_buffer_data[wValue]) {
						case DEVICE_REMOTE_WAKEUP:
							if (USB_buffer_data[bRequest]==CLEAR_FEATURE)
								USB_device_status &= 0xFE;
							else
								USB_device_status |= 0x01;
							/* set Request Error Flag */
							BD0I.bytecount = 0x00;
							/* set Request Error Flag */
							BD0I.status = 0xC8;
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					break;
				case RECIPIENT_ENDPOINT:
					/* only endpoint feature is endpoint halt. */
					switch (USB_USWSTAT) {
						/* if we're in the address state, there is only EP0 */
						case ADDRESS_STATE:
							if (!(USB_buffer_data[wIndex]&0x0F)) {
								/* get EP, strip off direction bit, and see if
								 * its EP0 */
								/* set Request Error Flag */
								BD0I.bytecount = 0x00;
								/* set Request Error Flag */
								BD0I.status = 0xC8;
							} else {
								/* set Request Error Flag */
								USB_error_flags |= 0x01;
							}
							break;
						case CONFIG_STATE:
							UEP = (unsigned char *)&UEP0;
							if (n = USB_buffer_data[wIndex]&0x0F) {
								/* get EP and strip off direction bit for offset
								 *  from UEP0, if not EP0... */
								/* compute pointer to the buffer descriptor for
								 * the specified EP */
								buf_desc_ptr = &BD0O+((n<<1)|((USB_buffer_data[wIndex]&0x80) ? 0x01:0x00));
								if (USB_buffer_data[wIndex]&0x80) {
									/* if the specified EP direction is IN... */
									if (UEP[n]&0x02) {
										/* if EPn is enabled for IN transfers... */
										buf_desc_ptr->status = (USB_buffer_data[bRequest]==CLEAR_FEATURE) ? 0x00:0x84;
										/* shutdown the EP or enable the EP. */
									} else {
										/* set Request Error Flag */
										USB_error_flags |= 0x01;
									}
								} else {
									/* ...otherwise the specified EP direction
									 * is OUT, so... */
									if (UEP[n]&0x04) {
										/* if EPn is enabled for OUT transfers... */
										buf_desc_ptr->status = (USB_buffer_data[bRequest]==CLEAR_FEATURE) ? 0x88:0x84;
									} else {
										/* set Request Error Flag */
										USB_error_flags |= 0x01;
									}
								}
							}
							if (!(USB_error_flags&0x01)) {
								/* if there was no Request Error...
								 * NOTE: Is this how one sends an ACK? */
								BD0I.bytecount = 0x00;
								/* ...send packet as DATA1, set UOWN bit */
								BD0I.status = 0xC8;
							}
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					break;
				default:
					/* set Request Error Flag */
					USB_error_flags |= 0x01;
			}
			break;
		case SET_ADDRESS:
			//debug_print("SET_ADDRESS : ");
			debug_print_val(USB_buffer_data[wValue]);
			if (USB_buffer_data[wValue]>0x7F) {
				/* if new device address is illegal, send Request Error */
				/* set Request Error Flag */
				USB_error_flags |= 0x01;
				//debug_print("Invalid address\r\n");
			} else {
				/* processing a SET_ADDRESS request */
				USB_dev_req = SET_ADDRESS;
				/* save new address */
				USB_address_pending = USB_buffer_data[wValue];
				/* NOTE: When done the actuall address get changed?
				 * USB_address_pending is a global, so hunt that down.
				 * prolly checks USB_dev_req */

				/* set EP0 IN byte count to 0 */
				BD0I.bytecount = 0x00;
				/* send packet as DATA1, set UOWN bit */
				BD0I.status = 0xC8;
				if (!GETDESCR_BEFORE_SETADDR) {
					/* setting the address before querying device descriptor
					* which means it is likely the PS3 BC mode */
					//debug_print("PS3 detected\r\n");
					SETADDR_BEFORE_GETDESCR=1;
					PS3_DETECTED=1;
				}
			}
			break;
		case GET_DESCRIPTOR:
			//debug_print("GET_DESCRIPTOR\n\r");
			/* processing a GET_DESCRIPTOR request */
			USB_dev_req = GET_DESCRIPTOR;
			if (!SETADDR_BEFORE_GETDESCR) {
				//debug_print("PC detected\r\n");
				/* querying the descriptors before setting the address */
				GETDESCR_BEFORE_SETADDR=1;
				//which means it is likely a proper PC connection
				PS3_DETECTED=0;
			}
			switch (USB_buffer_data[wValueHigh]) {
				case DEVICE:
					//debug_print("DESCR DEVICE\n\r");
					if(PS3_DETECTED) {
						USB_desc_ptr = PS3USB_Device_Descriptor;
					} else {
						USB_desc_ptr = PCUSB_Device_Descriptor;
					}
					/* aka the size of that descriptor */
					USB_bytes_left = USB_desc_ptr[0];
					if ((USB_buffer_data[wLengthHigh]==0x00) && (USB_buffer_data[wLength]<USB_bytes_left)) {
						/* NOTE: kinda worried about this check. Can a device
						 * descriptor be >0xFF in size? */
						USB_bytes_left = USB_buffer_data[wLength];
					}
					/* Send the next packet width check of the descriptor */
					PS3USB_SendDescriptorPacket();
					break;
				case CONFIGURATION:
					//debug_print("DESCR CONFIGURATION\n\r");
					switch (USB_buffer_data[wValue]) {
						case 0:
							/* I guess this means there is only support for one
							 * configuration */
							USB_desc_ptr = PS3USB_Configuration_Descriptor;
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					if (!(USB_error_flags&0x01)) {
						/* wTotalLength at an offset of 2 */
						USB_bytes_left = USB_desc_ptr[2];
						if ((USB_buffer_data[wLengthHigh]==0x00) &&
							(USB_buffer_data[wLength]<USB_bytes_left)) {
							USB_bytes_left = USB_buffer_data[wLength];
						}
						PS3USB_SendDescriptorPacket();
					}
					break;
				case STRING:
					//debug_print("DESCR STRING\n\r");
					switch (USB_buffer_data[wValue]) {
						case 0:
							if(PS3_DETECTED)
								USB_desc_ptr = PS3USB_String0;
							else
								USB_desc_ptr = PCUSB_String0;
							break;
						case 1:
							if(PS3_DETECTED)
								USB_desc_ptr = PS3USB_String1;
							else
								USB_desc_ptr = PCUSB_String1;
							break;
						case 2:
							if(PS3_DETECTED)
								USB_desc_ptr = PS3USB_String2;
							else
								USB_desc_ptr = PCUSB_String2;;
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					if (!(USB_error_flags&0x01)) {
						USB_bytes_left = USB_desc_ptr[0];
						if ((USB_buffer_data[wLengthHigh]==0x00) &&
							(USB_buffer_data[wLength]<USB_bytes_left)) {
							USB_bytes_left = USB_buffer_data[wLength];
						}
						PS3USB_SendDescriptorPacket();
					}
					break;
				case HID:
					//debug_print("DECR HID\r\n");
					switch (USB_buffer_data[wValue]) {
						case 0:
							USB_desc_ptr = PS3USB_Configuration_Descriptor+0x12;
							/* add offset of HID1 descriptor from the beginning
							 * of PS3USB_Configuration_Descriptor descriptor */
							/* whoa. this is crap. there's gotta be a better way
							 *  to do that.  NOTE: Make this better */
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					if (!(USB_error_flags&0x01)) {
						/* wTotalLength at an offset of 2 */
						USB_bytes_left = USB_desc_ptr[0];
						if ((USB_buffer_data[wLengthHigh]==0x00) &&
							(USB_buffer_data[wLength]<USB_bytes_left)) {
							USB_bytes_left = USB_buffer_data[wLength];
						}
						PS3USB_SendDescriptorPacket();
					}
					break;
				case REPORT:
					//debug_print("DECR REPORT\r\n");
					switch (USB_buffer_data[wValue]) {
						case 0:
						/* FLAG! The report descriptor here should be automagic
						 * from the declaration */

						/* total length of PS3USB_Report_Descriptor descirptor
						 * in HID1 descriptor, rather
						 * than PS3USB_Report_Descriptor */
							USB_bytes_left = 0x94;
							/* #error size of sizeof(PS3USB_Report_Descriptor) */
							USB_desc_ptr = PS3USB_Report_Descriptor;
							break;
						default:
							/* set Request Error Flag */
							USB_error_flags |= 0x01;
					}
					if (!(USB_error_flags&0x01)) {
						if ((USB_buffer_data[wLengthHigh]==0x00) &&
							(USB_buffer_data[wLength]<USB_bytes_left)) {
							USB_bytes_left = USB_buffer_data[wLength];
						}
						PS3USB_SendDescriptorPacket();
					}
					break;
				default:
					/* set Request Error Flag */
					USB_error_flags |= 0x01;
			}
			break;

		case GET_CONFIGURATION:
			//debug_print("GET_CONFIGURATION\n\r");
			/* interesting. the configuration is only set in
			 * the SET_CONFIGURATION call below, and frankly doesn't do anything
			 * but sit there in a global var. Grab the config number the host
			 * set, return it in a 1 byte pack */

			/* copy current device configuration to EP0 IN buffer */
			BD0I.address[0] = USB_curr_config;
			BD0I.bytecount = 0x01;
			/* send packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			break;
		case SET_CONFIGURATION:
			//debug_print("SET_CONFIGURATION\n\r");
			if (USB_buffer_data[wValue]<=PS3USB_NUM_CONFIGURATIONS) {
				/* host gave us the ok to setup shop.
				 * clear all EP control registers except for EP0 to disable
				 * EP1-EP15 prior to setting configuration */
				UEP1 = 0x00;
				UEP2 = 0x00;
				UEP3 = 0x00;
				UEP4 = 0x00;
				UEP5 = 0x00;
				UEP6 = 0x00;
				UEP7 = 0x00;
				UEP8 = 0x00;
				UEP9 = 0x00;
				UEP10 = 0x00;
				UEP11 = 0x00;
				UEP12 = 0x00;
				UEP13 = 0x00;
				UEP14 = 0x00;
				UEP15 = 0x00;
				switch (USB_curr_config = USB_buffer_data[wValue]) {
					case 0:
						USB_USWSTAT = ADDRESS_STATE;
						/* config 0 unsets everything, like a new plugin I
						 * guess. Set state machine to know we need to get
						 * an address. */
						break;
					default:
						/* if it aint 0, we dont really care what it is.
						 * Just enable the other EP's */

						/* tell state machine we configured. */
						USB_USWSTAT = CONFIG_STATE;
						/* set EP1 IN byte count */
						BD1I.bytecount = PS3USB_EP1_PACKET_SIZE;
						/* set EP1 IN buffer address */
						BD1I.address = &PS3USB_EP1_IN_buffer;
						/* clear UOWN bit (PIC can write EP1 IN buffer) */
						BD1I.status = 0x48;
						/* enable EP1 for interrupt in transfers */
						UEP1 = ENDPT_IN_ONLY;
						/* NOTE: This seems to be where we can specify between
						 * the four transfer types. */
						/* ENDPOINT SETUPS GO HERE! */
				}
				/* set EP0 IN byte count to 0 */
				BD0I.bytecount = 0x00;
				/* send packet as DATA1, set UOWN bit */
				BD0I.status = 0xC8;
			} else {
				/* set Request Error Flag */
				USB_error_flags |= 0x01;
			}
			break;
		case GET_INTERFACE:
			//debug_print("GET_INTERFACE\n\r");
			switch (USB_USWSTAT) {
				case CONFIG_STATE:
					if (USB_buffer_data[wIndex]<PS3USB_NUM_INTERFACES) {
						/* still quite a bit fuzzy on what this should
						 * do if anything. */

						/* always send back 0 for bAlternateSetting */
						BD0I.address[0] = 0x00;
						BD0I.bytecount = 0x01;
						/* send packet as DATA1, set UOWN bit */
						BD0I.status = 0xC8;
					} else {
						/* set Request Error Flag */
						USB_error_flags |= 0x01;
					}
					break;
				default:
					/* set Request Error Flag */
					USB_error_flags |= 0x01;
			}
			break;
		case SET_INTERFACE:
			//debug_print("SET_INTERFACE\n\r");
			switch (USB_USWSTAT) {
				case CONFIG_STATE:
					if (USB_buffer_data[wIndex]<PS3USB_NUM_INTERFACES) {
						switch (USB_buffer_data[wValue]) {
							case 0:
								/* currently support only bAlternateSetting of 0 */
								/* set EP0 IN byte count to 0 */
								BD0I.bytecount = 0x00;
								/* send packet as DATA1, set UOWN bit */
								BD0I.status = 0xC8;
								break;
							default:
								/* set Request Error Flag */
								USB_error_flags |= 0x01;
						}
					} else {
						/* set Request Error Flag */
						USB_error_flags |= 0x01;
					}
					break;
				default:
					/* set Request Error Flag */
					USB_error_flags |= 0x01;
			}
			break;
		case SET_DESCRIPTOR:
			//debug_print("SET_DESCRIPTOR\n\r");
		case SYNCH_FRAME:
			//debug_print("SYNCH_FRAME\n\r");
		default:
			/* set Request Error Flag */
			USB_error_flags |= 0x01;
	}
}

void PS3USB_ClassRequests(void)
{
	unsigned char n;

	//debug_print("PS3USB ClassRequests\r\n");
	switch (USB_buffer_data[bRequest]) {
		case GET_REPORT:
			/* FLAG! set to report size */
			for (n = 0; n<PS3USB_REPORT_SIZE; n++) {
				/* Copy Key_buffer to EP0 IN buffer
				 * BD0I.address[n] = PS3USB_USBbuffer[n]; */
				BD0I.address[n] = 0;
			}
			BD0I.address[0] = 33;
			BD0I.address[1] = 38;
			/* set byte count to 8
			 * BD0I.bytecount = PS3USB_REPORT_SIZE; */
			BD0I.bytecount = 8;
			/* send packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			/* Whoa whoa whoa. So the report can be sent out via EP0 here, or
			 * via EP1 in the PS3USB_SendReport function?
			 * what if the report is >8 bytes? Are there multiple requests? */
			break;
		case SET_REPORT:
			//debug_print("SET_REPORT\r\n");
			USB_dev_req = SET_REPORT;
			/* processing a SET_REPORT request. I guess there's not much to do
			 * with the SET_REPORT; setting the dev_req just tells to clear out
			 * the dev_req on the next outtoken. Doesn't DO anything. Not even
			 * an acknowledge or */

			/* zero byte answer. */
			break;
		case GET_PROTOCOL:
			//debug_print("GET_PROTOCOL\r\n");
			/* copy current protocol into EP0 IN buffer */
			BD0I.address[0] = USB_protocol;
			/* set byte count to 1 */
			BD0I.bytecount = 0x01;
			/* send packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			/* similar to the GET_CONFIGURATION from Standard requests; SET
			 * stashes the protocol in a global, and spits it back. if we get
			 * a GET */
			break;
		case SET_PROTOCOL:
			//debug_print("SET_PROTOCOL\r\n");
			/* update the new protocol value */
			USB_protocol = USB_buffer_data[wValue];
			/* set byte count to 0 */
			BD0I.bytecount = 0x00;
			/* sent packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			/* stash in global and done. */
			break;
		case GET_IDLE:
			//debug_print("GET_IDLE\r\n");
			/* copy current idle rate into EP0 IN buffer */
			BD0I.address[0] = USB_idle_rate;
			/* set byte count to 1 */
			BD0I.bytecount = 0x01;
			/* send packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			/* Same; SET stashes it in a global, and GET spits it back. */
			break;
		case SET_IDLE:
			//debug_print("SET_IDLE\r\n");
			/* update the new idle rate */
			USB_idle_rate = USB_buffer_data[wValueHigh];
			/* set byte count to 0 */
			BD0I.bytecount = 0x00;
			/* sent packet as DATA1, set UOWN bit */
			BD0I.status = 0xC8;
			break;
		default:
			/* set Request Error Flag */
			USB_error_flags |= 0x01;
	}
}

void PS3USB_VendorRequests(void)
{
	//debug_print("PS3USB VendorRequests\r\n");
	switch (USB_buffer_data[bRequest]) {
		default:
			/* set Request Error Flag */
			USB_error_flags |= 0x01;
	}
}

void PS3USB_ProcessInToken(void)
{
	//debug_print("PS3USB ProcessInTocken\r\n");
	/* these In tokens seem to be continuations, the host saying 'Im expecting
	 * more data: send it'. TODO: Need to verify if the larger HID reports will
	 * require multiple InTokens. */
	switch (USB_USTAT&0x18) {
		/* extract the EP bits */
		case EP0:
			//debug_print("--> EP0 (");
			debug_print_val(USB_dev_req);
			//debug_print(")");
			switch (USB_dev_req) {
				case SET_ADDRESS:
					//debug_print("--> set address\r\n");
					switch (UADDR = USB_address_pending) {
						case 0:
							USB_USWSTAT = DEFAULT_STATE;
							break;
						default:
							USB_USWSTAT = ADDRESS_STATE;
					}
					break;
				case GET_DESCRIPTOR:
					//debug_print("--> get descriptor\r\n");
					PS3USB_SendDescriptorPacket();
					break;
			}
			break;
		case EP1:
			//debug_print("--> EP1\r\n");
			break;
		case EP2:
			//debug_print("--> EP2\r\n");
			break;
	}
}

void PS3USB_ProcessOutToken(void)
{
	//debug_print("PS3USB Process Out Token\r\n");
	switch (USB_USTAT&0x18) {
		/* extract the EP bits */
		case EP0:
			//debug_print("PS3USB Process Out Token\r\n");
			switch (USB_dev_req) {
				case SET_REPORT:
					//debug_print("SET_REPORT case. About to clear the device request\r\n");
					/* clear device request */
					USB_dev_req = NO_REQUEST;
					break;
				default:
					//debug_print("Default case for \r\n");
					debug_print_val(USB_dev_req);
					break;
			}
			BD0O.bytecount = PS3USB_EP0_PACKET_SIZE;
			BD0O.status = 0x88;
			BD0I.bytecount = 0x00;	/* set EP0 IN byte count to 0 */
			BD0I.status = 0xC8;		/* send packet as DATA1, set UOWN bit */
			/* TODO: I really need to get down the buffer descriptor
			 * status bytes. */
			break;
		case EP1:
			break;
		case EP2:
			break;
	}
}

void PS3USB_SendDescriptorPacket(void)
{
	unsigned char n;

	if (USB_bytes_left<PS3USB_EP0_PACKET_SIZE) {
		/* sending a short packet, so clear device request */
		USB_dev_req = NO_REQUEST;
		USB_packet_length = USB_bytes_left;
		USB_bytes_left = 0x00;
	} else {
		USB_packet_length = PS3USB_EP0_PACKET_SIZE;
		USB_bytes_left -= PS3USB_EP0_PACKET_SIZE;
	}
	for (n = 0; n<USB_packet_length; n++) {
		BD0I.address[n] = *USB_desc_ptr++;
	}
	BD0I.bytecount = USB_packet_length;
	BD0I.status = ((BD0I.status^0x40)&0x40)|0x88;
	/* toggle the DATA01 bit, clear the PIDs bits, and set the UOWN and DTS bits
	 * so wait a tic. I know that EP0 buffer is prolly 8 in everything here;
	 * thats cool, well kinda. But if the descriptor is > UBS_packet_length,
	 * then the host will have to request the rest of the descriptor, right?
	 * maybe it does already checking the Made Simple site, it does appear from
	 * the mouse description on page 5 to be the case. THe host repeats the
	 * request, and the device ships another chunk out. looks like USB_desc_ptr
	 * is the pointer to where the descriptor is at in the process of being
	 * sent. Part I dont understand is that the USB_desc_ptr is overwritten to
	 * the beginning of the descriptor will each call. Hmm. I'm sure it works,
	 * just am missing something. it looks like the rest of the data is being
	 * sent when this function is being called by PS3USB_ProcessInToken. */
}

void PS3USB_SendReport(void)
{
	unsigned char n;

	for (n = 0; n<PS3USB_REPORT_SIZE; n++) {
		/* copy Key_buffer to EP1 IN buffer */
		BD1I.address[n] = PS3USB_USBbuffer[n];
	}
	/* set the EP1 IN byte count to 8 */
	BD1I.bytecount = PS3USB_REPORT_SIZE;
	/* toggle the DATA01 bit of the EP1 IN status register and set the UOWN
	 * and DTS bits */
	BD1I.status = ((BD1I.status&0x40)^0x40)|0x88;
}

void ps3usbApp(void)
{
	unsigned char USB_STICK_MODE;

	debug_print("Entering USBPS3 module\r\n");
	USB_STICK_MODE=USM_BOTH;
	if(!PCB_START)
		USB_STICK_MODE=USM_DIGITAL_ONLY;
	if(!PCB_SELECT)
		USB_STICK_MODE=USM_ANALOG_ONLY;
	debug_print("USB_STICK_MODE = ");
	debug_print_val(USB_STICK_MODE);
	debug_print("\r\n");
	SETADDR_BEFORE_GETDESCR=0;
	GETDESCR_BEFORE_SETADDR=0;
	PS3_DETECTED=0;

	/* Detect the PS3 by pressing Square and Cross buttons at startup */
	if(!PCB_SQUARE && !PCB_CROSS) {
		debug_print("PS3 detected\r\n");
		PS3_DETECTED=1;
	}

	PS3USB_InitUSB();

	/* while the peripheral is not configured... */
	debug_print("Wait before configured\r\n");
	while (USB_USWSTAT!=CONFIG_STATE) {
		/* ...service USB requests */
		PS3USB_ServiceUSB();
	}

	debug_print("USB configured\r\n");
	while (1) {
		if (!(BD1I.status&0x80)) {
			/* see if UOWN bit of EP1 IN status register is clear
			 * (i.e., PIC owns EP1 IN buffer) */
			PS3USB_USBbuffer[0]=PS3USB_USBbuffer[1]=0;
			PS3USB_USBbuffer[2]=0x08;
			PS3USB_USBbuffer[3]=PS3USB_USBbuffer[4]=PS3USB_USBbuffer[5]=PS3USB_USBbuffer[6]=0x80;
			PS3USB_USBbuffer[7]=PS3USB_USBbuffer[8]=PS3USB_USBbuffer[9]=PS3USB_USBbuffer[10]=PS3USB_USBbuffer[11]=PS3USB_USBbuffer[12]=PS3USB_USBbuffer[13]=0;
			PS3USB_USBbuffer[14]=PS3USB_USBbuffer[15]=PS3USB_USBbuffer[16]=PS3USB_USBbuffer[17]=PS3USB_USBbuffer[18]=0;

			if(!PCB_SQUARE) {
				PS3USB_USBbuffer[0]|=0x01;
				PS3USB_USBbuffer[14]=0xFF; //NO idea about the [14]
			}
			if(!PCB_CROSS) {
				PS3USB_USBbuffer[0]|=0x02;
				PS3USB_USBbuffer[13]=0xFF;
			}
			if(!PCB_CIRCLE) {
				PS3USB_USBbuffer[0]|=0x04;
				PS3USB_USBbuffer[12]=0xFF;
			}
			if(!PCB_TRIANGLE) {
				PS3USB_USBbuffer[0]|=0x08;
				PS3USB_USBbuffer[11]=0xFF;
			}
			if(!PCB_L1) {
				PS3USB_USBbuffer[0] |=0x10;
				PS3USB_USBbuffer[15]=0xFF; //4th punch ==L1
			}
			if(!PCB_L2) {
				PS3USB_USBbuffer[0] |=0x40;
				PS3USB_USBbuffer[17]=0xFF; //4th kick==L2
			}
			if(!PCB_R1) {
				PS3USB_USBbuffer[0] |=0x20;
				PS3USB_USBbuffer[16]=0xFF; //Fierce == R1
			}
			if(!PCB_R2) {
				PS3USB_USBbuffer[0] |=0x80;
				PS3USB_USBbuffer[18]=0xFF; //Roundhouse == R2
			}
			if(!PCB_DISABLE) {
				if(!PCB_SELECT)
					PS3USB_USBbuffer[1] |=0x01;
				if(!PCB_START)
					PS3USB_USBbuffer[1] |=0x02;
				if(!PCB_HOME)
					PS3USB_USBbuffer[1]|=0x10;
			}
			if(!PCB_L3)
				PS3USB_USBbuffer[1] |=0x04;
			if(!PCB_R3)
				PS3USB_USBbuffer[1] |=0x08;

			if(USB_STICK_MODE!=USM_ANALOG_ONLY) {
				if(!PCB_UP) {
					PS3USB_USBbuffer[2]=0x00;
					if(!PCB_RIGHT)
						PS3USB_USBbuffer[2]=0x01;
					if(!PCB_LEFT)
						PS3USB_USBbuffer [2]=0x07;
				} else {
					if(!PCB_DOWN) {
						PS3USB_USBbuffer[2]=0x04;
						if(!PCB_RIGHT)
							PS3USB_USBbuffer[2]=0x03;
						if(!PCB_LEFT)
							PS3USB_USBbuffer [2]=0x05;
					} else {//neither up nor down
						if(!PCB_RIGHT)
							PS3USB_USBbuffer[2]=0x02;
						if(!PCB_LEFT)
							PS3USB_USBbuffer [2]=0x06;
					}
				}
			}

			if(USB_STICK_MODE!=USM_DIGITAL_ONLY) {
				if(!PCB_RIGHT)
					PS3USB_USBbuffer[3]=0xFF;
				if(!PCB_LEFT)
					PS3USB_USBbuffer[3]=0x00;
				if(!PCB_UP)
					PS3USB_USBbuffer[4]=0x00;
				if(!PCB_DOWN)
					PS3USB_USBbuffer[4]=0xFF;
			}
			PS3USB_SendReport();
		}
		PS3USB_ServiceUSB();
	}
}
