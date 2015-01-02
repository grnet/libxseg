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
#include <xseg/util.h>
#include <xseg/xtypes.h>
#include <xseg/domain.h>

#include <xseg/xlock.h>

char __xseg_errbuf[4096];

static struct xlock __lock = {.owner = XLOCK_NOONE };

void __lock_domain(void)
{
    (void) xlock_acquire(&__lock);
}

void __unlock_domain(void)
{
    xlock_release(&__lock);
}

void __load_plugin(const char *name)
{
    void *dl;
    void (*init) (void);
    char _name[128];
    unsigned int namelen = strlen(name);

    strncpy(_name, "xseg_", 5);
    strncpy(_name + 5, name, 80);
    strncpy(_name + 5 + namelen, ".so", 3);
    _name[5 + namelen + 3] = 0;
    dl = dlopen(_name, RTLD_NOW);
    if (!dl) {
        XSEGLOG("Cannot load plugin '%s': %s\n", _name, dlerror());
        return;
    }

    strncpy(_name + 5 + namelen, "_init", 5);
    _name[127] = 0;
    init = (void (*)(void)) (long) dlsym(dl, _name);
    if (!init) {
        XSEGLOG("Init function '%s' not found!\n", _name);
        return;
    }

    init();
    //XSEGLOG("Plugin '%s' loaded.\n", name);
}

uint64_t __get_id(void)
{
    return (uint64_t) syscall(SYS_gettid);
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

void __get_current_time(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

int __renew_logctx(struct log_ctx *lc, char *peer_name,
                   enum log_level log_level, char *logfile, uint32_t flags)
{
    int fd, tmp_fd;

    if (peer_name) {
        strncpy(lc->peer_name, peer_name, MAX_PEER_NAME);
        lc->peer_name[MAX_PEER_NAME - 1] = 0;
    }

    lc->log_level = log_level;
    if (logfile && logfile[0]) {
        strncpy(lc->filename, logfile, MAX_LOGFILE_LEN);
        lc->filename[MAX_LOGFILE_LEN - 1] = 0;
    } else if (!(flags & REOPEN_FILE) || lc->logfile == STDERR_FILENO)
        return 0;

    fd = open(lc->filename, O_WRONLY | O_CREAT | O_APPEND,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 0) {
        return -1;
    }

    tmp_fd = lc->logfile;
    lc->logfile = fd;
    close(tmp_fd);

    flags &= ~REOPEN_FILE;
    if ((flags | lc->flags) & REDIRECT_STDOUT) {
        fd = dup2(lc->logfile, STDOUT_FILENO);
        if (fd < 0)
            return -1;
    }
    if ((flags | lc->flags) & REDIRECT_STDERR) {
        fd = dup2(lc->logfile, STDERR_FILENO);
        if (fd < 0)
            return -1;
    }
    lc->flags |= flags;

    return 0;
}

int (*renew_logctx) (struct log_ctx * lc, char *peer_name,
                     enum log_level log_level, char *logfile, uint32_t flags) =
    __renew_logctx;

int __init_logctx(struct log_ctx *lc, char *peer_name,
                  enum log_level log_level, char *logfile, uint32_t flags)
{
    int fd;

    if (peer_name) {
        strncpy(lc->peer_name, peer_name, MAX_PEER_NAME);
        lc->peer_name[MAX_PEER_NAME - 1] = 0;
    } else {
        return -1;
    }

    /* set logfile to stderr by default */
    lc->logfile = STDERR_FILENO;
#if 0
    /* duplicate stdout, stderr */
    fd = dup(STDOUT_FILENO);
    if (fd < 0) {
        return -1;
    }
    lc->stdout_orig = fd;

    fd = dup(STDERR_FILENO);
    if (fd < 0) {
        return -1;
    }
    lc->stderr_orig = fd;
#endif
    lc->log_level = log_level;
    if (!logfile || !logfile[0]) {
//              lc->logfile = lc->stderr_orig;
        return 0;
    }

    strncpy(lc->filename, logfile, MAX_LOGFILE_LEN);
    lc->filename[MAX_LOGFILE_LEN - 1] = 0;
    fd = open(lc->filename, O_WRONLY | O_CREAT | O_APPEND,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (fd < 1) {
//              lc->logfile = lc->stderr_orig;
        return -1;
    }
    lc->logfile = fd;

    if (flags & REDIRECT_STDOUT) {
        fd = dup2(lc->logfile, STDOUT_FILENO);
        if (fd < 0)
            return -1;
    }
    if (flags & REDIRECT_STDERR) {
        fd = dup2(lc->logfile, STDERR_FILENO);
        if (fd < 0)
            return -1;
    }
    lc->flags = flags;

    return 0;
}

int (*init_logctx) (struct log_ctx * lc, char *peer_name,
                    enum log_level log_level, char *logfile, uint32_t flags) =
    __init_logctx;

void __xseg_log2(struct log_ctx *lc, enum log_level level, char *fmt, ...)
{
    va_list ap;
    time_t timeval;
    char timebuf[1024], buffer[4096];
    char *buf = buffer;
    char *t = NULL, *pn = NULL;
    ssize_t r, sum;
    size_t count;
    int fd;

    va_start(ap, fmt);
    switch (level) {
    case E:
        t = "XSEG[EE]";
        break;
    case W:
        t = "XSEG[WW]";
        break;
    case I:
        t = "XSEG[II]";
        break;
    case D:
        t = "XSEG[DD]";
        break;
    default:
        t = "XSEG[UNKNONW]";
        break;
    }
    pn = lc->peer_name;
    if (!pn)
        pn = "Invalid peer name";

    time(&timeval);
    ctime_r(&timeval, timebuf);
    *strchr(timebuf, '\n') = '\0';

    buf += sprintf(buf, "%s: ", t);
    buf += snprintf(buf, MAX_PEER_NAME + 2, "%s: ", lc->peer_name);
    buf += sprintf(buf, "%s (%ld):\n\t", timebuf, timeval);
    unsigned long rem = sizeof(buffer) - (buf - buffer);
    buf += vsnprintf(buf, rem, fmt, ap);
    if (buf >= buffer + sizeof(buffer)) {
        buf = buffer + sizeof(buffer) - 2;      /* enough to hold \n and \0 */
    }
    buf += sprintf(buf, "\n");

    count = buf - buffer;
    sum = 0;
    r = 0;
    fd = *(volatile int *) &lc->logfile;
    do {
        r = write(fd, buffer + sum, count - sum);
        if (r < 0) {
            if (errno == EBADF)
                fd = *(volatile int *) &lc->logfile;
            else {
                //XSEGLOG("Error while writing log");
                break;
            }
        } else {
            sum += r;
        }
    } while (sum < count);
    /* No need to check for error */
    //fsync(fd);
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
    for (i = 0; i < size; ++i) {
        XSEGLOG("\t%s", bt[i]);
    }
}
