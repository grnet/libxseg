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

#ifndef _XSEG_DOMAIN_H
#define _XSEG_DOMAIN_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_PEER_NAME 64
#define MAX_LOGFILE_LEN 1024

/* domain-provided functions */
void __lock_domain(void);
void __unlock_domain(void);
void __load_plugin(const char *name);
int __xseg_preinit(void);
uint64_t __get_id(void);
void __get_current_time(struct timeval *tv);

extern char __xseg_errbuf[4096];
void __xseg_log(const char *msg);
extern int (*xseg_snprintf)(char *str, size_t size, const char *format, ...);

enum log_level { E = 0, W = 1, I = 2, D = 3};

extern void (*init_logctx)(char *peer_name, enum log_level level);
void __xseg_log2(enum log_level level, char *fmt, ...);

void xseg_printtrace(void);
#endif
