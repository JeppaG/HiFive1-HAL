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
 * uart.cpp
 *
 *  Created on: 12 Aug 2019
 *      Author: jeppa
 */

#include "uart.hpp"
#include "hwRegisterOperations.hpp"
#include <cstring>

extern "C"
{
/* Prototypes for functions implemented in fifoOperations.S */
	extern bool _enqueTxDataFromRingBuffer(uint8_t*           bufferHead,
                                           uint8_t            bufferSize,
         						           uint8_t*           readPointer,
                                           uint8_t            writePointer,
                                           volatile uint32_t* txFifoPtr );

	extern void _dequeRxDataToRingBuffer( uint8_t*            bufferHead,
	                                      uint8_t             bufferSize,
	              				          uint8_t             readPointer,
	                                      uint8_t*            writePointer,
	                                      volatile uint32_t*  rxFifoPtr );
}

UartImp::dataType UartImp::uart0Data = {
		                       	  /* txBuffer */      {0},
								  /* rxBuffer */      {0},
                                 };

UartImp::uartRegisterType* const UartImp::uart0Register = reinterpret_cast<UartImp::uartRegisterType* const>( UartImp::uart0BaseAddress );

UartImp::UartImp( uartRegisterType* const selectedUart,
                  const uint32_t    selectedBaudRate,
				  Gpio* const       selectedTxPin,
				  Gpio* const       selectedRxPin ) :
    uartRegister( selectedUart ),
	txPin( selectedTxPin ),
	rxPin( selectedRxPin )
{
	uartRegister->baudRateDiv = getBaudRateDiv ( 256000000u, selectedBaudRate );
	if ( uart0Register == selectedUart )
	{
		data =  &uart0Data;
		txPin->setAsIoFunction0(); /* UART0 TX */
		rxPin->setAsIoFunction0(); /* UART0 RX */
	}
	else
	{
		/* Should not be possible to end up here */
		while ( 1 );
	}

	disableTx();
	disableRx();
	disableTxInterrupt();
	disableRxInterrupt();
	hwRegOps::setRegister( uartRegister->txCtrl, waterMarkLevelMask, waterMarkLevel1 );
	hwRegOps::setRegister( uartRegister->rxCtrl, waterMarkLevelMask, waterMarkLevel6 );

	data->txBuffer.readPointer = 0;
	data->txBuffer.writePointer = 0;
	data->rxBuffer.readPointer = 0;
	data->rxBuffer.writePointer = 0;
}

UartImp::~UartImp()
{
	data = nullptr;
}

void UartImp::enableTx()
{
	flushTxBuffer();
	hwRegOps::setBits( uartRegister->txCtrl, txEnableBit );
}

void UartImp::enableRx()
{
	flushRxBuffer();
	enableRxInterrupt();
	hwRegOps::setBits( uartRegister->rxCtrl, rxEnableBit );
}

void UartImp::disableTx()
{
	hwRegOps::clearBits( uartRegister->txCtrl, txEnableBit );
	disableTxInterrupt();
}

void UartImp::disableRx()
{
	hwRegOps::clearBits( uartRegister->rxCtrl, rxEnableBit );
	disableTxInterrupt();
}

void UartImp::transmit ( const uint8_t* buffer, const uint8_t length )
{
	copyToTxBuffer( buffer, length );
    enableTxInterrupt();
}

uint8_t UartImp::receive ( uint8_t* buffer )
{
	uint32_t length = 0;
	/* Watermark level 0 will trigger an interrupt if any bytes at all are in the rx FIFO, so that
	 * all received bytes becomes available in the receive buffer
	 */
	hwRegOps::setRegister( uartRegister->rxCtrl, waterMarkLevelMask, waterMarkLevel0 );
	hwRegOps::setRegister( uartRegister->rxCtrl, waterMarkLevelMask, waterMarkLevel6 );

	return copyFromRxBuffer( buffer );
}


uint32_t UartImp::getBaudRateDiv ( const uint32_t tlClockInHz, const uint32_t baudRateInHz )
{
	/* Add 1/2 LSB in the quotient to achieve rounding instead o truncation
	 * The dividend is defined as ( tlClock/baudRate ) - 1 in the FE310-G000 manual
	 * therefore one LSB needs to be subtracted in the quotient, so including rounding,
	 * take ( tlClock - ( baudRate/2 ) )/baudRate */
	uint32_t div = tlClockInHz - ( baudRateInHz / 2 );
	return div/baudRateInHz;
}

void UartImp::enableTxInterrupt()
{
	hwRegOps::setBits( uartRegister->interruptEnable, txInterruptEnableBit );
}

void UartImp::disableTxInterrupt()
{
	hwRegOps::clearBits( uartRegister->interruptEnable, txInterruptEnableBit );
}

void UartImp::enableRxInterrupt()
{
	hwRegOps::setBits( uartRegister->interruptEnable, rxInterruptEnableBit );
}

void UartImp::disableRxInterrupt()
{
	hwRegOps::clearBits( uartRegister->interruptEnable, rxInterruptEnableBit );
}

void UartImp::flushTxBuffer()
{
	data->txBuffer.readPointer = data->txBuffer.writePointer;
}

void UartImp::flushRxBuffer()
{
	data->rxBuffer.writePointer = data->rxBuffer.readPointer;
}

void UartImp::copyToTxBuffer( const uint8_t* buffer, uint8_t length )
{
	uint8_t nextWritePointer = data->txBuffer.writePointer + 1;
	while ( length > 0 )
	{
		if ( nextWritePointer >= bufferSize )
		{
			nextWritePointer = 0;
		}
		if ( nextWritePointer == data->txBuffer.readPointer )
		{
			break;
		}
		data->txBuffer.buffer[data->txBuffer.writePointer] = *buffer;
		data->txBuffer.writePointer = nextWritePointer;
		nextWritePointer++;
		buffer++;
		length--;
	}
}

uint8_t UartImp::copyFromRxBuffer ( uint8_t* buffer )
{
	uint8_t length = 0;
	while ( data->rxBuffer.readPointer != data->rxBuffer.writePointer )
	{
		*buffer = data->rxBuffer.buffer[data->rxBuffer.readPointer];
		buffer++;
		length++;
		data->rxBuffer.readPointer++;
		if ( data->rxBuffer.readPointer >= bufferSize )
		{
			data->rxBuffer.readPointer = 0;
		}
	}

	return length;
}

/* The functions for uart fifo operations are implemented in the file fifoOperations.S
 * and have the following prototypes:
 *
 * bool _enqueTxDataFromRingBuffer( uint8_t*           bufferHead,
 *                                  uint8_t            bufferSize,
 *  					            uint8_t*           readPointer,
 *                                  uint8_t            writePointer,
 *                                  volatile uint32_t* txFifoPtr );
 */
//bool UartImp::enqueTxData( bufferType* txBuffer, volatile uint32_t* uartRegisterTxData )
//{
//	uint8_t* buffer = &txBuffer->buffer[0];
//	uint32_t bytesLeft = 0;
//	asm volatile( "1: beq %0, %2,  2f;" /* End when read and write pointers are equal */
//			      "   add t0, %3, %0;"  /* Set read address in t0 as buffer address + read pointer */
//			      "   lb %1, 0(t0);"    /* Read the next byte to transmit into bytesLeft */
//			      "   amoswap.w %1, %1, (%5);" /* Enque in txFifo and read out success status */
//			      "   bne %1, zero, 2f;"/* If no more bytes could be enqued, quit */
//				  "   addi %0, %0, 1;"  /* Increment the read pointer */
//			      "   blt %0, %4, 1b;"  /* Check if the end of the buffer is reached */
//			      "   add %0, zero, zero;" /* When the end of the buffer is reached, restart at 0 */
//				  "	  beq zero, zero, 1b;"
//			      "2:;"
//		 : /* Output Operands */ "+r" ( txBuffer->readPointer ), /* %0 */
//		                         "+r" ( bytesLeft )              /* %1 */
//		 : /* Input Operands */  "r" ( txBuffer->writePointer ), /* %2 */
//			                     "r" ( buffer ),                 /* %3 */
//								 "r" ( bufferSize ),             /* %4 */
//		                         "r" ( uartRegisterTxData )      /* %5 */
//		 : /* Clobbered Registers */ "t0"
//    );
//
//    return ( 0 == bytesLeft );
//}

/* void _dequeRxDataToRingBuffer( uint8_t*            bufferHead,
 *                                uint8_t             bufferSize,
 *              				  uint8_t             readPointer,
 *                                uint8_t*            writePointer,
 *                                volatile uint32_t*  rxFifoPtr );
 */
//void UartImp::dequeRxData ( bufferType* rxBuffer, volatile uint32_t* uartRegisterRxData )
//{
//	uint8_t* buffer = &rxBuffer->buffer[0];
//	asm volatile( "1: addi t0, %0, 1;"  /* t0 is the next write pointer */
//			      "   blt t0, %3, 2f;"  /* Check if the next write pointer is outside the buffer */
//			      "   add t0, zero, zero;" /* If the next write pointer is outside the buffer, restart at 0 */
//			      "2: lw t1, (%4);"      /* Deque the next Rx byte to t1 */
//			      "   blt t1, zero, 3f;"  /* Check if the Rx FIFO was empty, in that case we are done */
//			      "   add t2, %2, %0;"  /* Set write address in t2 as buffer address + write pointer */
//			      "   sb t1, (t2);"     /* Write the dequed byte to the Rx buffer */
//			      "   beq t0, %1, 1b;"  /* If the next write pointer equals the read-pointer, the buffer is full... */
//			      "   add %0, t0, zero;"/* ...otherwise set the write pointer to the next write pointer */
//				  "   beq zero, zero, 1b;"
//			      "3:;"
//		 : /* Output Operands */ "+r" ( rxBuffer->writePointer ) /* %0 */
//		 : /* Input Operands */  "r" ( rxBuffer->readPointer ),  /* %1 */
//			                     "r" ( buffer ),                 /* %2 */
//								 "r" ( bufferSize ),             /* %3 */
//		                         "r" ( uartRegisterRxData )      /* %4 */
//		 : /* Clobbered Registers */ "t0", "t1", "t2"
//    );
//
//}

/* Interrupt handlers */

void UartImp::uart0InterruptHandler()
{
    if ( true == hwRegOps::compareBits( uart0Register->interuptPending, txInterruptPendingBit ) )
    {
//		bool txComplete = enqueTxData ( &uart0Data.txBuffer, &uart0Register->txData );
    	bool moreDataToSend = _enqueTxDataFromRingBuffer( uart0Data.txBuffer.buffer,
    			                                          bufferSize,
    			   					                      &uart0Data.txBuffer.readPointer,
    			                                          uart0Data.txBuffer.writePointer,
    			                                          &uart0Register->txData );
		if ( false == moreDataToSend )
		{
			hwRegOps::clearBits( uart0Register->interruptEnable, txInterruptEnableBit );
		}
    }

    if ( true == hwRegOps::compareBits( uart0Register->interuptPending, rxInterruptPendingBit ) )
    {
    	_dequeRxDataToRingBuffer( uart0Data.rxBuffer.buffer,
    	                          bufferSize,
    	              			  uart0Data.rxBuffer.readPointer,
    	                          &uart0Data.rxBuffer.writePointer,
    	                          &uart0Register->rxData );
    }
}

Uart::~Uart()
{
	/* C++ demands that even a pure virtual destructor has an implementation */
}
