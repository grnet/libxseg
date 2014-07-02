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

#ifndef __X_WAITQ_H
#define __X_WAITQ_H

#include <xseg/xq.h>
#include <xseg/xwork.h>

#define XWAIT_SIGNAL_ONE (1 << 0)

struct xwaitq {
	int (*cond_fn)(void *arg);
	void *cond_arg;
	uint32_t flags;
	struct xq *q;
	struct xlock lock;
};

int xwaitq_init(struct xwaitq *wq, int (*cond_fn)(void *arg), void *arg, uint32_t flags);
int xwaitq_enqueue(struct xwaitq *wq, struct work *w);
void xwaitq_signal(struct xwaitq *wq);
void xwaitq_destroy(struct xwaitq *wq);



#endif /* __X_WAITQ_H */
