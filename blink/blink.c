// Demo USB device for ATxmega32a4u
// http://nonolithlabs.com
// (C) 2011 Kevin Mehall (Nonolith Labs) <km@kevinmehall.net>
//
// Licensed under the terms of the GNU GPLv3+

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>


int main(void)
{
	//sei();
	
	TCC0.CTRLA = 0x1;
	PORTD.DIRSET = 0x01;	

	while (1)
	{
		PORTD.OUT = 0x01;
		
		while(TCC0.CNT < 200)
			__asm__("nop");

		PORTD.OUT = 0;

		while(TCC0.CNT < 255)
                        __asm__("nop");

		TCC0.CNT = 0;
	}
}

