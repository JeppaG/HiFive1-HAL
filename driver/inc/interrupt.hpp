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
 *  Created on: 27 Aug 2019
 *      Author: jeppa
 */

#ifndef INTERRUPT_HPP_
#define INTERRUPT_HPP_

#include <cstdint>

class Interrupt {
public:
	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/

	static constexpr int8_t	notUsed  = 0;
	static constexpr int8_t	watchdog = 1;
	static constexpr int8_t	rtc      = 2;
	static constexpr int8_t	uart0    = 3;
	static constexpr int8_t	uart1    = 4;
	static constexpr int8_t	qspiI0   = 5;
	static constexpr int8_t	qspiI1   = 6;
	static constexpr int8_t	qspiI2   = 7;
	static constexpr int8_t	gpio0    = 8;
	static constexpr int8_t	gpio1    = 9;
	static constexpr int8_t	gpio2    = 10;
	static constexpr int8_t	gpio3    = 11;
	static constexpr int8_t	gpio4    = 12;
	static constexpr int8_t	gpio5    = 13;
	static constexpr int8_t	gpio6    = 14;
	static constexpr int8_t	gpio7    = 15;
	static constexpr int8_t	gpio8    = 16;
	static constexpr int8_t	gpio9    = 17;
	static constexpr int8_t	gpio10   = 18;
	static constexpr int8_t	gpio11   = 19;
	static constexpr int8_t	gpio12   = 20;
	static constexpr int8_t	gpio13   = 21;
	static constexpr int8_t	gpio14   = 22;
	static constexpr int8_t	gpio15   = 23;
	static constexpr int8_t	gpio16   = 24;
	static constexpr int8_t	gpio17   = 25;
	static constexpr int8_t	gpio18   = 26;
	static constexpr int8_t	gpio19   = 27;
	static constexpr int8_t	gpio20   = 28;
	static constexpr int8_t	gpio21   = 29;
	static constexpr int8_t	gpio22   = 30;
	static constexpr int8_t	gpio23   = 31;
	static constexpr int8_t	gpio24   = 32;
	static constexpr int8_t	gpio25   = 33;
	static constexpr int8_t	gpio26   = 34;
	static constexpr int8_t	gpio27   = 35;
	static constexpr int8_t	gpio28   = 36;
	static constexpr int8_t	gpio29   = 37;
	static constexpr int8_t	gpio30   = 38;
	static constexpr int8_t	gpio31   = 39;
	static constexpr int8_t	pwm0cmp0 = 40;
	static constexpr int8_t	pwm0cmp1 = 41;
	static constexpr int8_t	pwm0cmp2 = 42;
	static constexpr int8_t	pwm0cmp3 = 43;
	static constexpr int8_t	pwm1cmp0 = 44;
	static constexpr int8_t	pwm1cmp1 = 45;
	static constexpr int8_t	pwm1cmp2 = 46;
	static constexpr int8_t	pwm1cmp3 = 47;
	static constexpr int8_t	pwm2cmp0 = 48;
	static constexpr int8_t	pwm2cmp1 = 49;
	static constexpr int8_t	pwm2cmp2 = 50;
	static constexpr int8_t	pwm2cmp3 = 51;
	static constexpr int8_t	numberOfSources = 52;

	typedef void (*handlerType)(void);

	static void init();

	Interrupt( const int8_t interruptSelect );

	virtual ~Interrupt();

	static void setThreshold ( uint32_t threshold );

	void setPriority( uint8_t prioy );

 	void enable ();

	void disable ();

	static void defaultHandler();

private:

	static volatile  uint32_t* const priority;
	static volatile  uint32_t* const pendingArray;
	static volatile  uint32_t* const enableArray;
	static volatile  uint32_t* const priorityThreshold;
	static volatile  uint32_t* const claimComplete;

	const  uint8_t   source;


	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/
	/* plic registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t plicBaseAddress = 0x0C000000u;
    /* plic register address offsets as indicated in the fe310-G000 chip manual */
	static constexpr uint32_t priorityOffset          = 0;
	static constexpr uint32_t pendingArrayOffset      = 0x1000;
	static constexpr uint32_t enableArrayOffset       = 0x2000;
	static constexpr uint32_t priorityThresholdOffset = 0x200000;
	static constexpr uint32_t claimCompleteOffset     = 0x200004;


};

#endif /* INTERRUPT_HPP_ */
