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
 *
 *  fifoOperations.cpp
 *
 *  Created on: 6 Nov 2019
 *      Author: jeppa
 *
 *  Mocks of hw fifo operations implemented in fifoOperations.S
 */

#include <cstdint>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>


extern "C"
{
	bool _enqueTxData( uint8_t**          txPointer,
					   uint8_t*           bytesRemaining,
					   volatile uint32_t* txFifoPtr )
	{
		mock().actualCall( "_enqueTxData" )
			  .withParameter( "txPointer", reinterpret_cast<void*>( txPointer ) )
			  .withParameter( "bytesRemaining", reinterpret_cast<void*>( bytesRemaining ) )
			  .withParameter( "txFifoPtr", reinterpret_cast<volatile void*>( txFifoPtr ) );
		return mock().boolReturnValue();
	}


	void _dequeRxData( uint8_t**           rxPointer,
					   volatile uint32_t*  rxFifoPtr )
	{
		mock().actualCall( "_dequeRxData" )
			  .withParameter( "rxPointer", reinterpret_cast<void*>( rxPointer ) )
			  .withParameter( "rxFifoPinter", reinterpret_cast<volatile void*>( rxFifoPtr ) );
	}
}
