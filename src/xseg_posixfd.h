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

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#define MAGIC_POSIX_SD 9

/* Must always end with a "/" */
#define POSIXFD_DIR "/dev/shm/posixfd/"
/* Must be the len of POSIXFD_DIR without the \0 */
#define POSIXFD_DIR_LEN 17

struct posixfd_signal_desc {
	/* hexlified value of xport */
	/* FIXME include xseg_types or sth and use sizeof(xport) */
	char signal_file[sizeof(void *)];
	int fd;
	/* whether or not, the port should be signaled */
	int flag;
};

#define POSIXFD_FILENAME_LEN \
		(sizeof(((struct posixfd_signal_desc *)0)->signal_file))
