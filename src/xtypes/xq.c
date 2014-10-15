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

#include <xseg/xq.h>
#include <xseg/xtypes.h>
#include <string.h>

static inline int __snap(xqindex size)
{
	if (!size)
		return 0;
	return 1 << ((sizeof(size) * 8) - __builtin_clz(size) - 1);
}

void xq_free(struct xq *xq) {
	xtypes_free((void *)XPTR(&xq->queue));
	memset(xq, 0, sizeof(struct xq));
}

void xq_init_empty(struct xq *xq, xqindex size, void *mem)
{
	xq->head = 1;
	xq->tail = 0;
	XPTRSET(&xq->queue, mem);
	xq->size = __snap(size);
	xlock_release(&xq->lock);
}

void xq_init_map(struct xq *xq,
		 xqindex size,
		 xqindex count,
		 xqindex (*mapfn)(xqindex),
		 void *mem)
{
	xqindex t, *qmem = mem;
	xq->size = __snap(size);
	if (count > xq->size)
		count = xq->size;
	xq->head = count + 1;
	xq->tail = 0;
	XPTRSET(&xq->queue, qmem);
	for (t = 0; t < count; t++)
		qmem[t] = mapfn(t);
	xlock_release(&xq->lock);
}

void xq_init_seq(struct xq *xq, xqindex size, xqindex count, void *mem)
{
	xqindex t, *qmem = mem;
	xq->size = __snap(size);
	if (count > xq->size)
		count = xq->size;
	xq->head = count + 1;
	xq->tail = 0;
	XPTRSET(&xq->queue, qmem);
	for (t = 0; t < count; t++)
		qmem[t] = t;
	xlock_release(&xq->lock);
}

xqindex *xq_alloc_empty(struct xq *xq, xqindex size)
{
	xqindex *mem = xtypes_malloc(size * sizeof(xqindex));
	if (!mem)
		return mem;
	xq_init_empty(xq, size, mem);
	return mem;
}

xqindex *xq_alloc_map(struct xq *xq,
			xqindex size,
			xqindex count,
			xqindex (*mapfn)(xqindex)	)
{
	xqindex *mem = xtypes_malloc(size * sizeof(xqindex));
	if (!mem)
		return mem;
	xq_init_map(xq, size, count, mapfn, mem);
	return mem;
}

xqindex *xq_alloc_seq(struct xq *xq, xqindex size, xqindex count)
{
	xqindex *mem = xtypes_malloc(size * sizeof(xqindex));
	if (!mem)
		return mem;
	xq_init_seq(xq, size, count, mem);
	return mem;
}

xqindex xq_size(struct xq *xq)
{
	return xq->size;
}

xqindex xq_count(struct xq *xq)
{
	return xq->head - xq->tail - 1;
}

xqindex xq_element(struct xq *xq, xqindex index)
{
	return XPTR(&xq->queue)[index & (xq->size - 1)];
}

void xq_print(struct xq *xq)
{
	xqindex i;

	XSEGLOG("xq head: %lu, tail: %lu, size: %lu\n",
		(unsigned long)xq->head,
		(unsigned long)xq->tail,
		(unsigned long)xq->size);
	i = xq->tail + 1;

	for (;;) {
		if (i == xq->head)
			break;
		XSEGLOG(	"%lu %lu\n",
			(unsigned long)i,
			(unsigned long)xq_element(xq, i) );
		i += 1;
	}
}

xqindex __xq_append_head_idx(struct xq *xq, xqindex nr)
{
	xqindex head = xq->head;
	xq->head = head + nr;
	return head;
}

/*
xqindex xq_append_heads(struct xq *xq,
			xqindex nr,
			xqindex *heads)
{
	xqindex i, mask, head;
	xqindex serial = xlock_acquire(&xq->lock);

	if (!(xq_count(xq) + nr <= xq->size)) {
		serial = Noneidx;
		goto out;
	}

	mask = xq->size -1;
	head = __xq_append_head_idx(xq, nr);
	for (i = 0; i < nr; i++)
		XPTR(&xq->queue)[(head + i) & mask] = heads[i];
out:
	xlock_release(&xq->lock);
	return serial;
}
*/

xqindex __xq_append_head(struct xq *xq, xqindex xqi)
{
	if (xq_count(xq) >= xq->size) {
		return Noneidx;
	}
	XPTR(&xq->queue)[__xq_append_head_idx(xq, 1) & (xq->size -1)] = xqi;
	return xqi;

}
xqindex xq_append_head(struct xq *xq, xqindex xqi)
{
	xqindex serial;
	xlock_acquire(&xq->lock);
	serial = __xq_append_head(xq, xqi);
	xlock_release(&xq->lock);
	return serial;
}

xqindex __xq_pop_head_idx(struct xq *xq, xqindex nr)
{
	xqindex head = xq->head - nr;
	xq->head = head;
	return head;
}

/*
xqindex xq_pop_heads(struct xq *xq,
			xqindex nr,
			xqindex *heads)
{
	xqindex i, mask, head;
	xqindex serial = xlock_acquire(&xq->lock);

	if (xq_count(xq) < nr) {
		serial = Noneidx;
		goto out;
	}

	mask = xq->size -1;
	head = __xq_pop_head_idx(xq, nr);
	for (i = 0; i < nr; i++)
		heads[i] = XPTR(&xq->queue)[(head - i) & mask];
out:
	xlock_release(&xq->lock);
	return serial;
}
*/

xqindex __xq_pop_head(struct xq *xq)
{
	xqindex value = Noneidx;
	if (!xq_count(xq))
		return value;
	return XPTR(&xq->queue)[__xq_pop_head_idx(xq, 1) & (xq->size -1)];
}

xqindex xq_pop_head(struct xq *xq)
{
	xqindex value = Noneidx;
	(void)xlock_acquire(&xq->lock);
	value = __xq_pop_head(xq);
	xlock_release(&xq->lock);
	return value;
}

xqindex __xq_peek_head_idx(struct xq *xq, xqindex nr)
{
	xqindex head = xq->head - nr;
	return head;
}

xqindex __xq_peek_head(struct xq *xq)
{
	if (!xq_count(xq))
		return Noneidx;
	return XPTR(&xq->queue)[__xq_peek_head_idx(xq, 1) & (xq->size -1)];
}

xqindex xq_peek_head(struct xq *xq)
{
	xqindex value;
	(void)xlock_acquire(&xq->lock);
	value = __xq_peek_head(xq);
	xlock_release(&xq->lock);
	return value;
}

xqindex __xq_peek_tail_idx(struct xq *xq, xqindex nr)
{
	xqindex tail = xq->tail + nr;
	return tail;
}

xqindex __xq_peek_tail(struct xq *xq)
{
	if (!xq_count(xq))
		return Noneidx;
	return XPTR(&xq->queue)[__xq_peek_tail_idx(xq, 1) & (xq->size -1)];
}

xqindex xq_peek_tail(struct xq *xq)
{
	xqindex value;
	(void)xlock_acquire(&xq->lock);
	value = __xq_peek_tail(xq);
	xlock_release(&xq->lock);
	return value;
}

xqindex __xq_append_tail_idx(struct xq *xq, xqindex nr)
{
	xqindex tail = xq->tail - nr;
	xq->tail = tail;
	return tail + 1;
}

/*
xqindex xq_append_tails(struct xq *xq,
			xqindex nr,
			xqindex *tails)
{
	xqindex i, mask, tail;
	xqindex serial = xlock_acquire(&xq->lock);

	if (!(xq_count(xq) + nr <= xq->size)) {
		serial = Noneidx;
		goto out;
	}

	mask = xq->size -1;
	tail = __xq_append_tail_idx(xq, nr) + nr -1;
	for (i = 0; i < nr; i++)
		XPTR(&xq->queue)[(tail - i) & mask] = tails[i];
out:
	xlock_release(&xq->lock);
	return serial;
}
*/

xqindex __xq_append_tail(struct xq *xq, xqindex xqi)
{
	if (!(xq_count(xq) + 1 <= xq->size)) {
		return Noneidx;
	}
	XPTR(&xq->queue)[__xq_append_tail_idx(xq, 1) & (xq->size -1)] = xqi;
	return xqi;
}

xqindex xq_append_tail(struct xq *xq, xqindex xqi)
{
	xqindex serial = Noneidx;
	xlock_acquire(&xq->lock);
	serial =__xq_append_tail(xq, xqi);
	xlock_release(&xq->lock);
	return serial;
}

xqindex __xq_pop_tail_idx(struct xq *xq, xqindex nr)
{
	xqindex tail = xq->tail;
	xq->tail = tail + nr;
	return tail +1;
}

/*
xqindex xq_pop_tails(struct xq *xq, xqindex nr, xqindex *tails)
{
	xqindex i, mask, tail;
	xqindex serial = xlock_acquire(&xq->lock);

	if (xq_count(xq) < nr) {
		serial = Noneidx;
		goto out;
	}

	mask = xq->size -1;
	tail = __xq_pop_tail_idx(xq, nr);
	for (i = 0; i < nr; i++)
		tails[i] = XPTR(&xq->queue)[(tail + i) & mask];
out:
	xlock_release(&xq->lock);
	return serial;
}
*/

xqindex __xq_pop_tail(struct xq *xq)
{
	if (!xq_count(xq))
		return Noneidx;
	return XPTR(&xq->queue)[__xq_pop_tail_idx(xq, 1) & (xq->size -1)];
}

xqindex xq_pop_tail(struct xq *xq)
{
	xqindex value;
	(void)xlock_acquire(&xq->lock);
	value = __xq_pop_tail(xq);
	xlock_release(&xq->lock);
	return value;
}

int xq_head_to_tail(struct xq *headq, struct xq *tailq, xqindex nr)
{
	xqindex head, tail, hmask, tmask, *hq, *tq, i, ret = -1;

	if (headq >= tailq) {
		xlock_acquire(&headq->lock);
		xlock_acquire(&tailq->lock);
	} else {
		xlock_acquire(&tailq->lock);
		xlock_acquire(&headq->lock);
	}

	if (xq_count(headq) < nr || xq_count(tailq) + nr > tailq->size)
		goto out;

	hmask = headq->size -1;
	tmask = tailq->size -1;
	head = __xq_pop_head_idx(headq, nr);
	tail = __xq_append_tail_idx(tailq, nr);
	hq = XPTR(&headq->queue);
	tq = XPTR(&tailq->queue);

	for (i = 0; i < nr; i++)
		tq[(tail + i) & tmask] = hq[(head + i) & hmask];

	ret = 0;
out:
	xlock_release(&headq->lock);
	xlock_release(&tailq->lock);
	return ret;
}

int __xq_check(struct xq *xq, xqindex idx)
{
	xqindex i, val;
	for (i = xq->tail + 1; i != xq->head ; i++) {
		val = XPTR(&xq->queue)[i & (xq->size -1)];
		if (val == idx)
			return 1;
	}
	return 0;
}

int xq_check(struct xq *xq, xqindex idx)
{
	int r;
	xlock_acquire(&xq->lock);
	r = __xq_check(xq, idx);
	xlock_release(&xq->lock);
	return r;
}

xqindex __xq_resize(struct xq *xq, struct xq *newxq)
{
	xqindex i, mask, mask_new, head, tail, val;
	xqindex nr = xq_count(xq);

	if (nr > newxq->size) {
		return Noneidx;
	}

	mask = xq->size -1;
	mask_new = newxq->size -1;
	head = __xq_peek_head_idx(xq, nr);
	tail = __xq_append_tail_idx(newxq, nr) + nr -1;
	for (i = 0; i < nr; i++) {
		val = XPTR(&xq->queue)[(head + i) & mask];
		XPTR(&newxq->queue)[(tail - i) & mask_new] = val;
	}

	return nr;
}

xqindex xq_resize(struct xq *xq, struct xq *newxq)
{
	xqindex r = Noneidx;
	xlock_acquire(&xq->lock);
	xlock_acquire(&newxq->lock);
	r = __xq_resize(xq, newxq);
	xlock_release(&newxq->lock);
	xlock_release(&xq->lock);
	return r;
}

#ifdef __KERNEL__
#include <linux/module.h>
#include <xtypes/xq_exports.h>
#endif

