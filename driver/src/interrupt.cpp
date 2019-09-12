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
 * interrupt.cpp
 *
 *  Created on: 27 Aug 2019
 *      Author: jeppa
 */

#include "interrupt.hpp"
#include "hwRegisterOperations.hpp"
#include "timer.hpp"
#include "uart.hpp"

extern "C"
{
	extern void _set_trap_vector();
	extern void _disable_all_plic_interrupts();
	extern void _enable_plic_interrupt( uint32_t interruptId );
}

/*************** Declaration of static members **********************/
volatile  uint32_t* const Interrupt::priority = reinterpret_cast<uint32_t*>( plicBaseAddress );
volatile  uint32_t* const Interrupt::pendingArray = reinterpret_cast<uint32_t*>( plicBaseAddress + pendingArrayOffset );
volatile  uint32_t* const Interrupt::enableArray = reinterpret_cast<uint32_t*>( plicBaseAddress + enableArrayOffset );
volatile  uint32_t* const Interrupt::priorityThreshold = reinterpret_cast<uint32_t*>( plicBaseAddress + priorityThresholdOffset );
volatile  uint32_t* const Interrupt::claimComplete = reinterpret_cast<uint32_t*>( plicBaseAddress + claimCompleteOffset );


/***********************************************************************/
void Interrupt::init()
{
	_set_trap_vector();
	_disable_all_plic_interrupts();
}

Interrupt::Interrupt( const int8_t interruptSelect ) :
    source( interruptSelect )
{
}

Interrupt::~Interrupt()
{
}

void Interrupt::setThreshold ( uint32_t threshold )
{
	*priorityThreshold = threshold;
}

void Interrupt::setPriority( uint8_t prio )
{
	priority[source] = prio;
}

void Interrupt::enable ()
{
	extern void _enable_plic_interrupt( uint32_t interruptNumber );

	_enable_plic_interrupt ( source );
	priority[source] = 7;
}

void Interrupt::disable ()
{
	if ( 32 > source )
	{
		uint32_t bit = 1 << source;
		hwRegOps::clearBits( enableArray[0], bit );
	}
	else
	{
		uint32_t bit = 1 << ( source - 32 );
		hwRegOps::clearBits( enableArray[1], bit );
	}
}

void Interrupt::defaultHandler()
{
//	while (1);
}


/*** Interrupt Vector Table ***/
extern "C"
{
	extern const Interrupt::handlerType interruptVectorTable[Interrupt::numberOfSources] = {
		/* notUsed  */ Interrupt::defaultHandler,
		/* watchdog */ Interrupt::defaultHandler,
		/* rtc      */ Interrupt::defaultHandler,
		/* uart0    */ Uart::uart0InterruptHandler,
		/* uart1    */ Interrupt::defaultHandler,
		/* qspiI0   */ Interrupt::defaultHandler,
		/* qspiI1   */ Interrupt::defaultHandler,
		/* qspiI2   */ Interrupt::defaultHandler,
		/* gpio0    */ Interrupt::defaultHandler,
		/* gpio1    */ Interrupt::defaultHandler,
		/* gpio2    */ Interrupt::defaultHandler,
		/* gpio3    */ Interrupt::defaultHandler,
		/* gpio4    */ Interrupt::defaultHandler,
		/* gpio5    */ Interrupt::defaultHandler,
		/* gpio6    */ Interrupt::defaultHandler,
		/* gpio7    */ Interrupt::defaultHandler,
		/* gpio8    */ Interrupt::defaultHandler,
		/* gpio9    */ Interrupt::defaultHandler,
		/* gpio10   */ Interrupt::defaultHandler,
		/* gpio11   */ Interrupt::defaultHandler,
		/* gpio12   */ Interrupt::defaultHandler,
		/* gpio13   */ Interrupt::defaultHandler,
		/* gpio14   */ Interrupt::defaultHandler,
		/* gpio15   */ Interrupt::defaultHandler,
		/* gpio16   */ Interrupt::defaultHandler,
		/* gpio17   */ Interrupt::defaultHandler,
		/* gpio18   */ Interrupt::defaultHandler,
		/* gpio19   */ Interrupt::defaultHandler,
		/* gpio20   */ Interrupt::defaultHandler,
		/* gpio21   */ Interrupt::defaultHandler,
		/* gpio22   */ Interrupt::defaultHandler,
		/* gpio23   */ Interrupt::defaultHandler,
		/* gpio24   */ Interrupt::defaultHandler,
		/* gpio25   */ Interrupt::defaultHandler,
		/* gpio26   */ Interrupt::defaultHandler,
		/* gpio27   */ Interrupt::defaultHandler,
		/* gpio28   */ Interrupt::defaultHandler,
		/* gpio29   */ Interrupt::defaultHandler,
		/* gpio30   */ Interrupt::defaultHandler,
		/* gpio31   */ Interrupt::defaultHandler,
		/* pwm0cmp0 */ Interrupt::defaultHandler,
		/* pwm0cmp1 */ Interrupt::defaultHandler,
		/* pwm0cmp2 */ Interrupt::defaultHandler,
		/* pwm0cmp3 */ Interrupt::defaultHandler,
		/* pwm1cmp0 */ Timer::pwm1cmp0handler,
		/* pwm1cmp1 */ Interrupt::defaultHandler,
		/* pwm1cmp2 */ Interrupt::defaultHandler,
		/* pwm1cmp3 */ Interrupt::defaultHandler,
		/* pwm2cmp0 */ Interrupt::defaultHandler,
		/* pwm2cmp1 */ Interrupt::defaultHandler,
		/* pwm2cmp2 */ Interrupt::defaultHandler,
	    /* pwm2cmp3 */ Interrupt::defaultHandler
	};
};
