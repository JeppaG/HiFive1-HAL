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
 * hwRegisterOperations.cpp
 *
 *  Created on: 15 Aug 2019
 *      Author: jeppa
 */
#include "hwRegisterOperations.hpp"

inline bool hwRegOps::compareBits( volatile uint32_t &reg, const uint32_t bits )
{
    return ( bits == ( reg & bits ) );
}

inline void hwRegOps::setBits( volatile uint32_t &reg, const uint32_t bits )
{
	reg = ( reg | bits );
}

inline void hwRegOps::clearBits ( volatile uint32_t &reg, const uint32_t bits )
{
	reg = ( reg & ~bits );
}

inline void hwRegOps::setRegister ( volatile uint32_t &reg, const uint32_t mask, const uint32_t newSetting )
{
	uint32_t newReg = reg;
	newReg |= ( newSetting & mask ); /* set all bits that are '1' in both mask and newSetting to '1' */
	newReg &= ~( mask & ~newSetting  );  /* set all bits that are '1' in mask and '0' in newSetting to '0' */
	reg = newReg;
}



