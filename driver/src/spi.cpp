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
 * spi.cpp
 *
 *  Created on: 12 Aug 2019
 *      Author: jeppa
 */

#include "spi.hpp"
#include "hwRegisterOperations.hpp"
#include <cstring>

extern "C"
{
    /* Prototypes for functions implemented in fifoOperations.S */
    bool _enqueTxData( uint8_t**          txPointer,
                       uint8_t*           bytesRemaining,
                       volatile uint32_t* txFifoPtr );

    void _dequeRxData( uint8_t**           rxPointer,
                       volatile uint32_t*  rxFifoPtr );
}

SpiImp::transactionType SpiImp::spi1Transaction = {
		                          /* complete */       true,
		                       	  /* bytesRemaining */ 0,
								  /* txPointer */      nullptr,
								  /* rxPointer */      nullptr
                                 };

Gpio* SpiImp::spi1CsPin = nullptr;

SpiImp::spiRegisterType* const SpiImp::spi1 = reinterpret_cast<SpiImp::spiRegisterType* const>( SpiImp::spi1BaseAddress );

SpiImp::SpiImp( spiRegisterType* const selectedSpi,
                const uint32_t         selectedClockRate,
				Gpio* const            selectedSckPin,
				Gpio* const            selectedMosiPin,
				Gpio* const            selectedMisoPin,
				Gpio* const            selectedCsPin ) :
  spiRegister( selectedSpi ),
  sckPin( selectedSckPin ),
  mosiPin( selectedMosiPin ),
  misoPin( selectedMisoPin ),
  csPin( selectedCsPin )
{
	if ( spi1 == selectedSpi )
	{
		transaction =  &spi1Transaction;
		spi1CsPin = csPin;
		sckPin->setAsIoFunction0();  /* SPI1 SCK */
		mosiPin->setAsIoFunction0(); /* SPI1 MOSI */
		misoPin->setAsIoFunction0(); /* SPI1 MISO */
	}
	else
	{
		/* Should not be possible to end up here */
		while ( 1 );
	}

	csPin->set(); /* CS is defined as active low, so this sets it to idle */
	csPin->setAsOutput();
	spiRegister->sckDiv = getClockRateDiv ( 256000000u, selectedClockRate );
	spiRegister->sckMode = clockMode;
	spiRegister->chipSelectId = chipSelectId;
	spiRegister->chipSelectDefault = chipSelectDefault;
	spiRegister->chipSelectMode = chipSelectMode;
	spiRegister->delay0 = delay0;
	spiRegister->delay1 = delay1;
	spiRegister->frameFormat = frameFormat;
	spiRegister->txWaterMark = 0;
	spiRegister->rxWaterMark = 7;
	disableTxInterrupt();
	enableRxInterrupt();

	transaction->complete = true;
	transaction->bytesRemaining = 0;
	transaction->txPointer = nullptr;
	transaction->rxPointer = nullptr;
}

SpiImp::~SpiImp()
{
	transaction = nullptr;
}

void SpiImp::transceive ( uint8_t* const buffer, const uint8_t length )
{
	if ( ( true == transaction->complete ) && ( 0 < length ) )
	{
		transaction->complete = false;
		transaction->bytesRemaining = length;
	    transaction->txPointer = buffer;
	    transaction->rxPointer = buffer;
	    csPin->clear(); /* CS is active low */
	    _enqueTxData( &( transaction->txPointer ),
	                  &( transaction->bytesRemaining ),
	                  &( spiRegister->txData ) );
	    if ( 7 < length )
	    {
	    	spiRegister->rxWaterMark = 7;
	    }
	    else
	    {
	    	spiRegister->rxWaterMark = length - 1;
	    }
	}
}

bool SpiImp::transceiveComplete()
{
	return ( transaction->complete );
}

uint32_t SpiImp::getClockRateDiv ( const uint32_t tlClockInHz, const uint32_t clockRateInHz )
{
	/* Add 1/2 LSB in the quotient to achieve rounding instead o truncation
	 * The dividend is defined as tlClock/( 2*spiClock ) - 1 in the FE310-G000 manual
	 * therefore one LSB needs to be subtracted in the quotient, so including rounding,
	 * take ( tlClock - spiClock )/( 2*spiClock ) */
	uint32_t div = tlClockInHz - clockRateInHz;
	return div/( 2*clockRateInHz );
}

void SpiImp::enableTxInterrupt()
{
	hwRegOps::setBits( spiRegister->interruptEnable, txInterruptEnableBit );
}

void SpiImp::disableTxInterrupt()
{
	hwRegOps::clearBits( spiRegister->interruptEnable, txInterruptEnableBit );
}

void SpiImp::enableRxInterrupt()
{
	hwRegOps::setBits( spiRegister->interruptEnable, rxInterruptEnableBit );
}

void SpiImp::disableRxInterrupt()
{
	hwRegOps::clearBits( spiRegister->interruptEnable, rxInterruptEnableBit );
}

/* Interrupt handlers */

void SpiImp::spi1InterruptHandler()
{
    if ( 7 < spi1Transaction.bytesRemaining )
    {
    	spi1->rxWaterMark = 7;
    }
    else if ( 0 < spi1Transaction.bytesRemaining )
    {
    	spi1->rxWaterMark = spi1Transaction.bytesRemaining;
    }
    else
    {
    	spi1Transaction.complete = true;
    	spi1CsPin->set(); /* CS is idle HIGH */
    }

    _enqueTxData( &( spi1Transaction.txPointer ),
                  &( spi1Transaction.bytesRemaining ),
                  &( spi1->txData ) );
//    enqueTxData( &spi1Transaction, spi1 );
    _dequeRxData( &( spi1Transaction.rxPointer ),
                  &( spi1->rxData ) );
//    dequeRxData( &spi1Transaction, spi1 );
}

Spi::~Spi()
{
	/* C++ demands that even a pure virtual destructor has an implementation */
}
