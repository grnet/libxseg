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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <execinfo.h>
#include <syslog.h>
#include <xseg/util.h>
#include <xseg/xtypes.h>
#include <xseg/domain.h>

#include <xseg/xlock.h>

int (*xseg_snprintf)(char *str, size_t size, const char *format, ...) = snprintf;

char __xseg_errbuf[4096];

static struct xlock __lock = { .owner = XLOCK_NOONE};

void __lock_domain(void)
{
	(void)xlock_acquire(&__lock);
}

void __unlock_domain(void)
{
	xlock_release(&__lock);
}

void __load_plugin(const char *name)
{
	void *dl;
	void (*init)(void);
	char _name[128];
	unsigned int namelen = strlen(name);

	strncpy(_name, "xseg_", 5);
	strncpy(_name + 5, name, 80);
	strncpy(_name + 5 + namelen, ".so", 3);
	_name[5 + namelen + 3 ] = 0;
	dl = dlopen(_name, RTLD_NOW);
	if (!dl) {
		XSEGLOG("Cannot load plugin '%s': %s\n", _name, dlerror());
		return;
	}

	strncpy(_name + 5 + namelen, "_init", 5);
	_name[127] = 0;
	init = (void (*)(void))(long)dlsym(dl, _name);
	if (!init) {
		XSEGLOG("Init function '%s' not found!\n", _name);
		return;
	}

	init();
	//XSEGLOG("Plugin '%s' loaded.\n", name);
}

uint64_t __get_id(void)
{
	return (uint64_t)syscall(SYS_gettid);
}

void __xseg_log(const char *msg)
{
	fprintf(stderr, "%s", msg);
	fflush(stderr);
}

void *xtypes_malloc(unsigned long size)
{
	return malloc(size);
}

void xtypes_free(void *ptr)
{
	free(ptr);
}

void __get_current_time(struct timeval *tv) {
	gettimeofday(tv, NULL);
}

static inline int syslog_level(enum log_level level) {
    int syslog_level;

    switch (level) {
        case E: syslog_level = LOG_ERROR; break;
        case W: syslog_level = LOG_WARNING; break;
        case I: syslog_level = LOG_INFO; break;
        case D: syslog_level = LOG_DEBUG; break;
        default: syslog_level = LOG_DEBUG; break;
    }

    return syslog_level;
}

static void __init_logctx(char *peer_name, enum log_level level)
{
    int syslog_level;

    openlog(peer_name, LOG_PID | LOG_CONS, LOG_LOCAL0);

    /* FIXME: LOG_UPTO portability */
    setlogmask(LOG_UPTO(syslog_level(level)));

    return;
}

void (*init_logctx)(char *peer_name, enum log_level level) = __init_logctx;

void __xseg_log2(enum log_level level, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
    vsyslog(syslog_level(level), fmt, ap);
	va_end(ap);

	return;
}

/* TODO: Make an async-safe alternative */
void xseg_printtrace(void)
{
	void *array[20];
	char **bt;
	size_t size;
	int i;
	pid_t tid = __get_id();

	size = backtrace(array, 20);
	bt = backtrace_symbols(array, size);
	if (!bt) {
		return;
	}

	XSEGLOG("Backtrace of tid %d:", tid);
	for (i = 0; i < size; ++i)
	{
		XSEGLOG("\t%s", bt[i]);
	}
}
