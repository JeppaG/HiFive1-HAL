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

/***********************************************************************/

GpioImp::gpioRegisterType* const GpioImp::gpio0 = reinterpret_cast<gpioRegisterType*>( GpioImp::gpio0BaseAddress );

GpioImp::GpioImp( gpioRegisterType* const gpio, const uint32_t gpioPin ) :
	gpioRegister( gpio ),
	pin( gpioPin )
{
}

GpioImp::~GpioImp()
{
}

void GpioImp::setAsOutput()
{
	hwRegOps::clearBits( gpioRegister->ioFunctionEnable, pin );
	hwRegOps::clearBits( gpioRegister->outputXor, pin );
	hwRegOps::clearBits( gpioRegister->port, pin );
	hwRegOps::setBits( gpioRegister->inputEnable, pin );
	hwRegOps::setBits( gpioRegister->outputEnable, pin );
}

void GpioImp::setAsInput()
{
	hwRegOps::clearBits( gpioRegister->outputEnable, pin );
	hwRegOps::clearBits( gpioRegister->ioFunctionEnable, pin );
	hwRegOps::clearBits( gpioRegister->outputXor, pin );
	hwRegOps::clearBits( gpioRegister->port, pin );
	hwRegOps::setBits( gpioRegister->inputEnable, pin );
}

void GpioImp::setAsIoFunction0()
{
	hwRegOps::clearBits( gpioRegister->ioFunctionSelect, pin );
	hwRegOps::setBits( gpioRegister->ioFunctionEnable, pin );
}

void GpioImp::setAsIoFunction1()
{
	hwRegOps::setBits( gpioRegister->ioFunctionSelect, pin );
	hwRegOps::setBits( gpioRegister->ioFunctionEnable, pin );
}

void GpioImp::vEnableRiseIrq()
{
	hwRegOps::clearBits( gpioRegister->riseIrqPending, pin );
	hwRegOps::setBits( gpioRegister->riseIrqEnable, pin );
}

void GpioImp::vDisableRiseIrq()
{
	hwRegOps::clearBits( gpioRegister->riseIrqPending, pin );
	hwRegOps::clearBits( gpioRegister->riseIrqEnable, pin );
}

void GpioImp::set()
{
	hwRegOps::setBits( gpioRegister->port, pin );
}

void GpioImp::clear()
{
	hwRegOps::clearBits( gpioRegister->port, pin );
}

Gpio::~Gpio()
{
	/* In C++ even a pure virtual destructor must have an implementation */
}
