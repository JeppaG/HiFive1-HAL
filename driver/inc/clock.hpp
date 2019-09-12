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
 * clock.hpp
 *
 *  Created on: 14 Aug 2019
 *      Author: jeppa
 */

#ifndef CLOCK_HPP_
#define CLOCK_HPP_

#include <cstdint>

class Clock {
public:
	Clock();

	virtual ~Clock();

    void usePllWithHfXosc();

private:

	typedef struct {
      volatile       uint32_t hfRoscCfg;       /* Base address + 0  */
      volatile       uint32_t hfXoscCfg;       /* Base address + 4  */
      volatile       uint32_t pllCfg;          /* Base address + 8  */
      volatile       uint32_t pllOutDiv;       /* Base address + 12 */
	} clockRegisterType;

	static clockRegisterType* const clockRegister;

	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/

	/* Set the pointers to the crti registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t crtiBaseAddress = 0x10008000u;

	static constexpr uint32_t hfRoscEnableBit = 0x40000000u;
	static constexpr uint32_t hfRoscReadyBit  = 0x80000000u;

	static constexpr uint32_t hfXoscEnableBit = 0x40000000u;
	static constexpr uint32_t hfXoscReadyBit  = 0x80000000u;

	/* The values of the PLL dividers are set up so that the output core_clk is run at 256 MHz
     * with input from the 16 MHz hfXosc clock. To fulfil the conditions given in the FE310-G000
     * manual, this means that
     * - the input divider must be set to divide by 2 so that refr becomes 8 MHz, i.e pllr = 1,
     * - the vco multiplier must be set to multiply with 64 so that vco becomes 512 MHz, i.e. pllf = 31, since 2*(1+31) = 64,
     * - the output divider must be set to 2, so that the output becomes 2556 MHz, i.e. pllq = = 1.
     */
	static constexpr uint32_t pllSetUp = 0x000025F1u;
	                                  /* 0000 0000 0000 0000 0010 0101 1111 0001
				                       *                       |  |||| ||||   ||
				                       *                       |  |||| ||||   ++-pllr = 1
				                       *                       |  ||++-++++------pllf = 31 = 0x1F
				                       *                       |  ++-------------pllq = 1
				                       *                       +-----------------reference selet '1' -> hfXosc
	                                   */
	static constexpr uint32_t pllSetUpMask = 0x00002FF3u; /* One in all bits related to pllr, pllf, pllq and refsel */

	static constexpr uint32_t pllSelBit = 0x00010000u;    /* Select the pll clock as coreclk */
	static constexpr uint32_t pllByPassBit = 0x00040000u; /* Select pll turned off (1) or on (0), bit 18 */
	static constexpr uint32_t pllLockBit = 0x80000000u;   /* Indicates if the pll has acquired lock, bit 31 */

	static constexpr uint32_t pllOutputDiv = 0x00000100u;
                                          /* 0000 0000 0000 00000 0000 0001 0000 0000
                                           *                              |   || ||||
                                           *                              |   ++-++++-plloutdiv
                                           *                              +-----------plloutdivby1
                                           */
	static constexpr uint32_t pllOutputDivMask = 0x0000013Fu; /* One in all bits related to the pll output divider */

	/*********************************************************************/

	void enableHfXosc();
	void enablePll();
	void selectPll();
	void disableHfRosc();
};

#endif /* CLOCK_HPP_ */
