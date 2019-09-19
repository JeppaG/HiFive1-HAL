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

ClockImp clockImp( ClockImp::crti );
Clock* clock = dynamic_cast<Clock*>( &clockImp );

ClintInterrupt plicClintInterrupt ( ClintInterrupt::external );
Interrupt* plicInterrupt = dynamic_cast<Interrupt*>( &plicClintInterrupt );

PlicInterrupt uart0PlicInterrupt( PlicInterrupt::uart0 );
Interrupt* uart0Interrupt = dynamic_cast<Interrupt*>( &uart0PlicInterrupt );

GpioImp uartRxPinImp( GpioImp::gpio0, GpioImp::pin16 );
Gpio* uartRxPin = dynamic_cast<Gpio*>( &uartRxPinImp );

GpioImp uartTxPinImp( GpioImp::gpio0, GpioImp::pin17 );
Gpio* uartTxPin = dynamic_cast<Gpio*>( &uartTxPinImp );

GpioImp greenLedPinImp( GpioImp::gpio0, GpioImp::pin19 );
Gpio* greenLedPin = dynamic_cast<Gpio*>( &greenLedPinImp );

GpioImp blueLedPinImp( GpioImp::gpio0, GpioImp::pin21 );
Gpio* blueLedPin = dynamic_cast<Gpio*>( &blueLedPinImp );

GpioImp redLedPinImp( GpioImp::gpio0, GpioImp::pin22 );
Gpio* redLedPin = dynamic_cast<Gpio*>( &redLedPinImp );

UartImp uart0Imp( UartImp::uart0Register, 115200u );
Uart* uart0 = dynamic_cast<Uart*>( &uart0Imp );

PwmImp pwm1Imp( PwmImp::pwm1 );
Pwm* pwm1 = dynamic_cast<Pwm*>( &pwm1Imp );

extern "C"
{
	extern void _enable_interrupt();
}

int main ()
{
	uint8_t stringToPrint[32] = {'H','e','l','l','o',' ','w','o','r','l','d','\n','\r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t length = 13;
	PlicInterrupt::init();
	PlicInterrupt::setThreshold( 0 );
	int count = 0;
	clock->usePllWithHfXosc();
	pwm1->setUp1MsTimeBase();
	uartRxPin->setAsIoFunction0(); /* UART0 RX */
	uartTxPin->setAsIoFunction0(); /* UART0 TX */
	uart0->enableTx();
	uart0->enableRx();
	uart0Interrupt->enable();
	plicInterrupt->enable();
	_enable_interrupt();
	greenLedPin->setAsOutput();
	blueLedPin->setAsOutput();
	redLedPin->setAsOutput();
	greenLedPin->set();
	blueLedPin->clear();
	redLedPin->set();
	while ( 1 )
	{
		if ( 500 == count )
		{
			blueLedPin->clear();
			uart0->transmit( stringToPrint, length );
		}
		if ( 1000 == count )
		{
			count = 0;
			blueLedPin->set();
			length = uart0->receive( stringToPrint );
		}
		pwm1->waitFor1MsTimeOut();
		count++;
	}
	return 0;
}


