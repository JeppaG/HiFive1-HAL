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
 * gpio.cpp
 *
 *  Created on: 22 Aug 2019
 *      Author: jeppa
 */

#include "gpio.hpp"
#include "hwRegisterOperations.hpp"

Gpio gpioObject0;

Gpio* gpio = &gpioObject0;

/***********************************************************************/

Gpio::gpioRegisterType* const Gpio::gpioRegister = reinterpret_cast<gpioRegisterType*>( Gpio::gpio0 );

Gpio::Gpio()
{
}

Gpio::~Gpio()
{
}

void Gpio::setAsOutput( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->ioFunctionEnable, pin );
	hwRegOps::clearBits( gpioRegister->outputXor, pin );
	hwRegOps::clearBits( gpioRegister->port, pin );
	hwRegOps::setBits( gpioRegister->inputEnable, pin );
	hwRegOps::setBits( gpioRegister->outputEnable, pin );
}

void Gpio::setAsInput ( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->outputEnable, pin );
	hwRegOps::clearBits( gpioRegister->ioFunctionEnable, pin );
	hwRegOps::clearBits( gpioRegister->outputXor, pin );
	hwRegOps::clearBits( gpioRegister->port, pin );
	hwRegOps::setBits( gpioRegister->inputEnable, pin );
}

void Gpio::setAsIoFunction0 ( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->ioFunctionSelect, pin );
	hwRegOps::setBits( gpioRegister->ioFunctionEnable, pin );
}

void Gpio::setAsIoFunction1 ( const uint32_t pin )
{
	hwRegOps::setBits( gpioRegister->ioFunctionSelect, pin );
	hwRegOps::setBits( gpioRegister->ioFunctionEnable, pin );
}

void Gpio::vEnableRiseIrq ( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->riseIrqPending, pin );
	hwRegOps::setBits( gpioRegister->riseIrqEnable, pin );
}

void Gpio::vDisableRiseIrq ( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->riseIrqPending, pin );
	hwRegOps::clearBits( gpioRegister->riseIrqEnable, pin );
}

void Gpio::set( const uint32_t pin )
{
	hwRegOps::setBits( gpioRegister->port, pin );
}

void Gpio::clear( const uint32_t pin )
{
	hwRegOps::clearBits( gpioRegister->port, pin );
}
