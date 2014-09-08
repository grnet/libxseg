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


#include <xseg/xbinheap.h>

//TODO, container aware. use xptr
//add resize capability
//add custom compare functions

#define SWAP(_a_, _b_)				\
	do {					\
		typeof(_a_) __temp__ = _a_; 	\
		_a_ = _b_; 			\
		_b_ = __temp__ ; 		\
	} while (0)

static inline void swap_nodes(struct xbinheap *h, xbinheapidx a, xbinheapidx b)
{
	xbinheapidx h1, h2;
	h1 = h->nodes[a].h;
	h2 = h->nodes[b].h;
	//XSEGLOG("Swaping %llu, %llu", a, b);
	SWAP(h->nodes[a].key, h->nodes[b].key);
	SWAP(h->nodes[a].value, h->nodes[b].value);
	SWAP(h->nodes[a].h, h->nodes[b].h);
	SWAP(h->indexes[h1], h->indexes[h2]);
	//XSEGLOG("Index[%llu]: %llu, Index[%llu]: %lli", a, h->indexes[a], b, h->indexes[b]);
}

static inline int isMaxHeap(struct xbinheap *h)
{
	return (h->flags & XBINHEAP_MAX);
}

static int heapify_up(struct xbinheap *h, xbinheapidx i)
{
	xbinheapidx parent;
	struct xbinheap_node *n, *pn;
	int cmp;
	if (!i)
		return 0;
	parent = (i-1)/2;
	n = &h->nodes[i];
	pn = &h->nodes[parent];
	//XSEGLOG("i: %llu, p: %llu, count: %lu", n->key, pn->key, h->count);
	if (isMaxHeap(h)){
		cmp = pn->key < n->key;
	} else {
		cmp = pn->key > n->key;
	}
	if (cmp){
		swap_nodes(h, i, parent);
		return heapify_up(h, parent);
	}
	return 0;
}

static int heapify_down(struct xbinheap *h, xbinheapidx i)
{
	xbinheapidx left, right, largest;
	struct xbinheap_node *n, *ln, *rn, *largest_n;
	left = 2*i + 1;
	right = 2*i + 1 + 1;
	largest = i;
	(void)n;
	//n = &h->nodes[i];
	ln = &h->nodes[left];
	rn = &h->nodes[right];
	largest_n = &h->nodes[largest];
	if (isMaxHeap(h)){
	//	XSEGLOG("l: %llu, r: %llu, p: %llu, count: %lu", ln->key, rn->key, largest_n->key, h->count);
		if (left < h->count && (ln->key > largest_n->key)){
			largest = left;
			largest_n = &h->nodes[largest];
		}
		if (right < h->count && (rn->key > largest_n->key)){
			largest = right;
			largest_n = &h->nodes[largest];
		}
		if (largest != i){
			swap_nodes(h, i, largest);
			return heapify_down(h, largest);
		}
	} else {
		if (left < h->count && ln->key < largest_n->key){
			largest = left;
			largest_n = &h->nodes[largest];
		}
		if (right < h->count && rn->key < largest_n->key){
			largest = right;
			largest_n = &h->nodes[largest];
		}
		if (largest != i){
			swap_nodes(h, i, largest);
			return heapify_down(h, largest);
		}
	}
	return 0;
}

xbinheap_handler xbinheap_insert(struct xbinheap *h, xbinheapidx key,
		xbinheapidx value)
{
	xbinheap_handler ret;
	if (h->count + 1 > h->size)
		return NoNode;
	ret = h->nodes[h->count].h;
	h->nodes[h->count].key = key;
	h->nodes[h->count].value = value;
	//h->indexes[h->count] = h->count;
	heapify_up(h, h->count);
	h->count++;
	return ret;
}

int xbinheap_empty(struct xbinheap *h)
{
	return (h->count == 0);
}

/* peek min or max */
xbinheapidx xbinheap_peak(struct xbinheap *h)
{
	if (xbinheap_empty(h))
		return NoNode;
	return h->nodes[0].value;
}

/* extract min or max */
xbinheapidx xbinheap_extract(struct xbinheap *h)
{
	xbinheapidx ret = h->nodes[0].value;
	if (xbinheap_empty(h))
		return NoNode;
	h->count--;
	swap_nodes(h, 0, h->count);
	heapify_down(h, 0);
	return ret;
}

int xbinheap_increasekey(struct xbinheap *h, xbinheap_handler idx,
		xbinheapidx newkey)
{
	int r;
	xbinheapidx i = h->indexes[idx];
	struct xbinheap_node *hn = &h->nodes[i];
	//XSEGLOG("h: %llu, index: %llu, key: %llu, newkey:%llu",
	//		idx, i, hn->key, newkey);
	//assert newkey > hn->key
	hn->key = newkey;
	if (isMaxHeap(h)){
		r = heapify_up(h, i);
	} else {
		r = heapify_down(h, i);
	}
	return r;
}

xbinheapidx xbinheap_getkey(struct xbinheap *h, xbinheap_handler idx)
{
	xbinheapidx i = h->indexes[idx];
	if (i > h->count)
		return NoNode;
	return h->nodes[i].key;
}

int xbinheap_init(struct xbinheap *h, xbinheapidx size, uint32_t flags, void *mem)
{
	xbinheapidx i;
	if (!mem){
		h->indexes = xtypes_malloc(sizeof(xbinheapidx) * size);
		h->nodes = xtypes_malloc(sizeof(struct xbinheap_node) * size);
		if (!h->indexes || !h->nodes){
			xtypes_free(h->indexes);
			xtypes_free(h->nodes);
			return -1;
		}
	} else {
		h->indexes = mem;
		h->nodes = (void *)((unsigned long)mem + sizeof(xbinheapidx) * size);
	}
	for (i = 0; i < size; i++) {
		h->nodes[i].h = i;
		h->indexes[i] = i;
	}
	h->flags = flags;
	h->size = size;
	h->count = 0;
	return 0;
}

void xbinheap_free(struct xbinheap *h)
{
	xtypes_free(h->indexes);
	xtypes_free(h->nodes);
}


int xbinheap_decreasekey(struct xbinheap *h, xbinheap_handler idx,
   		xbinheapidx newkey)
{
	int r;
	xbinheapidx i = h->indexes[idx];
	struct xbinheap_node *hn = &h->nodes[i];
	//XSEGLOG("h: %llu, index: %llu, key: %llu, newkey:%llu",
	//		idx, i, hn->key, newkey);
	//assert newkey > hn->key
	hn->key = newkey;
	if (isMaxHeap(h)){
		r = heapify_down(h, i);
	} else {
		r = heapify_up(h, i);
	}
	return r;
}

#ifdef __KERNEL__
#include <linux/module.h>
#include <xtypes/xbinheap_exports.h>
#endif 
