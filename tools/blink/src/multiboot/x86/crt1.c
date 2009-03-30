/*
 * Copyright (C) 2008 Francesco Salvestrini
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

#include "config.h"
#include "elklib.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "libc/ctype.h"
#include "multiboot/multiboot.h"

#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

/*
 * CR0 flags
 */
#define CR0_PG  0x80000000 /* Enable Paging */
#define CR0_CD  0x40000000 /* Cache Disable */
#define CR0_NW  0x20000000 /* No Write-through */
#define CR0_AM  0x00040000 /* Alignment check Mask */
#define CR0_WP  0x00010000 /* Write-Protect kernel access */
#define CR0_NE  0x00000020 /* handle Numeric Exceptions */
#define CR0_ET  0x00000010 /* Extension Type is 80387 coprocessor */
#define CR0_TS  0x00000008 /* Task Switch */
#define CR0_EM  0x00000004 /* EMulate coprocessor */
#define CR0_MP  0x00000002 /* Monitor coProcessor */
#define CR0_PE  0x00000001 /* Protected mode Enable */

static unsigned long cr0_get(void)
{
	register unsigned long cr0;

	__asm__ volatile ("movl %%cr0, %0" : "=r" (cr0) : );

	return cr0;
}

/*
 * EFLAGS bits
 */
#define EFLAGS_CF   0x00000001 /* Carry Flag */
#define EFLAGS_PF   0x00000004 /* Parity Flag */
#define EFLAGS_AF   0x00000010 /* Auxillary carry Flag */
#define EFLAGS_ZF   0x00000040 /* Zero Flag */
#define EFLAGS_SF   0x00000080 /* Sign Flag */
#define EFLAGS_TF   0x00000100 /* Trap Flag */
#define EFLAGS_IF   0x00000200 /* Interrupt Flag */
#define EFLAGS_DF   0x00000400 /* Direction Flag */
#define EFLAGS_OF   0x00000800 /* Overflow Flag */
#define EFLAGS_IOPL 0x00003000 /* IOPL mask */
#define EFLAGS_NT   0x00004000 /* Nested Task */
#define EFLAGS_RF   0x00010000 /* Resume Flag */
#define EFLAGS_VM   0x00020000 /* Virtual Mode */
#define EFLAGS_AC   0x00040000 /* Alignment Check */
#define EFLAGS_VIF  0x00080000 /* Virtual Interrupt Flag */
#define EFLAGS_VIP  0x00100000 /* Virtual Interrupt Pending */
#define EFLAGS_ID   0x00200000 /* CPU ID detection flag */

static unsigned long eflags_get(void)
{
	unsigned long ef;

	__asm__ volatile ("pushf; popl %0" : "=r" (ef));

	return ef;
}

static int check_machine_state(void)
{
	uint32_t tmp;

	/* Check CR0 state */
	tmp = cr0_get();
	if (tmp & CR0_PG) {
		printf("Paging flag already set\n");
		return 0;
	}
	if (!(tmp & CR0_PE)) {
		printf("No protected mode flag set\n");
		return 0;
	}

	/* Check eflags state */
	tmp = eflags_get();
	if (tmp & EFLAGS_VM) {
		printf("Virtual mode flag already set\n");
		return 0;
	}
	if (tmp & EFLAGS_IF) {
		printf("Interrupt flag already set\n");
		return 0;
	}

	return 1;
}

/*
 * NOTE:
 *     This is our starting point, we start from here directly from the
 *     boot-loader with a (probably good) multiboot info structure.
 *     structure.
 */
void crt1_multiboot(unsigned long magic,
                    unsigned long addr)
{
	multiboot_info_t * mbi;

	/*
	 * NOTE:
	 *     Add early support, call it as soon as possible
	 */
        elklib_c_init();

        /* Turn off all streams ... */
        FILE_set(stdin,  NULL, NULL, NULL, NULL);
        FILE_set(stdout, NULL, NULL, NULL, NULL);
        FILE_set(stderr, NULL, NULL, NULL, NULL);

	/* Am I booted by a Multiboot-compliant boot loader?  */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid magic number: 0x%x", (unsigned) magic);
	}
	/* Yes, it seems so ... */

	if (!check_machine_state()) {
		panic("Wrong machine state");
	}

        multiboot(magic, addr);
}