//
// Copyright (C) 2008 Francesco Salvestrini
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

#ifndef CORE_PROCESS_H
#define CORE_PROCESS_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc++/string"
#include "libc++/list"
#include "core/thread.h"

typedef uint_t pid_t;

class process {
public:
	process(pid_t id);
	~process();

	pid_t id(void) { return id_; };

protected:
	process();

private:
	pid_t               id_;
	ktl::string         name_;
	ktl::list<thread *> threads_;
};

#endif // CORE_PROCESS_H
