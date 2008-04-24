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

#ifndef LIBS_TIME_H
#define LIBS_TIME_H

#include "config/config.h"
#include "core/archs/arch.h"

__BEGIN_DECLS

#define delay_ms(X) arch_delay_ms(X)
#define delay_us(X) arch_delay_us(X)
#define delay_ns(X) arch_delay_ns(X)

__END_DECLS

#endif /* LIBS_TIME_H */