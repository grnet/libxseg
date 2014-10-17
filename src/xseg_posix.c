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
#include <xseg_posix.h>
#define ERRSIZE 512
char errbuf[ERRSIZE];

static int posix_allocate(const char *name, uint64_t size)
{
	int ret = 0;
	int fd;
	int err_no = 0;

	fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0770);
	if (fd < 0) {
		err_no = errno;
		XSEGLOG("Cannot create shared segment: %s\n",
			strerror_r(errno, errbuf, ERRSIZE));
		ret = fd;
		goto exit;
	}

	if (ftruncate(fd, size) != 0) {
		err_no = errno;
		close(fd);
		XSEGLOG("Cannot seek into segment file: %s\n",
			strerror_r(errno, errbuf, ERRSIZE));
		ret = -1;
		goto exit;
	}

	close(fd);

exit:
	errno = err_no;
	return ret;
}

static int posix_deallocate(const char *name)
{
	return shm_unlink(name);
}

static void *posix_map(const char *name, uint64_t size, struct xseg *seg)
{
	struct xseg *xseg;
	int fd, err_no = 0;

	fd = shm_open(name, O_RDWR, 0000);
	if (fd < 0) {
		err_no = errno;
		XSEGLOG("Failed to open '%s' for mapping: %s\n",
			name, strerror_r(errno, errbuf, ERRSIZE));
		errno = err_no;
		return NULL;
	}

	xseg = mmap (	XSEG_BASE_AS_PTR,
			size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_FIXED /* | MAP_LOCKED */,
			fd, 0	);

	if (xseg == MAP_FAILED) {
		err_no = errno;
		XSEGLOG("Could not map segment: %s\n",
			strerror_r(errno, errbuf, ERRSIZE));
		errno = err_no;
		return NULL;
	}

	close(fd);

	err_no = errno;
	return xseg;
}

static void posix_unmap(void *ptr, uint64_t size)
{
	struct xseg *xseg = ptr;
	(void)munmap(xseg, size);
}


static void handler(int signum)
{
	static unsigned long counter;
	printf("%lu: signal %d: this shouldn't have happened.\n", counter, signum);
	counter ++;
}

static sigset_t savedset, set;
static pid_t pid;

static int posix_local_signal_init(struct xseg *xseg, xport portno)
{
	void (*h)(int);
	int r;
	h = signal(SIGIO, handler);
	if (h == SIG_ERR) {
		return -1;
	}

	sigemptyset(&set);
	sigaddset(&set, SIGIO);

	r = sigprocmask(SIG_BLOCK, &set, &savedset);
	if (r < 0) {
		return -1;
	}

	pid = syscall(SYS_gettid);
	return 0;
}

static void posix_local_signal_quit(struct xseg *xseg, xport portno)
{
	pid = 0;
	signal(SIGIO, SIG_DFL);
	sigprocmask(SIG_SETMASK, &savedset, NULL);
}

static int posix_remote_signal_init(void)
{
	return 0;
}

static void posix_remote_signal_quit(void)
{
	return;
}

static int posix_prepare_wait(struct xseg *xseg, uint32_t portno)
{
	struct posix_signal_desc *psd;
	struct xseg_port *port = xseg_get_port(xseg, portno);
	if (!port) {
		return -1;
	}
	psd = xseg_get_signal_desc(xseg, port);
	if (!psd) {
		return -1;
	}
	psd->waitcue = pid;
	return 0;
}

static int posix_cancel_wait(struct xseg *xseg, uint32_t portno)
{
	struct posix_signal_desc *psd;
	struct xseg_port *port = xseg_get_port(xseg, portno);
	if (!port) {
		return -1;
	}
	psd = xseg_get_signal_desc(xseg, port);
	if (!psd) {
		return -1;
	}
	psd->waitcue = 0;
	return 0;
}

static int posix_wait_signal(struct xseg *xseg, void *sd, uint32_t usec_timeout)
{
	int r;
	siginfo_t siginfo;
	struct timespec ts;

	ts.tv_sec = usec_timeout / 1000000;
	ts.tv_nsec = 1000 * (usec_timeout - ts.tv_sec * 1000000);

	/* FIXME: Now that posix signaling is fixed, we could get rid of the timeout
	 * and use a NULL timespec linux-specific)
	 */
	r = sigtimedwait(&set, &siginfo, &ts);
	if (r < 0) {
		return r;
	}

	return siginfo.si_signo;
}

static int posix_signal(struct xseg *xseg, uint32_t portno)
{
	struct posix_signal_desc *psd;
	struct xseg_port *port = xseg_get_port(xseg, portno);
	if (!port) {
		return -1;
	}
	psd = xseg_get_signal_desc(xseg, port);
	if (!psd) {
		return -1;
	}
	pid_t cue = (pid_t)psd->waitcue;
	if (!cue) {
		//HACKY!
		return -2;
	}

	/* FIXME: Make calls to xseg_signal() check for errors */
	return syscall(SYS_tkill, cue, SIGIO);
}

static void *posix_malloc(uint64_t size)
{
	return malloc((size_t)size);
}

static void *posix_realloc(void *mem, uint64_t size)
{
	return realloc(mem, (size_t)size);
}

static void posix_mfree(void *mem)
{
	free(mem);
}


int posix_init_signal_desc(struct xseg *xseg, void *sd)
{
	struct posix_signal_desc *psd = sd;
	if (!psd) {
		return -1;
	}
	psd->waitcue = 0;
	return 0;
}

void posix_quit_signal_desc(struct xseg *xseg, void *sd)
{
	return;
}

void * posix_alloc_data(struct xseg *xseg)
{
	struct xobject_h *sd_h = xseg_get_objh(xseg, MAGIC_POSIX_SD,
			sizeof(struct posix_signal_desc));
	return sd_h;
}

void posix_free_data(struct xseg *xseg, void *data)
{
	if (data) {
		xseg_put_objh(xseg, (struct xobject_h *)data);
	}
	return;
}

void *posix_alloc_signal_desc(struct xseg *xseg, void *data)
{
	struct posix_signal_desc *psd;
	struct xobject_h *sd_h = (struct xobject_h *) data;
	if (!sd_h) {
		return NULL;
	}
	psd = xobj_get_obj(sd_h, X_ALLOC);
	if (!psd) {
		return NULL;
	}
	psd->waitcue = 0;
	return psd;

}

void posix_free_signal_desc(struct xseg *xseg, void *data, void *sd)
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

static struct xseg_type xseg_posix = {
	/* xseg_operations */
	{
		.mfree		= posix_mfree,
		.allocate	= posix_allocate,
		.deallocate	= posix_deallocate,
		.map		= posix_map,
		.unmap		= posix_unmap,
	},
	/* name */
	"posix"
};

static struct xseg_peer xseg_peer_posix = {
	/* xseg_peer_operations */
	{
		.init_signal_desc   = posix_init_signal_desc,
		.quit_signal_desc   = posix_quit_signal_desc,
		.alloc_data         = posix_alloc_data,
		.free_data          = posix_free_data,
		.alloc_signal_desc  = posix_alloc_signal_desc,
		.free_signal_desc   = posix_free_signal_desc,
		.local_signal_init  = posix_local_signal_init,
		.local_signal_quit  = posix_local_signal_quit,
		.remote_signal_init = posix_remote_signal_init,
		.remote_signal_quit = posix_remote_signal_quit,
		.prepare_wait	    = posix_prepare_wait,
		.cancel_wait	    = posix_cancel_wait,
		.wait_signal	    = posix_wait_signal,
		.signal		    = posix_signal,
		.malloc		    = posix_malloc,
		.realloc 	    = posix_realloc,
		.mfree		    = posix_mfree,
	},
	/* name */
	"posix"
};

void xseg_posix_init(void)
{
	xseg_register_type(&xseg_posix);
	xseg_register_peer(&xseg_peer_posix);
}

