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

#include "config/config.h"
#include "core/archs/arch.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/timer.h"
#include "core/mutex.h"
#include "libs/list.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"

#if 0
static LIST_HEAD(timers);
static size_t granularity;

static void timers_update(void)
{
	timer_t * timer;

	if (LIST_ISEMPTY(timers)) {
		return;
	}

	timer = LIST_ENTRY(timers.next, timer_t, list);
	assert(timer);

	timer->expiration -= granularity;
	if (TIMER_EXPIRED(timer)) {
		LIST_REMOVE(timer->list);
		timer->callback(timer->data);
	}
}
#endif

int timers_init(void)
{
#if 0
	LIST_INIT(timers);

	granularity = arch_timer_granularity();
	assert(granularity > 0);

	timers_update();
#endif
	return 1;
}

int timers_fini(void)
{
	return 1;
}

#if 0
int timer_add(timer_t * timer)
{
	assert(timer);
	if ((timer->callback == NULL) ||
	    (timer->expiration < 0)){
		return 0;
	}

	if (LIST_ISEMPTY(timers)) {
		LIST_INSERT_AFTER(timers, timer->list);
	} else {
		timer_t * curr1, * curr2;

		LIST_FOREACH_FORWARD(&timers, curr1->list) {
			curr2 = curr1->list.next;
			if (curr1->expiration > timer->expiration) {
				timer->expiration -=
					LIST_ENTRY(&curr1->list.prev,
						   timer_t,
						   list)->expiration;
				curr1->expiration -= timer->expiration;
				LIST_INSERT_BEFORE(timers, timer->list);
				return 1;
			}
		}

		if (curr1->list.prev != &timers) {
			timer->expiration -= LIST_ENTRY(&curr1->list.prev,
							timer_t,
							list);
		}
		LIST_INSERT_TAIL(timers, timer->list);
	}

	return 1;
}

int timer_remove(timer_t * timer)
{
	int err = 0;

	assert(timer);

	if (LIST_ISEMPTY(timers)) {
		return err;
	} else {
		timer_t * curr1;

		LIST_FOREACH_FORWARD(&timers, &curr1->list) {
			if (curr1 == timer) {
				if (timer->expiration > 0) {
					LIST_ENTRY(&curr1->list.prev,
						   timer_t,
						   list)->expiration +=
						timer->expiration;
				}

				LIST_REMOVE(curr1->list);

				return 1;
			}
		}
	}

	return err;
}
#endif

#if CONFIG_DEBUGGER
static dbg_result_t command_timers_on_execute(FILE * stream,
					      int    argc,
					      char * argv[])
{
#if 0
	list_entry_t * temp_l;
#endif

	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Timers:\n");
#if 0
	LIST_FOREACH_FORWARD(&timers, temp_l) {
		timer_t * temp_t;
		temp_t = LIST_ENTRY(temp_l, timer_t, list);
		fprintf(stream, "  0x%p %d\n",
			temp_t->callback, temp_t->expiration);
	}
#endif
	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(timers,
		    "Show timers",
		    NULL,
		    NULL,
		    command_timers_on_execute,
		    NULL);
#endif
