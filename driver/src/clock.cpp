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
 * clock.cpp
 *
 *  Created on: 14 Aug 2019
 *      Author: jeppa
 */

#include "clock.hpp"
#include "hwRegisterOperations.hpp"



/***********************************************************************/

Clock::clockRegisterType* const Clock::clockRegister = reinterpret_cast<Clock::clockRegisterType*>( Clock::crtiBaseAddress );

Clock::Clock()
{
}

Clock::~Clock()
{
}

void Clock::usePllWithHfXosc()
{
	enableHfXosc();
	enablePll();
	selectPll();
	disableHfRosc();
}

void Clock::enableHfXosc()
{
	hwRegOps::setBits( clockRegister->hfXoscCfg, hfXoscEnableBit );
	while ( false == hwRegOps::compareBits( clockRegister->hfXoscCfg, hfXoscReadyBit ) )
	{
		/* Wait for the hfXosc to become ready before proceeding */
	}
}

void Clock::enablePll()
{
    /* Set up dividers and multipliers inside the pll according to compile-time settings */
	hwRegOps::setRegister( clockRegister->pllCfg, pllSetUpMask, pllSetUp );
	/* select output division of the PLL clock according to compile-time settings */
	hwRegOps::setRegister( clockRegister->pllOutDiv, pllOutputDivMask, pllOutputDiv );
	/* Enable the pll */
	hwRegOps::clearBits( clockRegister->pllCfg, pllByPassBit );

	/* wait for at least 100 us before polling the lock bit */
	for ( uint32_t i = 0u; i < 20000; i++ )
	{
		/* Wait for 20000 cycles -> 1000 us at 20 MHz */
	}

	while ( false == hwRegOps::compareBits( clockRegister->pllCfg, pllLockBit ) )
	{
		/* Wait for the lock bit to signal that the PLL has locked */
	}
}

void Clock::selectPll()
{
	hwRegOps::setBits( clockRegister->pllCfg, pllSelBit );
}

void Clock::disableHfRosc()
{
    hwRegOps::clearBits( clockRegister->hfRoscCfg, hfRoscEnableBit );
}
