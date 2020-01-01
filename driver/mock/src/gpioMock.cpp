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
 * goioMock.cpp
 *
 *  Created on: 24 Nov 2019
 *      Author: jeppa
 */

#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "gpioMock.hpp"

GpioMock::GpioMock()
{
}

void GpioMock::setAsOutput ()
{
	mock().actualCall( "setAsOutput" )
		  .onObject( this );
}

void GpioMock::setAsInput ()
{
	mock().actualCall( "setAsInput" )
	      .onObject( this );
}

void GpioMock::setAsIoFunction0 ()
{
	mock().actualCall( "setAsIoFunction0" )
		  .onObject( this );
}

void GpioMock::setAsIoFunction1 ()
{
	mock().actualCall( "setAsIoFunction1" )
		  .onObject( this );
}

void GpioMock::vEnableRiseIrq ()
{
	mock().actualCall( "enableRiseIrq" )
	      .onObject( this );
}

void GpioMock::vDisableRiseIrq ()
{
	mock().actualCall( "disableRiseIrq" )
	      .onObject( this );
}

void GpioMock::set ()
{
	mock().actualCall( "set" )
		  .onObject( this );
}

void GpioMock::clear ()
{
	mock().actualCall( "clear" )
		  .onObject( this );
}

GpioMock::~GpioMock()
{

}

Gpio::~Gpio()
{
	/* C++ demands that a pure virtual destructor has an implementation */
}






