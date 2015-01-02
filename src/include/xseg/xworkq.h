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

#ifndef __X_WORK_H
#define __X_WORK_H

#include <xseg/xq.h>
#include <xseg/xwork.h>

struct xworkq {
    struct xlock q_lock;
    uint32_t flags;
    struct xq *q;
    struct xlock *lock;
};

int xworkq_init(struct xworkq *wq, struct xlock *lock, uint32_t flags);
int xworkq_enqueue(struct xworkq *wq, void (*job_fn) (void *q, void *arg),
                   void *job);
void xworkq_signal(struct xworkq *wq);
void xworkq_destroy(struct xworkq *wq);



#endif                          /* __X_WORK_H */
