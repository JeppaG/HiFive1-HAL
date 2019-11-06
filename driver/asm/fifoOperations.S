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
 * fifoOperations.S
 *
 *  Created on: 14 Oct 2019
 *      Author: jeppa
 */

 /*
  * Function: _enqueTxDataFromRingBuffer
  *
  * C/C++ interface:
  *
  * bool _enqueTxDataFromRingBuffer( uint8_t*           bufferHead,
  *                                  uint8_t            bufferSize,
  *						             uint8_t*           readPointer,
  *                                  uint8_t            writePointer,
  *                                  volatile uint32_t* txFifoPtr );
  *
  * By RISC-V calling convention, the register mapping above is:
  *   bool returnValue                : a0
  *   uint8_t* bufferHead             : a0
  *   uint8_t  buffersize             : a1
  *   uint8_t* readPointer            : a2
  *   uint8_t  writePointer           : a3
  *   volatile uint32_t txfifoPointer : a4
  *
  *   temporary registers are used as follows:
  *   de-referenced read-pointer                 : t0
  *   read adress                                : t1
  *   next byte to transmit and fifo full status : t2
  */

.global _enqueTxDataFromRingBuffer
_enqueTxDataFromRingBuffer:
	.cfi_startproc
		lb t0, (a2);            /* put the readPointer into register t0 */
		add t2, zero, zero;     /* Set the initial state of the return value to that there are no more bytes to send */
1:  	beq t0, a3, 2f;         /* End when read and write pointers are equal */
		add t1, a0, t0;         /* Set read address in t1 as buffer address + read pointer */
		lb  t2, (t1);           /* Read the next byte to transmit into t2 */
	    amoswap.w t2, t2, (a4); /* Enque in txFifo and read out success status */
	    bne t2, zero, 2f;       /* If no more bytes could be enqued, quit */
		addi t0, t0, 1;         /* Increment the read pointer */
	    blt t0, a1, 1b;         /* Check if the end of the buffer is reached */
	    add t0, zero, zero;     /* When the end of the buffer is reached, restart at 0 */
		beq zero, zero, 1b;     /* Goto the beginning of the loop and process the next byte */

2:      srli a0, t2, 31;        /* Put the return value, i.e. the flag indicating that there are more bytes to send in a0 */
        sb t0, (a2);            /* Save the updated read-pointer */

        ret
	.cfi_endproc

 /*
  * Function: _dequeRxDataToRingBuffer
  *
  * C/C++ interface:
  *
  * void _dequeRxDataToRingBuffer( uint8_t*           bufferHead,
  *                                uint8_t            bufferSize,
  *						           uint8_t            readPointer,
  *                                uint8_t*            writePointer,
  *                                volatile uint32_t*  rxFifoPtr );
  *
  * By RISC-V calling convention, the register mapping above is:
  *   uint8_t* bufferHead              : a0
  *   uint8_t  buffersize              : a1
  *   uint8_t  readPointer             : a2
  *   uint8_t* writePointe             : a3
  *   volatile uint32_t* rxfifoPointer : a4
  *
  *   temporary registers are used as follows:
  *   de-referenced write-pointer                                  : t0
  *   next write pointer                                           : t1
  *   next received byte and fifo empty status                     : t2
  *   absolute ring buffer address pointed to by the write-pointer : t3
  */
.global _dequeRxDataToRingBuffer
_dequeRxDataToRingBuffer:
	.cfi_startproc
		lb t0, (a3);		/* Put the write pointer into register t0 */
1:      addi t1, t0, 1;     /* t1 is the next write pointer */
		blt t1, a1, 2f;     /* Check if the next write pointer is outside the buffer */
		add t1, zero, zero; /* If the next write pointer is outside the buffer, restart at 0 */
2:      lw t2, (a4);        /* Deque the next Rx byte to t2 */
		blt t2, zero, 3f;   /* Check if the Rx FIFO was empty, in that case we are done */
		add t3, a0, t0;     /* Set write address in t3 as buffer address + write pointer */
		sb t2, (t3);        /* Write the dequed byte to the Rx buffer */
		beq t1, a2, 1b;     /* If the next write pointer equals the read-pointer, the buffer is full... */
		add t0, t1, zero;   /* ...otherwise set the write pointer to the next write pointer */
		beq zero, zero, 1b;

3:		sb t0, (a3)         /* Save the updated write-pointer */

        ret
	.cfi_endproc


 /*
  * Function: _enqueTxData
  *
  * C/C++ interface:
  *
  * bool _enqueTxData( uint8_t**          txPointer,
  *                    uint8_t*           bytesRemaining,
  *                    volatile uint32_t* txFifoPtr );
  *
  * By RISC-V calling convention, the register mapping above is:
  *   bool              returnValue    : a0
  *   uint8_t*          txPointer      : a0
  *   uint8_t*          bytesRemaining : a1
  *   volatile uint32_t txfifoPointer  : a2
  *
  *   temporary registers are used as follows:
  *   de-referenced read-address                 : t0
  *   de-referenced byte-count                   : t1
  *   next byte to transmit and fifo full status : t2
  */
.global _enqueTxData
_enqueTxData:
	.cfi_startproc
		lw t0, (a0);            /* dereference the txPointer into t0 */
		lb t1, (a1);            /* dereference the byte-count into t1 */
1:	 	beq zero, t1, 2f;       /* End when no more bytes remain */
   		lb t2, (t0);            /* Read the next byte to transmit from the buffer */
		amoswap.w t2, t2, (a2); /* Enque in txFifo and read out success status */
		bne t2, zero, 2f;       /* If no more bytes could be enqued, quit */
		addi t0, t0, 1;         /* Increment the read address */
		addi t1, t1, -1;        /* Decrement the byte-count */
		beq zero, zero, 1b;

2:		sb t1, (a1)             /* Save the byte-count */
		sw t0, (a0)				/* Save the txPointer */
		srli a0, t2, 31;        /* Put the return value, i.e. the flag indicating that there are more bytes to send in a0 */

        ret
	.cfi_endproc

 /*
  * Function: _dequeRxData
  *
  * C/C++ interface:
  *
  * void _dequeRxData( uint8_t**           rxPointer,
  *                    volatile uint32_t*  rxFifoPtr );
  *
  * By RISC-V calling convention, the register mapping above is:
  *   uint8_t** rxPointer              : a0
  *   volatile uint32_t* rxfifoPointer : a1
  *
  *   temporary registers are used as follows:
  *   de-referenced rxPointer                  : t0
  *   next received byte and fifo empty status : t1
  */
.global _dequeRxData
_dequeRxData:
	.cfi_startproc
		lw t0, (a0);        /* dereference the rxPointer into t0 */
1: 		lw t1, (a1);        /* deque the next rx byte from the rxFiFo */
    	blt t1, zero, 2f;   /* Check if the Rx FIFO was empty, in that case we are done */
		sb t1, (t0);        /* Write the dequed byte to the buffer */
		addi t0, t0, 1;     /* Increment the rxPointer */
		beq zero, zero, 1b;

2:		sb t0, (a0);        /* Save the rxPointer */

        ret
	.cfi_endproc
