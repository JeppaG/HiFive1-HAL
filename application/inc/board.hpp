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
 * board.hpp
 *
 *  Created on: 23 Sep 2019
 *      Author: jeppa
 */

#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <cstdint>
#include "clock.hpp"
#include "interrupt.hpp"
#include "gpio.hpp"
#include "uart.hpp"
#include "timer.hpp"
#include "spi.hpp"

namespace board
{
	extern Clock* clock;

	extern Interrupt* plicInterrupt;
	extern Interrupt* uart0Interrupt;
	extern Interrupt* spi1Interrupt;

	extern Gpio* greenLedPin;
	extern Gpio* blueLedPin;
	extern Gpio* redLedPin;

	extern Uart* uart0;

	extern Pwm* pwm1;

	extern Spi* spi1;
};

#endif /* BOARD_HPP_ */
