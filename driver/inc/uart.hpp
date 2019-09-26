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
 *  Created on: 04 Sep 2019
 *      Author: jeppa
 */

#ifndef UART_HPP_
#define UART_HPP_

#include <cstdint>
#include "gpio.hpp"

class Uart {
public:
	virtual ~Uart() =0;

	virtual void enableTx() =0;

	virtual void enableRx() =0;

	virtual void disableTx() =0;

	virtual void disableRx() =0;

	virtual void transmit ( const uint8_t* buffer, const uint8_t length ) =0;

	virtual uint8_t receive ( uint8_t* buffer ) =0;
};

class UartImp : public Uart
{
public:
	typedef struct {
      volatile       uint32_t txData;          /* Base address + 0  */
      volatile       uint32_t rxData;          /* Base address + 4  */
      volatile       uint32_t txCtrl;          /* Base address + 8  */
      volatile       uint32_t rxCtrl;          /* Base address + 12 */
      volatile       uint32_t interruptEnable; /* Base address + 16 */
      volatile       uint32_t interuptPending; /* Base address + 20 */
      volatile       uint32_t baudRateDiv;     /* Base address + 24 */
	} uartRegisterType;

	static uartRegisterType* const uart0Register;

	UartImp( uartRegisterType* const selectedUart,
		     const uint32_t    selectedBaudRate,
			 Gpio* const       selectedTxPin,
			 Gpio* const       selectedRxPin );

	virtual ~UartImp();

	void enableTx();

	void enableRx();

	void disableTx();

	void disableRx();

	void transmit ( const uint8_t* buffer, const uint8_t length );

	uint8_t receive ( uint8_t* buffer );

	/* Interrupt handlers are declared "public" so that they may be included in the interrupt vector table */
	static void uart0InterruptHandler();

private:
	static constexpr int bufferSize = 32; /* Max 32 bytes can be stored in internal buffers */

	typedef struct {
		uint8_t	buffer[bufferSize];
		uint8_t	writePointer;
		uint8_t readPointer;
	} bufferType;

	typedef struct {
		bufferType    txBuffer;
		bufferType    rxBuffer;
	} dataType;

	uartRegisterType* const uartRegister;
	dataType*               data;
	Gpio* const             txPin;
	Gpio* const             rxPin;

	/* The UART buffers needs to be accessed by the interrupt handlers, and therefore they have to
	 * be declared static.
	 */
	static dataType  uart0Data;

	uint32_t getBaudRateDiv ( const uint32_t tlClockInHz, const uint32_t ui32BaudRateInHz );

	void enableTxInterrupt();

	void disableTxInterrupt();

	void enableRxInterrupt();

	void disableRxInterrupt();

	void flushTxBuffer();

	void flushRxBuffer();

	void copyToTxBuffer( const uint8_t* buffer, const uint8_t length );

	uint8_t copyFromRxBuffer ( uint8_t* buffer );

	static bool enqueTxData ( bufferType* txBuffer, volatile uint32_t* uartRegisterTxData );

	static void dequeRxData ( bufferType* rxBuffer, volatile uint32_t* uartRegisterRxData );

	bool transmitAndReturnSuccess( const uint8_t txData );

	/************************************************************************
	 *  Compile-time configuration parameters
	 ************************************************************************/
	/* Set the pointers to the uart registers base addresses to the addresses indicated in the FE310-G000 chip manual */
	static constexpr uint32_t uart0BaseAddress = 0x10013000u;

	static constexpr uint32_t txConfig = 0x0000000u;
	                                  /* 0000 0000 0000 0000 0000 0000 0000 0000
				                       *                 |||                  ||
				                       *                 |||                  |+ TX enable 0 -> Not enabled
				                       *                 |||                  +--number of stop bits 0 -> 1 stop-bit, 1 -> 2 stop-bits
				                       *                 +++---------------------Transmit watermark level, 0-7
	                                   */

	static constexpr uint32_t rxConfig = 0x0000000u;
	                                  /* 0000 0000 0000 0000 0000 0000 0000 0000
				                       *                 |||                   |
				                       *                 |||                   + RX enable 0 -> Not enabled
				                       *                 +++---------------------Receive watermark level, 0-7
	                                   */

	static constexpr uint32_t txEnableBit           = 0x00000001;
	static constexpr uint32_t rxEnableBit           = 0x00000001;
	static constexpr uint32_t txInterruptEnableBit  = 0x00000001;
	static constexpr uint32_t rxInterruptEnableBit  = 0x00000002;
	static constexpr uint32_t txInterruptPendingBit = 0x00000001;
	static constexpr uint32_t rxInterruptPendingBit = 0x00000002;
	static constexpr uint32_t waterMarkLevel0       = 0x00000000;
	static constexpr uint32_t waterMarkLevel1       = 0x00010000;
	static constexpr uint32_t waterMarkLevel6       = 0x00060000;
	static constexpr uint32_t waterMarkLevel7       = 0x00070000;
	static constexpr uint32_t waterMarkLevelMask    = 0x00070000;
};

#endif /* UART_HPP_ */
