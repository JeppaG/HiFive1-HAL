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
 * exception.S
 *
 *  Created on: 28 Aug 2019
 *      Author: jeppa
 */
.extern interruptVectorTable

.equ plic_claim, 0x0c200004
.equ interrupt_enable_1, 0x0c002000
.equ interrupt_enable_2, 0x0c002004
.equ number_of_plic_interrupts, 52
.equ pending, 0x0c001000

.section .text.trap_vector
.global _set_trap_vector
_set_trap_vector:
    .cfi_startproc
        /* Set the trapvector, mtvec, to the address of the trap-dispatcher trap_vector below */
    	la t0, trapvector
    	csrw mtvec, t0
    	ret
    .cfi_endproc

.global _disable_all_plic_interrupts
_disable_all_plic_interrupts:
	.cfi_startproc
		/* This function disables all plic interrupts it is intended to be called from C++ with the following form:

		   void _disable_all_plic_interrupt();

		   Registers are used as follows:
		   t0 - holds the address of the InterruptEnable register to be written
		   t1 - holds the total number of plic interrupts
		   t2 - loop counter
		*/

		li t0, interrupt_enable_1        /* t0 points to the first interrupt enable register in plic */
		li t1, number_of_plic_interrupts /* This is the total number of plic interrupts */
		add t2, zero, zero               /* use t2 as loop counter */
1:      sw zero, (t0)					 /* Disable all interrupts in this InterruptEnable register */
		addi t0, t0, 4  				 /* Go to the next InterruptEnable register */
		addi t2, t2, 32					 /* Add the number of interrupts covered in this loop */
		blt t2, t1, 1b                   /* Check if all Interrupts are done */

		ret
	.cfi_endproc


.global _enable_plic_interrupt
_enable_plic_interrupt:
	.cfi_startproc
	    /* This function is intended to be called from C++ with the following form:

	       void _enable_plic_interrupt( uint32_t interruptId );

	       interruptId is the number identifying the interrupt to clear the pending bit for

	       Registers are used as follows:
	       a0 - carries the argument, interruptId from the caller
	       a1 - pointer to the plic interrupt enable base address
	       a2 - holds the bit corresponding to the MEIE bit in the mie csr
	       t0 - pointer to the current plic interrupt enable register
	       t1 - pointer to the plic claim/complete register
	       t2 - holding the bit-mask to enable/disable a given interrupt
	       t3 - holding the (interruptId DIV 32)*4, + base address, ie the address of the InterruptEnable register
	       t4 - Used as temporary variable
	       t5 - Used as temporary variable
	       t6 - Used as temporary variable
	    */
		li a1, interrupt_enable_1 /* a1 points to the first interruptEnable register in plic */
		li t1, plic_claim         /* t1 points to the claim/complete register */
		addi t2, zero, 1          /* set t2 , bit 0 to 1 */
		srli t3, a0, 5            /* set t3 to interruptId DIV 32, i.e InterruptEnable register offset */
		slli t4, t3, 5            /* set t4 to interruptId MOD 32, i.e the bit position within - */
		sub t4, a0, t4            /* - the selected InterruptEnable register */
		slli t3, t3, 2            /* Multiply by 4 to get the address offset in bytes */
		add t3, t3, a1            /* Add the InterruptEnable base address */
		sll t2, t2, t4            /* Set t2 as bit mask for the selected bit */

		li a2, 0x1024             /* Clear bit 11 in the machine interrupt enable register */
		csrrc zero, mie, a2       /* to disable machine external interrupts while clearing IP bit */

        /* Clear all plic interrupt enable bits and save them to enable them again when done */
		add t6, zero, zero        /* use t6 as loop counter and t4 as end marker for the loop */
		addi t4, zero, number_of_plic_interrupts
		add t0, zero, a1          /* reset address pointer to InterruptEnable base address */
1:		lw t5, (t0)               /* Read the next word of interrupt enables */
		bne t0, t3, 2f            /* Check if this is the register holding the interrupt to be enabled */
		or t5, t5, t2			  /* Save the new interrupt enable bit as set */
2:      sw t5, (sp)
        addi sp, sp, -4           /* Save the present interupEnables on the stack */
        sw zero, (t0)             /* Disable all interrupts in the present InterruptEnable register */
        addi t0, t0, 4            /* increment the InterruptEnable register to the next register */
        addi t6, t6, 32           /* Increment the loop counter with the number of interrupts covered in this loop */
        blt t6, t4, 1b            /* Take the next InteruptEnable register */

		/* Enable the selected interrupt and clear the Pending bit for it */
		sw t2, (t3)               /* Enable the interrupt selected. This is now the only plic interrupt that is enabled */
		lw zero, (t1)             /* Claim interrupt. If the IP bit is set it will now be cleared */
		sw a0, (t1)               /* Signal interrupt complete to enable forwarding of the next interrupt */

		/* Enable all interrupts that were previously enabled */
        addi t0, t0, -4           /* t0 points to the last interruptEnable register in plic */
1:		lw t5, 4(sp)              /* Read the next word of saved interrupt enables */
        addi sp, sp, 4            /* Update stack-pointer */
        sw t5, (t0)               /* Re-enable interrupts that should be enabled */
        addi t0, t0, -4           /* Decrement the InterruptEnable register to the next register */
        bge t0, a1, 1b            /* Take the next InteruptEnable register */

		csrrs zero, mie, a2       /* Enable machine external interrupts */

		ret
	.cfi_endproc

.global _enable_interrupt
_enable_interrupt:
	.cfi_startproc
		addi t0, zero, 0x8         /* Clear bit 3 in the machine interrupt enable register */
		csrrc zero, mie, t0        /* to disable machine software interrupt                */
		slli t0, t0, 4             /* Clear bit 7 in the machine interrupt enable register */
		csrrc zero, mie, t0        /* to disable machine timer interrupts               */
		slli t0, t0, 4             /* Set bit 11 in the machine interrupt enable register */
		csrrs zero, mie, t0        /* to enable machine external interrupts               */
		addi t0, zero, 0x8         /* Set bit 3 in the machine status register to enable */
		csrrs zero, mstatus, t0    /* global interupts                                   */
		ret
	.cfi_endproc

.align 2                           /* This aligns the address to an even 2^n byte, so n=2 gives word alignment */
trapvector:
	.cfi_startproc
		addi sp, sp, -64            /* Save ALL caller saved rgisters on the stack.  */
		sw ra, 64(sp)               /* Since the user context is not aware that the  */
		sw t0, 60(sp)				/* interrupt is executing, it has no way of preserving */
		sw t1, 56(sp)               /* its registers, and the interrupt context doesn't  */
		sw t2, 52(sp)               /* know which registers are in use in the user context */
		sw a0, 48(sp)
		sw a1, 44(sp)
		sw a2, 40(sp)
		sw a3, 36(sp)
		sw a4, 32(sp)
		sw a5, 28(sp)
		sw a6, 24(sp)
		sw a7, 20(sp)
		sw t3, 16(sp)
		sw t4, 12(sp)
		sw t5, 8(sp)
		sw t6, 4(sp)
		csrrc t0, mcause, zero      /* Read the mcaus register into t0 */
		bgez t0, dispatch_exception /* Go on to dispatch an exception. Interrupts have mcause < 0 */
		slli t0, t0, 2              /* This is an interrupt. Use t0 as index for the interrupt vector table */
		la t1, interrupt_vector_table
		add t0, t0, t1              /* Pick the correct address for the handler in the vector table */
        lw t0, (t0)
		jalr t0                     /* Call the interrupt handler pointed to by the vector table */
		lw ra, 64(sp)               /* Pop ALL caller saved registers before returning from the */
		lw t0, 60(sp)				/* interrupt */
		lw t1, 56(sp)
		lw t2, 52(sp)
		lw a0, 48(sp)
		lw a1, 44(sp)
		lw a2, 40(sp)
		lw a3, 36(sp)
		lw a4, 32(sp)
		lw a5, 28(sp)
		lw a6, 24(sp)
		lw a7, 20(sp)
		lw t3, 16(sp)
		lw t4, 12(sp)
		lw t5, 8(sp)
		lw t6, 4(sp)
		addi sp, sp, 64
		mret                        /* Return from interrupt */
	.cfi_endproc

dispatch_exception:
	.cfi_startproc
		j dispatch_exception        /* Never ending loop to catch unexpected exceptions */
	.cfi_endproc

interrupt_vector_table:
	.4byte	reserved                /* Not used, interrupt #0 */
	.4byte	reserved                /* Not used, interrupt #1 */
	.4byte	reserved                /* Not used, interrupt #2 */
	.4byte  machine_sw_interrupt
	.4byte	reserved                /* Not used, interrupt #4 */
	.4byte	reserved                /* Not used, interrupt #5 */
	.4byte	reserved                /* Not used, interrupt #6 */
	.4byte  machine_timer_interrupt
	.4byte	reserved                /* Not used, interrupt #8 */
	.4byte	reserved                /* Not used, interrupt #9 */
	.4byte	reserved                /* Not used, interrupt #10 */
	.4byte  machine_external_interrupt
	.4byte	reserved                /* Not used, interrupt #12 */
	.4byte	reserved                /* Not used, interrupt #13 */
	.4byte	reserved                /* Not used, interrupt #14 */
	.4byte	reserved                /* Not used, interrupt #15 */

machine_sw_interrupt:
	.cfi_startproc
		j machine_sw_interrupt
	.cfi_endproc

machine_timer_interrupt:
	.cfi_startproc
		j machine_timer_interrupt
	.cfi_endproc

machine_external_interrupt:
	.cfi_startproc
		li t0, plic_claim           /* Claim the interrupt */
		lw t1, (t0)
		beq t1, zero, 1f            /* In the event that no interrupt is pending, skip handling of it */
		addi sp, sp, -12             /* Push the claimed interrupt ID to the stack */
		sw t0, 4(sp)
		sw t1, 8(sp)
		sw ra, 12(sp)

		/* Jump to the correct interrupt handler in C++ using the external C++ interrupt vector table */
		la t0, interruptVectorTable /* This is the base address for the vector table */
		slli t1, t1, 2              /* The address offset is the interrupt number * 4 */
		add t0, t0, t1
		lw t0, (t0)                 /* Pick the jump address from the vector table */
		jalr t0					    /* Jump to the interrupt handler. */

		lw t0, 4(sp)                /* Pop the claimed interrupt ID from the stack */
		lw t1, 8(sp)
		lw ra, 12(sp)
		addi sp, sp, 12
		sw t1, (t0)             /* Signal interrupt completion */
1:		ret                     /* Return to the trapvector dispatcher */
	.cfi_endproc

reserved:
	.cfi_startproc
		j reserved
	.cfi_endproc

