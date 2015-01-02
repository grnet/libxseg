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
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <xseg/util.h>
#include <xseg/xseg.h>
#include <xseg/xobj.h>
#include <xseg_posixfd.h>

#define FIFO_RDWR_UGO	S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

static struct posixfd_signal_desc *__get_signal_desc(struct xseg *xseg,
                                                     xport portno)
{
    struct xseg_port *port = xseg_get_port(xseg, portno);
    if (!port) {
        return NULL;
    }
    struct posixfd_signal_desc *psd = xseg_get_signal_desc(xseg, port);
    if (!psd) {
        return NULL;
    }
    return psd;
}

static void __get_filename(struct posixfd_signal_desc *psd, char *filename)
{
    int pos = 0;
    strncpy(filename + pos, POSIXFD_DIR, POSIXFD_DIR_LEN);
    pos += POSIXFD_DIR_LEN;
    strncpy(filename + pos, psd->signal_file, POSIXFD_FILENAME_LEN);
    pos += POSIXFD_FILENAME_LEN;
    filename[pos] = 0;
}

/*
 * In order to be able to accept signals we must:
 *
 * a) Create the name piped for our signal descriptor.
 * b) Open the named pipe and get an fd.
 */
static int posixfd_local_signal_init(struct xseg *xseg, xport portno)
{
    /* create or truncate POSIXFD+portno file */
    int fd;
    char filename[POSIXFD_DIR_LEN + POSIXFD_FILENAME_LEN + 1];

    struct posixfd_signal_desc *psd = __get_signal_desc(xseg, portno);
    if (!psd) {
        return -1;
    }
    __get_filename(psd, filename);


    if (mkfifo(filename, FIFO_RDWR_UGO) < 0 && errno != EEXIST) {
        return -1;
    }

    /* Ensure that the count of connected readers/writers is above zero */
    if ((fd = open(filename, O_RDWR | O_NONBLOCK)) < 0) {
        unlink(filename);
        return -1;
    }
    psd->fd = fd;
    return 0;
}

/*
 * To clean up after our signal initialiazation, we should:
 *
 * a) close the open fd for our named pipe
 * b) unlink the named pipe from the file system.
 */
static void posixfd_local_signal_quit(struct xseg *xseg, xport portno)
{
    char filename[POSIXFD_DIR_LEN + POSIXFD_FILENAME_LEN + 1];
    struct posixfd_signal_desc *psd = __get_signal_desc(xseg, portno);

    if (psd->fd >= 0) {
        close(psd->fd);
        psd->fd = -1;
    }

    __get_filename(psd, filename);
    unlink(filename);
    return;
}

/*
 * When this peer type is initialized, we must make sure the directory where the
 * named pipes will be created, exist. Also make sure that th setgid bit is set.
 */
static int posixfd_remote_signal_init(void)
{
    int r;
    struct stat st;

    r = stat(POSIXFD_DIR, &st);
    if (r < 0) {
        return -1;
    }

    if (!S_ISDIR(st.st_mode)) {
        return -1;
    }

    if (st.st_mode & S_ISGID) {
        return 0;
    }

    r = chmod(POSIXFD_DIR, st.st_mode | S_ISGID);
    if (r < 0) {
        return -1;
    }

    return 0;
}

static void posixfd_remote_signal_quit(void)
{
    return;
}

static int posixfd_prepare_wait(struct xseg *xseg, uint32_t portno)
{
    char buf[512];
    size_t len;
    int value = 0;

    struct posixfd_signal_desc *psd = __get_signal_desc(xseg, portno);
    if (!psd) {
        return -1;
    }
    psd->flag = 1;

    /* Drain the pipe */
    do {
        len = read(psd->fd, buf, sizeof(buf));
        value |= (len > 0 ? 0 : -1);
    } while ((len == -1 && errno == EINTR) || len == sizeof(buf));

    return value;
}

static int posixfd_cancel_wait(struct xseg *xseg, uint32_t portno)
{
    char buf[512];
    size_t len;
    int value = 0;

    struct posixfd_signal_desc *psd = __get_signal_desc(xseg, portno);
    if (!psd) {
        return -1;
    }
    psd->flag = 0;

    /* Drain the pipe */
    do {
        len = read(psd->fd, buf, sizeof(buf));
        value |= (len > 0 ? 0 : -1);
    } while ((len == -1 && errno == EINTR) || len == sizeof(buf));

    return value;
}

/*
 * To wait a signal, the posixfd peer must use select on the fd of its named
 * pipe.
 *
 * When the peer wakes up from the select, if it wasn't waked up because of a
 * timeout, it should read as much as it can from the named pipe to clean it and
 * prepare it for the next select.
 */
static int posixfd_wait_signal(struct xseg *xseg, void *sd,
                               uint32_t usec_timeout)
{
    int r;
    struct timeval tv;
    char buf[512];
    size_t len;
    int value = 0;
    fd_set fds;

    struct posixfd_signal_desc *psd = (struct posixfd_signal_desc *) sd;
    if (!psd) {
        return -1;
    }

    tv.tv_sec = usec_timeout / 1000000;
    tv.tv_usec = usec_timeout - tv.tv_sec * 1000000;

    FD_ZERO(&fds);
    FD_SET(psd->fd, &fds);

    r = select(psd->fd + 1, &fds, NULL, NULL, &tv);
    //XSEGLOG("Tv sec: %ld, tv_usec: %ld", tv.tv_sec, tv.tv_usec);

    if (r < 0) {
        if (errno != EINTR) {
            return -1;
        } else {
            return 0;
        }
    }

    if (r != 0) {
        /* Drain the pipe */
        do {
            len = read(psd->fd, buf, sizeof(buf));
            value |= (len > 0 ? 0 : -1);
        } while ((len == -1 && errno == EINTR) || len == sizeof(buf));
    }

    return value;
}

/*
 * To signal a posixfd peer, we must:
 *
 * a) Check if the peer wants to be signaled.
 * b) Open the named pipe, it provides.
 * c) Write some data to the named pipe, so the peer's fd will be selectable for
 *    writing.
 * d) Close the named pipe.
 */
static int posixfd_signal(struct xseg *xseg, uint32_t portno)
{
    int r, fd;
    /* NULL terminated */
    char filename[POSIXFD_DIR_LEN + POSIXFD_FILENAME_LEN + 1] = POSIXFD_DIR;

    struct posixfd_signal_desc *psd = __get_signal_desc(xseg, portno);
    if (!psd) {
        return -1;
    }

    if (!psd->flag) {
        /* If the peer advises not to signal, we respect it. */
        return 0;
    }
    __get_filename(psd, filename);

    fd = open(filename, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        return -1;
    }

    do {
        r = write(fd, "a", 1);
    } while (r < 0 && errno == EINTR);

    if (r < 0) {
        close(fd);
        return -1;
    }
    /* FIXME what here? */
    r = close(fd);

    return 0;
}

static void *posixfd_malloc(uint64_t size)
{
    return malloc((size_t) size);
}

static void *posixfd_realloc(void *mem, uint64_t size)
{
    return realloc(mem, (size_t) size);
}

static void posixfd_mfree(void *mem)
{
    free(mem);
}

/* taken from user/hash.c */
static char get_hex(unsigned int h)
{
    switch (h) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        return h + '0';
    case 10:
        return 'a';
    case 11:
        return 'b';
    case 12:
        return 'c';
    case 13:
        return 'd';
    case 14:
        return 'e';
    case 15:
        return 'f';
    }
    /* not reachable */
    return '0';
}

static void hexlify(unsigned char *data, long datalen, char *hex)
{
    long i;
    for (i = 0; i < datalen; i++) {
        hex[2 * i] = get_hex((data[i] & 0xF0) >> 4);
        hex[2 * i + 1] = get_hex(data[i] & 0x0F);
    }
}



int posixfd_init_signal_desc(struct xseg *xseg, void *sd)
{
    struct posixfd_signal_desc *psd = sd;
    if (!psd) {
        return -1;
    }
    psd->signal_file[0] = 0;
    /* POSIXFD_FILENAME_LEN = 2 * sizeof(void *) */
    hexlify((unsigned char *) &sd, POSIXFD_FILENAME_LEN / 2, psd->signal_file);
    psd->flag = 0;
    psd->fd = -1;

    return 0;
}

void posixfd_quit_signal_desc(struct xseg *xseg, void *sd)
{
    return;
}

void *posixfd_alloc_data(struct xseg *xseg)
{
    struct xobject_h *sd_h = xseg_get_objh(xseg, MAGIC_POSIX_SD,
                                           sizeof(struct posixfd_signal_desc));
    return sd_h;
}

void posixfd_free_data(struct xseg *xseg, void *data)
{
    if (data) {
        xseg_put_objh(xseg, (struct xobject_h *) data);
    }
    return;
}

void *posixfd_alloc_signal_desc(struct xseg *xseg, void *data)
{
    struct xobject_h *sd_h = (struct xobject_h *) data;
    if (!sd_h) {
        return NULL;
    }
    struct posixfd_signal_desc *psd = xobj_get_obj(sd_h, X_ALLOC);
    if (!psd) {
        return NULL;
    }
    return psd;

}

void posixfd_free_signal_desc(struct xseg *xseg, void *data, void *sd)
{
    struct xobject_h *sd_h = (struct xobject_h *) data;
    if (!sd_h) {
        return;
    }
    if (sd) {
        xobj_put_obj(sd_h, sd);
    }
    return;
}

static struct xseg_peer xseg_peer_posixfd = {
    /* xseg_peer_operations */
    {
     .init_signal_desc      = posixfd_init_signal_desc,
     .quit_signal_desc      = posixfd_quit_signal_desc,
     .alloc_data            = posixfd_alloc_data,
     .free_data             = posixfd_free_data,
     .alloc_signal_desc     = posixfd_alloc_signal_desc,
     .free_signal_desc      = posixfd_free_signal_desc,
     .local_signal_init     = posixfd_local_signal_init,
     .local_signal_quit     = posixfd_local_signal_quit,
     .remote_signal_init    = posixfd_remote_signal_init,
     .remote_signal_quit    = posixfd_remote_signal_quit,
     .prepare_wait          = posixfd_prepare_wait,
     .cancel_wait           = posixfd_cancel_wait,
     .wait_signal           = posixfd_wait_signal,
     .signal                = posixfd_signal,
     .malloc                = posixfd_malloc,
     .realloc               = posixfd_realloc,
     .mfree                 = posixfd_mfree,
     },
    /* name */
    "posixfd"
};

void xseg_posixfd_init(void)
{
    xseg_register_peer(&xseg_peer_posixfd);
}
