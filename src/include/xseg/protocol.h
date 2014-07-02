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

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/*
 * Reply structures.
 * Every X_OP returns a corresponding xseg_reply_op struct
 * for structured replies. See <xseg/xseg.h> for the list of ops.
 */
struct xseg_reply_info {
	uint64_t size;
};

#define XSEG_MAX_TARGETLEN 256

#if (XSEG_MAX_TARGETLEN < 64)
#warning "XSEG_MAX_TARGETLEN should be at least 64!"
#undef XSEG_MAX_TARGETLEN
#define XSEG_MAX_TARGETLEN 64
#endif

#define XF_MAPFLAG_READONLY (1 << 0)
#define XF_MAPFLAG_ZERO     (1 << 1)

struct xseg_reply_map_scatterlist {
	char target[XSEG_MAX_TARGETLEN];
	uint32_t flags;
	uint32_t targetlen;
	uint64_t offset;
	uint64_t size;
};

struct xseg_reply_map {
	uint32_t cnt;
	struct xseg_reply_map_scatterlist segs[];
};

struct xseg_create_map_scatterlist {
	char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
	uint32_t flags;
};

struct xseg_request_create {
	uint32_t cnt;
	uint32_t blocksize;
	uint32_t create_flags;
	struct xseg_create_map_scatterlist segs[];
};

struct xseg_request_clone {
        char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
        uint64_t size;
};

struct xseg_request_copy {
        char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
};

struct xseg_request_snapshot {
        char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
};

struct xseg_reply_hash {
	char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
};

struct xseg_request_rename {
        char target[XSEG_MAX_TARGETLEN];
	uint32_t targetlen;
};
#endif
