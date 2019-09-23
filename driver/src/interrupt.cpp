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
#include "spi.hpp"

extern "C"
{
	extern void _set_trap_vector();
	extern void _disable_all_plic_interrupts();
	extern void _enable_plic_interrupt( uint32_t interruptId );
}

ClintInterrupt::ClintInterrupt( const uint16_t interruptId ):
	interruptBit ( interruptId )
{
}

ClintInterrupt::~ClintInterrupt()
{
}

void ClintInterrupt::setPriority( uint8_t prio )
{
	/* The priority is fixed for Clint interrupts */
}

void ClintInterrupt::enable ()
{
	asm volatile( "   csrrs zero, mie, %0;"  /* Set interrupt enable bit in csr mie */
		 : /* Output Operands */ /* None */
		 : /* Input Operands */  "r" ( interruptBit )  /* %0 */
		 : /* Clobbered Registers */ /* None */
    );
}

void ClintInterrupt::disable ()
{
	asm volatile( "   csrrc zero, mie, %0;"  /* Clear interrupt enable bit in csr mie */
		 : /* Output Operands */ /* None */
		 : /* Input Operands */  "r" ( interruptBit )  /* %0 */
		 : /* Clobbered Registers */ /* None */
    );
}

void ClintInterrupt::defaultHandler()
{
//	while (1);
}

/*********************************************************************************************************
 * Clint Interrupt vectors
 */
extern "C"
{
    extern const Interrupt::handlerType _tickTimerHandler = ClintInterrupt::defaultHandler;
    extern const Interrupt::handlerType _swInterruptHandler = ClintInterrupt::defaultHandler;
}

/*************** Declaration of static members **********************/
volatile  uint32_t* const PlicInterrupt::priority = reinterpret_cast<uint32_t*>( plicBaseAddress );
volatile  uint32_t* const PlicInterrupt::pendingArray = reinterpret_cast<uint32_t*>( plicBaseAddress + pendingArrayOffset );
volatile  uint32_t* const PlicInterrupt::enableArray = reinterpret_cast<uint32_t*>( plicBaseAddress + enableArrayOffset );
volatile  uint32_t* const PlicInterrupt::priorityThreshold = reinterpret_cast<uint32_t*>( plicBaseAddress + priorityThresholdOffset );
volatile  uint32_t* const PlicInterrupt::claimComplete = reinterpret_cast<uint32_t*>( plicBaseAddress + claimCompleteOffset );


/***********************************************************************/
void PlicInterrupt::init()
{
	_set_trap_vector();
	_disable_all_plic_interrupts();
}

PlicInterrupt::PlicInterrupt( const int8_t interruptSelect ) :
    source( interruptSelect )
{
}

PlicInterrupt::~PlicInterrupt()
{
}

void PlicInterrupt::setThreshold ( uint32_t threshold )
{
	*priorityThreshold = threshold;
}

void PlicInterrupt::setPriority( uint8_t prio )
{
	if ( 0 < source )
	{
		priority[source] = prio;
	}
}

void PlicInterrupt::enable ()
{
	extern void _enable_plic_interrupt( uint32_t interruptNumber );

	_enable_plic_interrupt ( source );
    priority[source] = 7;
}

void PlicInterrupt::disable ()
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

void PlicInterrupt::defaultHandler()
{
//	while (1);
}

/*** PLIC Interrupt Vector Table ***/
extern "C"
{
	extern const Interrupt::handlerType interruptVectorTable[PlicInterrupt::numberOfSources] = {
		/* notUsed  */ PlicInterrupt::defaultHandler,
		/* watchdog */ PlicInterrupt::defaultHandler,
		/* rtc      */ PlicInterrupt::defaultHandler,
		/* uart0    */ UartImp::uart0InterruptHandler,
		/* uart1    */ PlicInterrupt::defaultHandler,
		/* qspiI0   */ PlicInterrupt::defaultHandler,
		/* qspiI1   */ SpiImp::spi1InterruptHandler,
		/* qspiI2   */ PlicInterrupt::defaultHandler,
		/* gpio0    */ PlicInterrupt::defaultHandler,
		/* gpio1    */ PlicInterrupt::defaultHandler,
		/* gpio2    */ PlicInterrupt::defaultHandler,
		/* gpio3    */ PlicInterrupt::defaultHandler,
		/* gpio4    */ PlicInterrupt::defaultHandler,
		/* gpio5    */ PlicInterrupt::defaultHandler,
		/* gpio6    */ PlicInterrupt::defaultHandler,
		/* gpio7    */ PlicInterrupt::defaultHandler,
		/* gpio8    */ PlicInterrupt::defaultHandler,
		/* gpio9    */ PlicInterrupt::defaultHandler,
		/* gpio10   */ PlicInterrupt::defaultHandler,
		/* gpio11   */ PlicInterrupt::defaultHandler,
		/* gpio12   */ PlicInterrupt::defaultHandler,
		/* gpio13   */ PlicInterrupt::defaultHandler,
		/* gpio14   */ PlicInterrupt::defaultHandler,
		/* gpio15   */ PlicInterrupt::defaultHandler,
		/* gpio16   */ PlicInterrupt::defaultHandler,
		/* gpio17   */ PlicInterrupt::defaultHandler,
		/* gpio18   */ PlicInterrupt::defaultHandler,
		/* gpio19   */ PlicInterrupt::defaultHandler,
		/* gpio20   */ PlicInterrupt::defaultHandler,
		/* gpio21   */ PlicInterrupt::defaultHandler,
		/* gpio22   */ PlicInterrupt::defaultHandler,
		/* gpio23   */ PlicInterrupt::defaultHandler,
		/* gpio24   */ PlicInterrupt::defaultHandler,
		/* gpio25   */ PlicInterrupt::defaultHandler,
		/* gpio26   */ PlicInterrupt::defaultHandler,
		/* gpio27   */ PlicInterrupt::defaultHandler,
		/* gpio28   */ PlicInterrupt::defaultHandler,
		/* gpio29   */ PlicInterrupt::defaultHandler,
		/* gpio30   */ PlicInterrupt::defaultHandler,
		/* gpio31   */ PlicInterrupt::defaultHandler,
		/* pwm0cmp0 */ PlicInterrupt::defaultHandler,
		/* pwm0cmp1 */ PlicInterrupt::defaultHandler,
		/* pwm0cmp2 */ PlicInterrupt::defaultHandler,
		/* pwm0cmp3 */ PlicInterrupt::defaultHandler,
		/* pwm1cmp0 */ PwmImp::pwm1cmp0handler,
		/* pwm1cmp1 */ PlicInterrupt::defaultHandler,
		/* pwm1cmp2 */ PlicInterrupt::defaultHandler,
		/* pwm1cmp3 */ PlicInterrupt::defaultHandler,
		/* pwm2cmp0 */ PlicInterrupt::defaultHandler,
		/* pwm2cmp1 */ PlicInterrupt::defaultHandler,
		/* pwm2cmp2 */ PlicInterrupt::defaultHandler,
	    /* pwm2cmp3 */ PlicInterrupt::defaultHandler
	};
};

Interrupt::~Interrupt()
{
	/* C++ demands that even a pure virtual destructor has an implementation */
}


