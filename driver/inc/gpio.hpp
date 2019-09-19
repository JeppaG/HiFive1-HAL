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
 * gpio.hpp
 *
 *  Created on: 22 Aug 2019
 *      Author: jeppa
 */

#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <cstdint>

class Gpio
{
public:
    virtual	void setAsOutput () =0;

	virtual void setAsInput () =0;

	virtual void setAsIoFunction0 () =0;

	virtual void setAsIoFunction1 () =0;

	virtual void vEnableRiseIrq () =0;

	virtual void vDisableRiseIrq () =0;

	virtual void set () =0;

	virtual void clear () =0;

	virtual ~Gpio() =0;
};
class GpioImp : public Gpio
{
public:

	static constexpr uint32_t pin0  = 0x00000001;
	static constexpr uint32_t pin1  = 0x00000002;
	static constexpr uint32_t pin2  = 0x00000004;
	static constexpr uint32_t pin3  = 0x00000008;
	static constexpr uint32_t pin4  = 0x00000010;
	static constexpr uint32_t pin5  = 0x00000020;
	static constexpr uint32_t pin6  = 0x00000040;
	static constexpr uint32_t pin7  = 0x00000080;
	static constexpr uint32_t pin8  = 0x00000100;
	static constexpr uint32_t pin9  = 0x00000200;
	static constexpr uint32_t pin10 = 0x00000400;
	static constexpr uint32_t pin11 = 0x00000800;
	static constexpr uint32_t pin12 = 0x00001000;
	static constexpr uint32_t pin13 = 0x00002000;
	static constexpr uint32_t pin14 = 0x00004000;
	static constexpr uint32_t pin15 = 0x00008000;
	static constexpr uint32_t pin16 = 0x00010000;
	static constexpr uint32_t pin17 = 0x00020000;
	static constexpr uint32_t pin18 = 0x00040000;
	static constexpr uint32_t pin19 = 0x00080000;
	static constexpr uint32_t pin20 = 0x00100000;
	static constexpr uint32_t pin21 = 0x00200000;
	static constexpr uint32_t pin22 = 0x00400000;
	static constexpr uint32_t pin23 = 0x00800000;
	static constexpr uint32_t pin24 = 0x01000000;
	static constexpr uint32_t pin25 = 0x02000000;
	static constexpr uint32_t pin26 = 0x04000000;
	static constexpr uint32_t pin27 = 0x08000000;
	static constexpr uint32_t pin28 = 0x10000000;
	static constexpr uint32_t pin29 = 0x20000000;
	static constexpr uint32_t pin30 = 0x40000000;
	static constexpr uint32_t pin31 = 0x80000000;

	typedef struct {
      volatile uint32_t value;            /* Base address + 0  */
      volatile uint32_t inputEnable;      /* Base address + 4  */
      volatile uint32_t outputEnable;     /* Base address + 8  */
      volatile uint32_t port;             /* Base address + 12 */
      volatile uint32_t pullUpEnable;     /* Base address + 16 */
      volatile uint32_t driveStrength;    /* Base address + 20 */
      volatile uint32_t riseIrqEnable;    /* Base address + 24 */
      volatile uint32_t riseIrqPending;   /* Base address + 28 */
      volatile uint32_t fallIrqEnable;    /* Base address + 32 */
      volatile uint32_t fallIrqPending;   /* Base address + 36 */
      volatile uint32_t highIrqEnable;    /* Base address + 40 */
      volatile uint32_t highIrqPending;   /* Base address + 44 */
      volatile uint32_t lowIrqEnable;     /* Base address + 48 */
      volatile uint32_t lowIrqPending;    /* Base address + 52 */
      volatile uint32_t ioFunctionEnable; /* Base address + 56 */
      volatile uint32_t ioFunctionSelect; /* Base address + 60 */
      volatile uint32_t outputXor;        /* Bade address + 64 */

	} gpioRegisterType;

	GpioImp( gpioRegisterType* const gpio, const uint32_t gpioPin );

	virtual ~GpioImp();

	void setAsOutput ();

	void setAsInput ();

	void setAsIoFunction0 ();

	void setAsIoFunction1 ();

	void vEnableRiseIrq ();

	void vDisableRiseIrq ();

	void set ();

	void clear ();

	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/

	/* GPIO registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t gpio0BaseAddress = 0x10012000u;

    static gpioRegisterType* const gpio0;

private:
    gpioRegisterType* const gpioRegister;
    const uint32_t pin;
};

#endif /* GPIO_HPP_ */
