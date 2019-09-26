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
 * board.cpp
 *
 *  Created on: 24 Sep 2019
 *      Author: jeppa
 */
#include <cstdint>
#include "board.hpp"

namespace boardImplementation
{
	ClockImp clockImp( ClockImp::crti );

	ClintInterrupt plicClintInterrupt ( ClintInterrupt::external );
	PlicInterrupt uart0PlicInterrupt( PlicInterrupt::uart0 );
	PlicInterrupt spi1PlicInterrupt( PlicInterrupt::qspiI1 );

	GpioImp uartRxPin  ( GpioImp::gpio0, GpioImp::pin16 );
	GpioImp uartTxPin  ( GpioImp::gpio0, GpioImp::pin17 );
	GpioImp spiMosiPin ( GpioImp::gpio0, GpioImp::pin3 );
	GpioImp spiMisoPin ( GpioImp::gpio0, GpioImp::pin4 );
	GpioImp spiSckPin  ( GpioImp::gpio0, GpioImp::pin5 );
	GpioImp spiCsPin   ( GpioImp::gpio0, GpioImp::pin2 );
	GpioImp greenLedPin( GpioImp::gpio0, GpioImp::pin19 );
	GpioImp blueLedPin ( GpioImp::gpio0, GpioImp::pin21 );
	GpioImp redLedPin  ( GpioImp::gpio0, GpioImp::pin22 );

	UartImp uart0( /* UART instance */   UartImp::uart0Register,
			       /* Baud-rate in Hz */ 115200u,
				   /* TX pin */          &uartTxPin,
				   /* RX pin */          &uartRxPin );

	PwmImp pwm1( PwmImp::pwm1 );

	SpiImp spi1( /* SPI instance */   SpiImp::spi1,
			     /* SCK rate in Hz */ 20000000u,
                 /* SCK pin */        &spiSckPin,
                 /* MOSI Pin */       &spiMosiPin,
                 /* MISO Pin */       &spiMisoPin,
                 /* CS Pin */         &spiCsPin );
}

namespace board
{
	Clock* clock = dynamic_cast<Clock*>( &boardImplementation::clockImp );

	Interrupt* plicInterrupt  = dynamic_cast<Interrupt*>( &boardImplementation::plicClintInterrupt );
	Interrupt* uart0Interrupt = dynamic_cast<Interrupt*>( &boardImplementation::uart0PlicInterrupt );
	Interrupt* spi1Interrupt  = dynamic_cast<Interrupt*>( &boardImplementation::spi1PlicInterrupt );

	Gpio* greenLedPin = dynamic_cast<Gpio*>( &boardImplementation::greenLedPin );
	Gpio* blueLedPin  = dynamic_cast<Gpio*>( &boardImplementation::blueLedPin );
	Gpio* redLedPin   = dynamic_cast<Gpio*>( &boardImplementation::redLedPin );

	Uart* uart0 = dynamic_cast<Uart*>( &boardImplementation::uart0 );

	Pwm* pwm1 = dynamic_cast<Pwm*>( &boardImplementation::pwm1 );

	Spi* spi1 = dynamic_cast<Spi*>( &boardImplementation::spi1 );
}
