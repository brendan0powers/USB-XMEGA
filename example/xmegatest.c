// Demo USB device for ATxmega32a4u
// http://nonolithlabs.com
// (C) 2011 Kevin Mehall (Nonolith Labs) <km@kevinmehall.net>
//
// Licensed under the terms of the GNU GPLv3+

#include "xmegatest.h"

unsigned int timer = 15625; // 500ms
unsigned char bulkdatain[64];
unsigned char bulkdataout[64];


void configureEndpoint(void){
	USB_ep_in_init(1, USB_EP_TYPE_BULK_gc, 64);
	USB_ep_in_start(1, bulkdatain, 64);
	
	USB_ep_out_init(2, USB_EP_TYPE_BULK_gc, 64);
	USB_ep_out_start(2, bulkdataout);
}

uint8_t outcntr = 0;

void pollEndpoint(void){	
	if (USB_ep_out_received(2)){
		USB_ep_out_start(2, bulkdataout);
		outcntr++;
	}
}

int main(void){
	SetupHardware();
	sei();
	
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc; // 31.25KHz = 0.032ms
	
	configureEndpoint();
	
	while (1){
		while(TCC0.CNT < timer){ 
			USB_Task();
			pollEndpoint();
		}
		PORTE.OUTTGL = (1<<0);
    	TCC0.CNT=0;
	}
}

/** Configures the board hardware and chip peripherals for the project's functionality. */
void SetupHardware(void){
	PORTE.DIRSET = (1<<0) | (1<<1);
	PORTE.OUTSET = (1<<0);
	
	USB_ConfigureClock();
	USB_Init();
}

/** Event handler for the library USB Control Request reception event. */
bool EVENT_USB_Device_ControlRequest(USB_Request_Header_t* req){
	if ((req->bmRequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_VENDOR){
		if (req->bRequest == 0x23){
			ep0_buf_in[0] = outcntr;
			ep0_buf_in[1] = bulkdataout[0];
			ep0_buf_in[2] = endpoints[2].out.CNTL;
			ep0_buf_in[3] = endpoints[2].out.STATUS;
			USB_ep0_send(4);
			return true;
		}else if (req->bRequest == 0x24){
			USB_ep_out_init(2, USB_EP_TYPE_BULK_gc, 64);
			USB_ep_out_start(2, bulkdataout);
			outcntr = 0;
			USB_ep0_send(0);
		}else if (req->bRequest == 0xBB){
			USB_ep0_send(0);
			USB_ep0_wait_for_complete();
			_delay_us(10000);
			USB_Detach();
			_delay_us(100000);
			void (*enter_bootloader)(void) = 0x47fc /*0x8ff8/2*/;
			enter_bootloader();
		}
	}
	
	return false;
}

