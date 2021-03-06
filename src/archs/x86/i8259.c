/* -*- c -*- */

/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "config/config.h"
#include "libc/stdio.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "archs/x86/port.h"
#include "archs/x86/asm.h"
#include "archs/x86/i8259.h"
#include "archs/x86/arch.h"
#include "archs/x86/idt.h"
#include "dbg/debug.h"
#include "dbg/debugger.h"

#if CONFIG_I8259_DEBUG
#define dprintf(F,A...) printf("i8259: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define ICU_RESET  0x11
#define PIC_MASTER 0x20
#define PIC_SLAVE  0xA0
#define PIC_ELCONF 0x4D0 /* Edge/Level control register */

#define IRQ_CASCADE     2
#define PROTECT_CASCADE 1

#define CHECK_IRQ_INDEX(IDX) assert((IDX) < I8259_IRQS)

void i8259_eoi(uint_t irq)
{
        CHECK_IRQ_INDEX(irq);

        /* dprintf("Sending EOI for irq %d\n", irq); */
        if (irq >= 8) {
                /* dprintf("Sending EOI to slave\n"); */
                port_out8(PIC_SLAVE, 0x20);
        }

        /* dprintf("Sending EOI to master\n"); */
        port_out8(PIC_MASTER, 0x20);
}

void i8259_enable(uint_t irq)
{
        CHECK_IRQ_INDEX(irq);

        /* dprintf("Enabling irq %d\n", irq); */

#if PROTECT_CASCADE
        if (irq == IRQ_CASCADE) {
                dprintf("Never enable the cascade!\n");
                return;
        }
#endif

        if (irq < 8) {
                /* IRQ on master PIC */
                port_out8(PIC_MASTER + 1,
                          port_in8(PIC_MASTER + 1) & (~(1 << irq)));
        } else {
                /* IRQ on slave PIC */
                port_out8(PIC_SLAVE + 1,
                          port_in8(PIC_SLAVE + 1)  & (~(1 << (irq - 8))));
        }
}

void i8259_disable(uint_t irq)
{
        CHECK_IRQ_INDEX(irq);

        /* dprintf("Disabling irq %d\n", irq); */

#if PROTECT_CASCADE
        if (irq == IRQ_CASCADE) {
                dprintf("Never disable the cascade!\n");
                return;
        }
#endif

        if (irq < 8) {
                /* IRQ on master PIC */
                port_out8(PIC_MASTER + 1,
                          port_in8(PIC_MASTER + 1) | (1 << irq));
        } else {
                /* IRQ on slave PIC */
                port_out8(PIC_SLAVE + 1,
                          port_in8(PIC_SLAVE + 1)  | (1 << (irq - 8)));
        }
}

i8259_mask_t i8259_mask_get(void)
{
        return (port_in8(PIC_SLAVE + 1) << 8 |
                port_in8(PIC_MASTER + 1));
}

void i8259_mask_set(i8259_mask_t mask)
{
#if PROTECT_CASCADE
        if (mask & ~0xFFFB) {
                dprintf("Never disable the cascade!\n");
                mask &= 0xFFFB;
        }
#endif
        port_out8(PIC_MASTER + 1, (mask & 0x00FF));
        port_out8(PIC_SLAVE + 1,  (mask & 0xFF00) >> 8);
}

int i8259_init(void)
{
        dprintf("Initializing\n");

        /* Send ICW1: reset */
        dprintf("Resetting\n");
        port_out8(PIC_MASTER, ICU_RESET);
        port_out8(PIC_SLAVE,  ICU_RESET);

        /* Send ICW2: controller base address (IDT base index) */
        dprintf("Remapping to idt-base %d\n", I8259_IDT_BASE_INDEX);
        port_out8(PIC_MASTER + 1, I8259_IDT_BASE_INDEX);
        port_out8(PIC_SLAVE + 1,  I8259_IDT_BASE_INDEX + 8);

        /* Send ICW3 master: mask where the slave is connected to master */
        port_out8(PIC_MASTER + 1, 0x04);
        /* Send ICW3 slave: index where the slave is connected on master */
        port_out8(PIC_SLAVE + 1,  0x02);

        dprintf("Setting mode\n");
        /* Send ICW4: 8086 mode, fully nested, not bufferd, no implicit EOI */
        port_out8(PIC_MASTER + 1, 0x01);
        port_out8(PIC_SLAVE + 1,  0x01);

        /* Disable all IRQs except the cascade (IRQ2) */
        i8259_mask_set(0xFFFB);

        return 1;
}

void i8259_fini(void)
{
        dprintf("Finalizing\n");

        port_out8(PIC_MASTER, ICU_RESET);
        port_out8(PIC_SLAVE,  ICU_RESET);

        /* Disable all IRQs */
        i8259_mask_set(0xFFFF);
}

#if CONFIG_DEBUGGER
static dbg_result_t command_i8259_on_execute(FILE* stream,
                                             int   argc,
                                             char* argv[])
{
        assert(stream);

        unused_argument(argc);
        unused_argument(argv);

        fprintf(stream, "I8259:\n");
        fprintf(stream, "  Mask: 0x%04x\n", i8259_mask_get());

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(i8259,
                    "Show i8259",
                    NULL,
                    NULL,
                    command_i8259_on_execute,
                    NULL);
#endif
