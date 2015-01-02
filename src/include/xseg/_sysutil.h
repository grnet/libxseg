/*
Copyright (C) 2010-2014 GRNET S.A.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SYSUTIL_H
#define _SYSUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <sys/domain.h>

#define REOPEN_FILE     (1 << 1)
#define REDIRECT_STDOUT (1 << 1)
#define REDIRECT_STDERR (1 << 2)


struct log_ctx {
//      int stdout_orig;
//      int stderr_orig;
    char filename[MAX_LOGFILE_LEN];
    volatile int logfile;
    char peer_name[MAX_PEER_NAME];
    unsigned int log_level;
    uint32_t flags;
};

#endif
