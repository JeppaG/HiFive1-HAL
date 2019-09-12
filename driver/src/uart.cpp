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

Uart uart0Object( Uart::uart0, 115200u );

Uart* uart0 = &uart0Object;

Uart::dataType Uart::uart0Data = {
		                       	  /* txBuffer */      {0},
								  /* rxBuffer */      {0},
                                 };

Uart::uartRegisterType* const Uart::uart0Register = reinterpret_cast<Uart::uartRegisterType*>( uart0 );

Uart::Uart( const baseAddress selectedUart,
            const uint32_t    selectedBaudRate ) :
  uartRegister( reinterpret_cast<uartRegisterType*>( selectedUart ) )
{
	uartRegister->baudRateDiv = getBaudRateDiv ( 256000000u, selectedBaudRate );
	switch ( selectedUart )
	{
	case uart0:
		data =  &uart0Data;
		break;
	default:
		/* Should not be possible to end up here */
		break;
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

Uart::~Uart()
{
	data = nullptr;
}

void Uart::enableTx()
{
	flushTxBuffer();
	hwRegOps::setBits( uartRegister->txCtrl, txEnableBit );
}

void Uart::enableRx()
{
	flushRxBuffer();
	enableRxInterrupt();
	hwRegOps::setBits( uartRegister->rxCtrl, rxEnableBit );
}

void Uart::disableTx()
{
	hwRegOps::clearBits( uartRegister->txCtrl, txEnableBit );
	disableTxInterrupt();
}

void Uart::disableRx()
{
	hwRegOps::clearBits( uartRegister->rxCtrl, rxEnableBit );
	disableTxInterrupt();
}

void Uart::transmit ( const uint8_t* buffer, const uint8_t length )
{
	copyToTxBuffer( buffer, length );
    enableTxInterrupt();
}

uint8_t Uart::receive ( uint8_t* buffer )
{
	uint32_t length = 0;
	/* Watermark level 0 will trigger an interrupt if any bytes at all are in the rx FIFO, so that
	 * all received bytes becomes available in the receive buffer
	 */
	hwRegOps::setRegister( uartRegister->rxCtrl, waterMarkLevelMask, waterMarkLevel0 );
	hwRegOps::setRegister( uartRegister->rxCtrl, waterMarkLevelMask, waterMarkLevel6 );

	return copyFromRxBuffer( buffer );
}


uint32_t Uart::getBaudRateDiv ( const uint32_t tlClockInHz, const uint32_t baudRateInHz )
{
	/* Add 1/2 LSB in the quotient to achieve rounding instead o truncation
	 * The dividend is defined as ( tlClock/baudRate ) - 1 in the FE310-G000 manual
	 * therefore one LSB needs to be subtracted in the quotient, so including rounding,
	 * take ( tlClock - ( baudRate/2 ) )/baudRate */
	uint32_t div = tlClockInHz - ( baudRateInHz / 2 );
	return div/baudRateInHz;
}

void Uart::enableTxInterrupt()
{
	hwRegOps::setBits( uartRegister->interruptEnable, txInterruptEnableBit );
}

void Uart::disableTxInterrupt()
{
	hwRegOps::clearBits( uartRegister->interruptEnable, txInterruptEnableBit );
}

void Uart::enableRxInterrupt()
{
	hwRegOps::setBits( uartRegister->interruptEnable, rxInterruptEnableBit );
}

void Uart::disableRxInterrupt()
{
	hwRegOps::clearBits( uartRegister->interruptEnable, rxInterruptEnableBit );
}

void Uart::flushTxBuffer()
{
	data->txBuffer.readPointer = data->txBuffer.writePointer;
}

void Uart::flushRxBuffer()
{
	data->rxBuffer.writePointer = data->rxBuffer.readPointer;
}

void Uart::copyToTxBuffer( const uint8_t* buffer, uint8_t length )
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

uint8_t Uart::copyFromRxBuffer ( uint8_t* buffer )
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
bool Uart::enqueTxData( bufferType* txBuffer, volatile uint32_t* uartRegisterTxData )
{
	uint8_t* buffer = &txBuffer->buffer[0];
	uint32_t bytesLeft = 0;
	asm volatile( "1: beq %0, %2,  2f;" /* End when read and write pointers are equal */
			      "   add t0, %3, %0;"  /* Set read address in t0 as buffer address + read pointer */
			      "   lb %1, 0(t0);"    /* Read the next byte to transmit into bytesLeft */
			      "   amoswap.w %1, %1, (%5);" /* Enque in txFifo and read out success status */
			      "   bne %1, zero, 2f;"/* If no more bytes could be enqued, quit */
				  "   addi %0, %0, 1;"  /* Increment the read pointer */
			      "   blt %0, %4, 1b;"  /* Check if the end of the buffer is reached */
			      "   add %0, zero, zero;" /* When the end of the buffer is reached, restart at 0 */
				  "	  beq zero, zero, 1b;"
			      "2:;"
		 : /* Output Operands */ "+r" ( txBuffer->readPointer ), /* %0 */
		                         "+r" ( bytesLeft )              /* %1 */
		 : /* Input Operands */  "r" ( txBuffer->writePointer ), /* %2 */
			                     "r" ( buffer ),                 /* %3 */
								 "r" ( bufferSize ),             /* %4 */
		                         "r" ( uartRegisterTxData )      /* %5 */
		 : /* Clobbered Registers */ "t0"
    );

    return ( 0 == bytesLeft );
}

void Uart::dequeRxData ( bufferType* rxBuffer, volatile uint32_t* uartRegisterRxData )
{
	uint8_t* buffer = &rxBuffer->buffer[0];
	asm volatile( "1: addi t0, %0, 1;"  /* t0 is the next write pointer */
			      "   blt t0, %3, 2f;"  /* Check if the next write pointer is outside the buffer */
			      "   add t0, zero, zero;" /* If the next write pointer is outside the buffer, restart at 0 */
			      "2: lw t1, (%4);"      /* Deque the next Rx byte to t1 */
			      "   blt t1, zero, 3f;"  /* Check if the Rx FIFO was empty, in that case we are done */
			      "   add t2, %2, %0;"  /* Set write address in t2 as buffer address + write pointer */
			      "   sb t1, (t2);"     /* Write the dequed byte to the Rx buffer */
			      "   beq t0, %1, 1b;"  /* If the next write pointer equals the read-pointer, the buffer is full... */
			      "   add %0, t0, zero;"/* ...otherwise set the write pointer to the next write pointer */
				  "   beq zero, zero, 1b;"
			      "3:;"
		 : /* Output Operands */ "+r" ( rxBuffer->writePointer ) /* %0 */
		 : /* Input Operands */  "r" ( rxBuffer->readPointer ),  /* %1 */
			                     "r" ( buffer ),                 /* %2 */
								 "r" ( bufferSize ),             /* %3 */
		                         "r" ( uartRegisterRxData )      /* %4 */
		 : /* Clobbered Registers */ "t0", "t1", "t2"
    );

}

/* Interrupt handlers */

void Uart::uart0InterruptHandler()
{
    if ( true == hwRegOps::compareBits( uart0Register->interuptPending, txInterruptPendingBit ) )
    {
		bool txComplete = enqueTxData ( &uart0Data.txBuffer, &uart0Register->txData );
		if ( true == txComplete )
		{
			hwRegOps::clearBits( uart0Register->interruptEnable, txInterruptEnableBit );
		}
    }

    if ( true == hwRegOps::compareBits( uart0Register->interuptPending, rxInterruptPendingBit ) )
    {
    	dequeRxData ( &uart0Data.rxBuffer, &uart0Register->rxData );
    }
}

