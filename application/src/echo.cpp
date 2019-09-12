/*
 * Copyright 2019 JG Mechatronics AB
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *****************************************************************************
 * echo.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: jeppa
 */
#include <stdio.h>
#include "interrupt.hpp"
#include "clock.hpp"
#include "timer.hpp"
#include "gpio.hpp"
#include "uart.hpp"
#include "minimumCppRuntime.hpp"

Clock clock;
Interrupt uart0Interrupt( Interrupt::uart0 );

extern "C"
{
	extern void _enable_interrupt();
}

int main ()
{
	uint8_t stringToPrint[32] = {'H','e','l','l','o',' ','w','o','r','l','d','\n','\r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t length = 13;
	Interrupt::init();
	Interrupt::setThreshold( 0 );
	int count = 0;
	clock.usePllWithHfXosc();
	timer1->setUp1MsTimeBase();
	gpio->setAsIoFunction0( Gpio::pin16 ); /* UART0 RX */
	gpio->setAsIoFunction0( Gpio::pin17 ); /* UART0 TX */
	uart0->enableTx();
	uart0->enableRx();
	uart0Interrupt.enable();
	_enable_interrupt();
	gpio->setAsOutput( Gpio::pin19 );
	gpio->setAsOutput( Gpio::pin21 );
	gpio->setAsOutput( Gpio::pin22 );
	gpio->set( Gpio::pin19 );
	gpio->clear( Gpio::pin21 );
	gpio->set( Gpio::pin22 );
	while ( 1 )
	{
		if ( 500 == count )
		{
			gpio->clear( Gpio::pin21 );
			uart0->transmit( stringToPrint, length );
		}
		if ( 1000 == count )
		{
			count = 0;
			gpio->set( Gpio::pin21 );
			length = uart0->receive( stringToPrint );
		}
		timer1->waitFor1MsTimeOut();
		count++;
	}
	return 0;
}


