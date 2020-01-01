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
 * spiAdc.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: jeppa
 */
#include <stdio.h>
#include <cstring>

#include "board.hpp"
#include "minimumCppRuntime.hpp"

extern "C"
{
	extern void _enable_interrupt();
}

int main ()
{
	uint8_t stringToPrint[32] = {'H','e','l','l','o',' ','w','o','r','l','d','\n','\r',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t length = 13;
	uint8_t spiString[21] = {'T','e','s','t',' ','o','f',' ','S','P','I',' ','I','n','t','e','r','f','a','c','e'};
	uint8_t spiBufferArea[21] = {0};
	uint8_t* spiBuffer = spiBufferArea;
	PlicInterrupt::init();
	PlicInterrupt::setThreshold( 0 );
	int count = 0;
	board::clock->usePllWithHfXosc();
	board::pwm1->setUp1MsTimeBase();
	board::uart0->enableTx();
	board::uart0->enableRx();
	board::uart0Interrupt->enable();
	board::spi1Interrupt->enable();
	board::plicInterrupt->enable();
	_enable_interrupt();
	board::greenLedPin->setAsOutput();
	board::blueLedPin->setAsOutput();
	board::redLedPin->setAsOutput();
	board::greenLedPin->set();
	board::blueLedPin->clear();
	board::redLedPin->set();
	board::uart0->transmit( spiString, 21 );
	while ( 1 )
	{
		if ( 500 == count )
		{
			board::blueLedPin->clear();
			char prtString[20];
			int32_t voltageInMilliVolt = ( static_cast<int32_t>( spiBuffer[0] )*256 + static_cast<int32_t>( spiBuffer[1] ) );
			voltageInMilliVolt *= 5000;
			voltageInMilliVolt += 2048;
			voltageInMilliVolt /= 4096;
			sprintf( prtString, "%i\r\n", voltageInMilliVolt );
			const uint8_t* prtPtr = (uint8_t*)( &prtString[0] );
			board::uart0->transmit( prtPtr, 7 );
			memcpy( spiBuffer, spiString, 2 );
			board::spi1->transceive( spiBuffer, 2 );
		}
		if ( 1000 == count )
		{
			count = 0;
			board::blueLedPin->set();
			length = board::uart0->receive( stringToPrint );
		}
		board::pwm1->waitFor1MsTimeOut();
		count++;
	}
	return 0;
}


