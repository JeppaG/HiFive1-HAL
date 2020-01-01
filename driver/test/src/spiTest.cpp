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
 * spiTest.cpp
 *
 *  Created on: 29 Sep 2019
 *      Author: jeppa
 */

#include "../../inc/spi.hpp"

#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include <cstdint>
#include <cstring>

#include "../../mock/inc/gpioMock.hpp"

constexpr uint32_t tlClockInHz = 256000000;

static SpiImp::spiRegisterType spi1Register;

/* Set the pointers to the spi registers base addresses to the addresses of the mock spiRegisters declared above */
SpiImp::spiRegisterType* const SpiImp::spi1 = &spi1Register;


int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}

TEST_GROUP(SpiTest)
{
	Gpio*                    sckPin;
	Gpio*                    misoPin;
	Gpio*                    mosiPin;
	Gpio*                    csPin;

	void setup()
	{
		sckPin = dynamic_cast<Gpio*>( new GpioMock );
		mosiPin = dynamic_cast<Gpio*>( new GpioMock );
		misoPin = dynamic_cast<Gpio*>( new GpioMock );
		csPin = dynamic_cast<Gpio*>( new GpioMock );

        memset( &spi1Register, 0, sizeof( SpiImp::spiRegisterType ) );
	}

	void teardown()
	{
		delete sckPin;
		delete mosiPin;
		delete misoPin;
		delete csPin;
		mock().checkExpectations();
		mock().clear();
	}

	uint32_t calculateSpiSckDiv( const uint32_t requestedSpiClockInHz )
	{
		/* Add 1/2 LSB in the quotient to achieve rounding instead o truncation
		 * The dividend is defined as tlClock/( 2*spiClock ) - 1 in the FE310-G000 manual
		 * therefore one LSB needs to be subtracted in the quotient, so including rounding,
		 * take ( tlClock - spiClock )/( 2*spiClock ) */
		uint32_t div = tlClockInHz - requestedSpiClockInHz;
		return div/( 2*requestedSpiClockInHz );

	}

	static constexpr uint32_t clockMode = 0x00000002u;
	                                    /* 0000 0000 0000 0000 0000 0000 0000 0010
	                                     *                                      ||
	                                     *                                      |+-Phase 0 -> Data is shifted on the trailing- and sampled on the leading edge of sck
	                                     *                                      +--Polarity 1 -> sck is active low
	                                     */

	/* There doesn't seem to be any pins for HW Chip Select available for SPI1. Still the registers
	 * are included for completeness. */
	static constexpr uint32_t chipSelectId = 0x00000000u; /* Id of the selected gpio for HW Chip Select control */
	static constexpr uint32_t chipSelectDefault = 0x00000000u; /* Inactive state for HW CS pins */
	static constexpr uint32_t chipSelectMode = 0x00000003u;
                                             /* 0000 0000 0000 0000 0000 0000 0000 0011
                                              *                                      ||
                                              *                                      ++-Mode. 3-> Disable HW control of the CS pin
                                              */
	static constexpr uint32_t delay0 = 0x00000000u;
                                    /* 0000 0000 0000 0000 0000 0000 0000 0000
                                     *           |||| ||||           |||| ||||
                                     *           |||| ||||           ++++-++++- Delay from CS to first leading edge of sck in sck cycles
                                     *           ++++-++++--------------------- Delay from last trailing edge of sck to de-assettion of CS
                                     */

	static constexpr uint32_t delay1 = 0x00000000u;
                                    /* 0000 0000 0000 0000 0000 0000 0000 0000
                                     *           |||| ||||           |||| ||||
                                     *           |||| ||||           ++++-++++- Minimum CS inactive time
                                     *           ++++-++++--------------------- Max inter-frame delay without de-asserting CS
                                     */

	static constexpr uint32_t frameFormat = 0x00080000u;
                                         /* 0000 0000 0000 1000 0000 0000 0000 0000
                                          *                ||||                ||||
                                          *                ||||                ||++-Protocol 0 -> Single (MISO + MOSI)
                                          *                ||||                |+---Endian 0 -> Tx/Rx MSB first
                                          *                ||||                +----Direction 0 -> Rx (+ Tx when single protocol)
                                          *                ++++---------------------Number of bits per frame, 8
                                          */

    static constexpr uint32_t rxInterruptEnable = 0x00000002u;
                                             	 /* 0000 0000 0000 0000 0000 0000 0000 0010
                                             	  *                                      ||
                                             	  *                                      |+-Tx Water-mark 0 -> disabled
                                             	  *                                      +--Rx Water-mark 1 -> enabled
                                             	  */

    static constexpr uint32_t rxAndTxInterruptEnable = 0x00000003u;
                                             	      /* 0000 0000 0000 0000 0000 0000 0000 0011
                                             	       *                                      ||
                                             	       *                                      |+-Tx Water-mark 0 -> disabled
                                             	       *                                      +--Rx Water-mark 1 -> enabled
                                             	       */
};

/*! When SPI-1 is initialized
 *  - The selected pins for sck, miso and mosi shall be set to IoFunction0
 *  - The selected cs-pin shall be set and set to Output.
 *  - Spi1 clock-division register shall be set so that the resulting
 *    clock rate becomes the requested SPI clock rate as described in the
 *    FE310-G000 manual
 */
TEST( SpiTest, InitSpi1 )
{
	mock().expectOneCall( "setAsIoFunction0" ).onObject( sckPin );
	mock().expectOneCall( "setAsIoFunction0" ).onObject( misoPin );
	mock().expectOneCall( "setAsIoFunction0" ).onObject( mosiPin );
	mock().expectOneCall( "set" ).onObject( csPin );
	mock().expectOneCall( "setAsOutput" ).onObject( csPin );


	SpiImp spi1( /* SPI instance */   SpiImp::spi1,
			     /* SCK rate in Hz */ 20000000u,
                 /* SCK pin */        sckPin,
                 /* MOSI Pin */       mosiPin,
                 /* MISO Pin */       misoPin,
                 /* CS Pin */         csPin );

	CHECK_EQUAL( calculateSpiSckDiv( 20000000 ), spi1Register.sckDiv );
	CHECK_EQUAL( clockMode, spi1Register.sckMode );
	CHECK_EQUAL( chipSelectId, spi1Register.chipSelectId );
	CHECK_EQUAL( chipSelectDefault, spi1Register.chipSelectDefault );
	CHECK_EQUAL( chipSelectMode, spi1Register.chipSelectMode );
	CHECK_EQUAL( delay0, spi1Register.delay0 );
	CHECK_EQUAL( delay1, spi1Register.delay1 );
	CHECK_EQUAL( frameFormat, spi1Register.frameFormat );
	CHECK_EQUAL( rxInterruptEnable, spi1Register.interruptEnable ); /* Only rx interruppt enabled at initialization */
	CHECK_EQUAL( 0, spi1Register.txWaterMark );
	CHECK_EQUAL( 7, spi1Register.rxWaterMark );

};
