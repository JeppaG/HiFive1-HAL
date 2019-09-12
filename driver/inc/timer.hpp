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
 * timer.hpp
 *
 *  Created on: 20 Aug 2019
 *      Author: jeppa
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <cstdint>

class Timer {
public:
	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/
	static constexpr uint8_t pwm0 = 0;
	static constexpr uint8_t pwm1 = 1;
	static constexpr uint8_t pwm2 = 2;

	Timer( const uint8_t timerSelect );

	virtual ~Timer();

	void setUp1MsTimeBase();

	void waitFor1MsTimeOut ();

	/* The interrupt handlers are declared public, since they are needed in the global
	 * interrupt vector table. They are not meant to be accessed through the interface of the class
	 */
	static void pwm1cmp0handler();

private:

	typedef struct {
      volatile       uint32_t pwmCfg;          /* Base address + 0  */
      volatile       uint32_t reserved1;       /* Base address + 4  */
      volatile       uint32_t pwmCount;        /* Base address + 8  */
      volatile       uint32_t reserved2;       /* Base address + 12 */
      volatile       uint32_t pwmCountScaled;  /* Base address + 16, the "pwms" register */
      volatile 	     uint32_t reserved3;       /* Base address + 20 */
      volatile       uint32_t reserved4;       /* Base address + 24 */
      volatile       uint32_t reserved5;       /* Base address + 28 */
      volatile       uint32_t pwmCmp0;         /* Base address + 32 */
      volatile       uint32_t pwmCmp1;         /* Base address + 36 */
      volatile       uint32_t pwmCmp2;         /* Base address + 40 */
      volatile       uint32_t pwmCmp3;         /* Base address + 44 */

	} timerRegisterType;

	static timerRegisterType* const timerRegisterPwm0;
	static timerRegisterType* const timerRegisterPwm1;
	static timerRegisterType* const timerRegisterPwm2;

	timerRegisterType* timerRegister;

	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/

	/* Timer registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t pwm0BaseAddress = 0x10015000u;
	static constexpr uint32_t pwm1BaseAddress = 0x10025000u;
	static constexpr uint32_t pwm2BaseAddress = 0x10035000u;

	/* The tickTimer is set up to generate a 1 ms system tick. With 256MHz bus clock, this is
	 * accomplished by dividin the clock with 1024, i.e the counter register is shifted 10 bits
	 * and the count is reset when comparing to 249. This gives 250 counting cycles and
	 * each cycle is 1024/256000000 = 4 us. 250*4us = 1 ms
	 *
	 * The pwmCmp0Ip flag is set when the counter reaches 249. It can then be cleared by writing
	 * 0 to the pwmCmp0Ip bit in the  pwmCfg register */

	static constexpr uint32_t tickTimerConfig = 0x0000130Au;
	                                  /* 0000 0000 0000 0000 0001 0011 0000 1010
				                       * |||| ||||      ||||   ||  |||      ||||
				                       * |||| ||||      ||||   ||  |||      ++++-pwmScale = 10 256MHz/1024 = 250 kHz
				                       * |||| ||||      ||||   ||  ||+-++++------pwmSticky - disallow clearing the ip bits
				                       * |||| ||||      ||||   ||  |+------------pwmZeroComp = 1 reset counter when match to cmp0
				                       * |||| ||||      ||||   ||  +-------------pwmDeGlitch = 0 comparator output is not latched
				                       * |||| ||||      ||||   |+----------------pwmEnAlways = 1, Always on
				                       * |||| ||||      ||||   +-----------------pwmEnOneShot = 0, Not used in always on mode.
				                       * |||| ||||      ++++---------------------pwmCmpXCenter = 0, No center aligned PWM
				                       * |||| ++++-------------------------------pwmCmpXGang = 0, No ganging of comparators
				                       * ++++------------------------------------pwmCmpXIp = 0, Clear all pending interrupt flags to 0.
	                                   */

	static constexpr uint32_t tickTimerPwm0CmpConfig = 249u;

	static constexpr uint32_t cmp0IrqMask = 0x10000000u;

	/*********************************************************************/

};

extern Timer* timer0;
extern Timer* timer1;
extern Timer* timer2;

#endif /* TIMER_HPP_ */
