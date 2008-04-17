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

#ifndef CORE_LOG_H
#define CORE_LOG_H

#include "config/config.h"
#include "libc/stdio.h"

__BEGIN_DECLS

typedef enum {
	LOG_NORMAL,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL
} log_level_t;

int  log_init(void);

int  log(log_level_t level, const char* format, ...);
void log_dump(FILE* stream);

void log_fini(void);

__END_DECLS

#endif /* CORE_LOG_H */
