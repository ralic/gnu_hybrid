// -*- c++ -*-

//
// Copyright (C) 2008, 2009 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "config/config.h"
#include "archs/arch.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "core/timer.h"
#include "dbg/debug.h"
#include "core/interrupt.h"
#include "dbg/debugger.h"
#include "libc++/cstdio"
#include "libc++/cstring"
#include "libc++/list"

#define BANNER          "timer: "

#if CONFIG_TIMERS_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static std::list<timer_t *> timers;
static size_t               granularity;

static int timer_enqueue(timer_t * timer)
{
        std::list<timer_t *>::iterator iter;

        for (iter = timers.begin(); iter != timers.end(); iter++) {
                if ((*iter)->expiration > timer->expiration) {
                        dprintf("Cursor expiration time is bigger\n");
                        break;
                }

                timer->expiration -= (*iter)->expiration;
                dprintf("Decremented timer expiration (now %d)\n",
                        timer->expiration);

                if (timer->expiration < 0) {
                        dprintf("Timer expiration underflow while walking\n");
                        timer->expiration = 0;
                        break;
                }
        }

        timers.insert(iter, timer);

        TIMERS_DUMP(timers);

        return 1;
}

void timers_update(void * unused)
{
        unused_argument(unused);
        assert(granularity > 0);

        dprintf("Updating timers (granularity %dHz)\n", granularity);
        if (timers.empty()) {
                dprintf("List is empty, quitting update\n");
                return;
        } else {
                dprintf("Timers to go %d\n", timers.size());
        }

        TIMERS_DUMP(timers);
        timer_t * & timer = timers.front();
        assert(timer);

        dprintf("Updating timer 0x%p\n", timer);
        TIMER_DUMP(timer);

        timer->expiration -= granularity;
        if (TIMER_EXPIRED(timer)) {
                dprintf("Timer 0x%p expired\n", timer);
                TIMER_DUMP(timer);

                if (!timer->callback) {
                        dprintf("Timer 0x%p callback is empty\n", timer);
                        return;
                }

                timer->callback(timer->data);

                switch (timer->type) {
                        case TIMER_ONE_SHOT:
                                // Remove the entry now
                                timers.pop_front();
                                break;
                        case TIMER_REPETITIVE:
                                break;
                        default:
                                bug();
                                break;
                }
        }

        TIMERS_DUMP(timers);

        dprintf("Updated completed\n");
}

int timers_init(void)
{
        dprintf("Initializing timers\n");

        timers.clear();

        granularity = arch_timer_granularity();
        assert(granularity > 0);

        dprintf("Timers granularity %dHz\n", granularity);

        if (!interrupt_attach(0, timers_update, 0)) {
                dprintf("Cannot attach timers update interrupt handler\n");
                return 0;
        }
        if (!interrupt_enable(0)) {
                dprintf("Cannot enable timer interrupt\n");
                return 0;
        }
        dprintf("Initialized\n");

        return 1;
}

int timers_fini(void)
{
        dprintf("Finalizing timers\n");

        if (!interrupt_disable(0)) {
                dprintf("Cannot disable timer interrupt\n");
                return 0;
        }
        if (!interrupt_detach(0, timers_update)) {
                dprintf("Cannot detach timers update interrupt handler\n");
                return 0;
        }

        if (!timers.empty()) {
                dprintf("Timers list not empty\n");
                return 0;
        }

        return 1;
}

int timer_add(timer_t * timer)
{
        assert(timer);

#if CONFIG_TIMERS_DEBUG
        timer->absolute = timer->expiration;
#endif

        if (!TIMER_GOOD(timer)) {
                dprintf("Cannot add timer, no useful infos\n");
                return 0;
        }

        dprintf("Adding timer 0x%p (expiration %d)\n",
                timer, timer->expiration);

        return timer_enqueue(timer);
}

int timer_remove(timer_t * timer)
{
        dprintf("Removing timer 0x%p\n", timer);

        assert(timer);

        if (timers.empty()) {
                dprintf("Timers list is empty, cannot remove\n");
                return 0;
        }

        std::list<timer_t *>::iterator iter1;

        for (iter1 = timers.begin(); iter1 != timers.end(); iter1++) {
                dprintf("Walking timer 0x%p\n", (*iter1));
                if ((*iter1) == timer) {
                        dprintf("Got timer!\n");

                        std::list<timer_t *>::iterator iter2;

                        iter2 = iter1;
                        iter2++;

                        if (iter2 != timers.end()) {
                                dprintf("Next timer is 0x%p\n", (*iter2));

                                if ((*iter1)->expiration > 0) {
                                        dprintf("Timer 0x%p is not expired, "
                                                "fixing\n", (*iter1));
                                        (*iter2)->expiration +=
                                                (*iter1)->expiration;
                                }
                        }

                        dprintf("Removing timer 0x%p from list\n", (*iter1));
                        timers.erase(iter1);

                        dprintf("Timer 0x%p removed successfully\n", timer);
                        return 1;
                }
        }

        dprintf("Timer 0x%p not found\n", timer);

        return 0;
}

#if CONFIG_DEBUGGER
static void callback(void * data)
{
        unused_argument(data);

        dprintf("Called test timer callback with opaque data 0x%p", data);
}

static dbg_result_t command_timers_on_execute(FILE * stream,
                                              int    argc,
                                              char * argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc == 0) {
                fprintf(stream, "Timers:\n");

                std::list<timer_t *>::iterator iter;
                for (iter = timers.begin(); iter != timers.end(); iter++) {
#if CONFIG_TIMERS_DEBUG
                        fprintf(stream, "  0x%p 0x%p %d (%d)\n",
                                (*iter),
                                (*iter)->callback,
                                (*iter)->expiration,
                                (*iter)->absolute);
#else
                        fprintf(stream, "  0x%p 0x%p %d\n",
                                (*iter),
                                (*iter)->callback,
                                (*iter)->expiration);
#endif
                }

                return DBG_RESULT_OK;
        } else if (argc == 2) {
                int time;

                time = atoi(argv[1]);

                if (!strcmp(argv[0], "add")) {
                        timer_t * t;

                        t = new timer_t;
                        t->expiration = time;
                        t->callback   = callback;
#if CONFIG_TIMERS_DEBUG
                        t->removable  = 1;
#endif
                        if (!timer_add(t)) {
                                return DBG_RESULT_ERROR;
                        }
                        return DBG_RESULT_OK;

                } else if (!strcmp(argv[0], "remove")) {
                        std::list<timer_t *>::iterator iter;
                        timer_t *                      timer;

                        for (iter  = timers.begin();
                             iter != timers.end();
                             iter++) {
                                if ((*iter)->expiration == time) {
                                        break;
                                }
                        }
                        if (iter == timers.end()) {
                                return DBG_RESULT_ERROR;
                        }

                        timer = *iter;
#if CONFIG_TIMERS_DEBUG
                        if (!timer->removable) {
                                dprintf("Cannot remove timer\n");
                                return DBG_RESULT_ERROR;
                        }
#endif
                        dprintf("Deleting timer 0x%p\n", timer);
                        timers.erase(iter);
                        delete timer;

                        return DBG_RESULT_OK;
                } else {
                        return DBG_RESULT_ERROR_WRONG_PARAMETERS;
                }
        }

        return DBG_RESULT_ERROR_WRONG_PARAMETERS;
}

DBG_COMMAND_DECLARE(timers,
                    "Manage timers",
                    NULL,
                    NULL,
                    command_timers_on_execute,
                    NULL);
#endif
