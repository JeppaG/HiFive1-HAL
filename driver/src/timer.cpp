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
 * timer.cpp
 *
 *  Created on: 20 Aug 2019
 *      Author: jeppa
 */

#include "timer.hpp"
#include "hwRegisterOperations.hpp"

/***********************************************************************/
PwmImp::pwmRegisterType* const PwmImp::pwm0 = reinterpret_cast<pwmRegisterType* const>( pwm0BaseAddress );
PwmImp::pwmRegisterType* const PwmImp::pwm1 = reinterpret_cast<pwmRegisterType* const>( pwm1BaseAddress );
PwmImp::pwmRegisterType* const PwmImp::pwm2 = reinterpret_cast<pwmRegisterType* const>( pwm2BaseAddress );

PwmImp::PwmImp( pwmRegisterType* pwmSelect ) :
	pwmRegister( pwmSelect )
{
}

PwmImp::~PwmImp()
{
}

void PwmImp::setUp1MsTimeBase()
{
	pwmRegister->pwmCmp0 = tickTimerPwm0CmpConfig;
	pwmRegister->pwmCfg = tickTimerConfig;
}

void PwmImp::waitFor1MsTimeOut()
{
	while ( false == hwRegOps::compareBits( pwmRegister->pwmCfg, cmp0IrqMask )  );
	hwRegOps::clearBits( pwmRegister->pwmCfg, cmp0IrqMask );
}

/* Interrupt handlers */
void PwmImp::pwm1cmp0handler()
{
	static uint32_t count = 0;

	hwRegOps::clearBits( pwm1->pwmCfg, cmp0IrqMask );
	count++;
	if ( 250 == count )
	{
	//	gpio->clear( Gpio::pin21 );
	}
	else if ( 500 == count )
	{
	//	gpio->set( Gpio::pin21 );
		count = 0;
	}
}

Pwm::~Pwm()
{
	/* C++ demands that even a pure virtual destrutor has an implementation */
}
