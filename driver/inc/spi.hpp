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
 * uart.h
 *
 *  Created on: 20 Sep 2019
 *      Author: jeppa
 */

#ifndef SPI_HPP_
#define SPI_HPP_

#include <cstdint>
#include <gpio.hpp>

class Spi {
public:
	virtual ~Spi() =0;

	virtual void transceive ( uint8_t* const buffer, const uint8_t length ) =0;

	virtual bool transceiveComplete () =0;
};

class SpiImp : public Spi
{
public:
	typedef struct {
      volatile uint32_t sckDiv;                 /* Base address + 0x00 */
      volatile uint32_t sckMode;                /* Base address + 0x04 */
      volatile uint32_t reserved1;              /* Base address + 0x08 */
      volatile uint32_t reserved2;              /* Base address + 0x0C */
      volatile uint32_t chipSelectId;           /* Base address + 0x10 */
      volatile uint32_t chipSelectDefault;      /* Base address + 0x14 */
      volatile uint32_t chipSelectMode;         /* Base address + 0x18 */
      volatile uint32_t reserved3;              /* Base address + 0x1C */
      volatile uint32_t reserved4;              /* Base address + 0x20 */
      volatile uint32_t reserved5;              /* Base address + 0x24 */
      volatile uint32_t delay0;                 /* Base address + 0x28 */
      volatile uint32_t delay1;                 /* Base address + 0x2C */
      volatile uint32_t reserved6;              /* Base address + 0x30 */
      volatile uint32_t reserved7;              /* Base address + 0x34 */
      volatile uint32_t reserved8;              /* Base address + 0x38 */
      volatile uint32_t reserved9;              /* Base address + 0x3C */
      volatile uint32_t frameFormat;            /* Base address + 0x40 */
      volatile uint32_t reserved10;             /* Base address + 0x44 */
      volatile uint32_t txData;                 /* Base address + 0x48 */
      volatile uint32_t rxData;                 /* Base address + 0x4C */
      volatile uint32_t txWaterMark;            /* Base address + 0x50 */
      volatile uint32_t rxWaterMark;            /* Base address + 0x54 */
      volatile uint32_t reserved11;             /* Base address + 0x58 */
      volatile uint32_t reserved12;             /* Base address + 0x5C */
      volatile uint32_t flashInterfaceCtl;      /* Base address + 0x60 */
      volatile uint32_t flashInstructionFormat; /* Base address + 0x64 */
      volatile uint32_t reserved13;             /* Base address + 0x68 */
      volatile uint32_t reserved14;             /* Base address + 0x6C */
      volatile uint32_t interruptEnable;        /* Base address + 0x70 */
      volatile uint32_t interruptPending;       /* Base address + 0x74 */
	} spiRegisterType;

	static spiRegisterType* const spi1;

	SpiImp( spiRegisterType* const selectedSpi,
		     const uint32_t        selectedClockRate,
			 Gpio* const           selectedSckPin,
			 Gpio* const           selectedMosiPin,
			 Gpio* const           selectedMisoPin,
			 Gpio* const           selectedCsPin );

	virtual ~SpiImp();

	void transceive ( uint8_t* const buffer, const uint8_t length );

	bool transceiveComplete ();

	/* Interrupt handlers are declared "public" so that they may be included in the interrupt vector table */
	static void spi1InterruptHandler();

private:
	static constexpr int bufferSize = 32; /* Max 32 bytes can be stored in internal buffers */

	typedef struct {
		bool     complete;
		uint8_t  bytesRemaining;
		uint8_t* txPointer;
		uint8_t* rxPointer;
	} transactionType;

	spiRegisterType* const spiRegister;
	Gpio* const            sckPin;
	Gpio* const            mosiPin;
	Gpio* const     	   misoPin;
	Gpio* const            csPin;
	transactionType*       transaction;

	/* The transaction data and spiRegisters needs to be accessed by the interrupt handlers,
	 * and therefore they have to be declared static.
	 */
	/* The transaction data and the CS Pin needs to be accessed in the interrupt handler,
	 * and therefore needs to be copied to a static member variables */
	static transactionType        spi1Transaction;
	static Gpio*                  spi1CsPin;

	uint32_t getClockRateDiv ( const uint32_t tlClockInHz, const uint32_t ui32ClockRateInHz );

	void enableTxInterrupt();

	void disableTxInterrupt();

	void enableRxInterrupt();

	void disableRxInterrupt();

	void flushTxBuffer();

	void flushRxBuffer();

	void copyToTxBuffer( const uint8_t* buffer, const uint8_t length );

	uint8_t copyFromRxBuffer ( uint8_t* buffer );

	bool transmitAndReturnSuccess( const uint8_t txData );

	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/
	/* Set the pointers to the spi registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t spi1BaseAddress = 0x10024000u;

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

    static constexpr uint32_t interruptEnable = 0x00000000u;
                                             /* 0000 0000 0000 0000 0000 0000 0000 0000
                                              *                                      ||
                                              *                                      |+-Tx Water-mark 0 -> disabled
                                              *                                      +--Rx Water-mark 0 -> disabled
                                              */

    static constexpr uint32_t txInterruptEnableBit  = 0x00000001;
	static constexpr uint32_t rxInterruptEnableBit  = 0x00000002;
	static constexpr uint32_t txInterruptPendingBit = 0x00000001;
	static constexpr uint32_t rxInterruptPendingBit = 0x00000002;
};

#endif /* SPI_HPP_ */
