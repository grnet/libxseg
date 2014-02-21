/*
 * Copyright 2012 GRNET S.A. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GRNET S.A. ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GRNET S.A OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and
 * documentation are those of the authors and should not be
 * interpreted as representing official policies, either expressed
 * or implied, of GRNET S.A.
 */

#include <stdio.h>
#include <xseg/xobj.h>
#include <pthread.h>
#include <sys/time.h>

#define OBJ_H_MAGIC 0
#define FOO_OBJ_H_MAGIC 1

struct foo {
	uint64_t id;
	uint64_t bar;
	uint64_t bar1;
	uint64_t bar2;
};

void *mem;
unsigned long size = 1024*1024 * 100;
unsigned long al_unit = 12;
unsigned long nr_threads = 8;
struct xheap *heap;
struct xobject_h obj_h;


unsigned long allocations = 0;

struct thread_arg{
	int id;
	struct xobject_h *obj_h;
	unsigned long c;
	unsigned long gets;
	unsigned long puts;
};

void *thread_test(void *arg)
{
	struct thread_arg *targ = (struct thread_arg *) arg;
	struct xobject_h *obj_h = targ->obj_h;
	int id = targ->id;
	unsigned long c = targ->c;

	struct foo *foo_obj;
	unsigned long i = 0;
	do {
		foo_obj = xobj_get_obj(obj_h, X_ALLOC);
		if (foo_obj != NULL){
			i++;
			memset(foo_obj, 1, sizeof(struct foo));
			if (c) {
				xobj_put_obj(obj_h, foo_obj);
				c--;
			}
		}
	}while (foo_obj != NULL);

	targ->gets = i;
	targ->puts = targ->c - c;
	return NULL;
}

int test_threads()
{
	int i;
	unsigned long gets = 0;
	unsigned long puts = 0;

	int r = xheap_init(heap, size, al_unit, mem);
	if (r < 0){
		printf("threads: xheap init error\n");
		return -1;
	}
	xobj_handler_init(&obj_h, mem, FOO_OBJ_H_MAGIC, sizeof(struct foo), heap);

	struct thread_arg *targs = malloc(sizeof(struct thread_arg) * nr_threads);
	if (!targs) {
		printf("error malloc\n");
		return -1;
	}

	pthread_t *threads = malloc(sizeof(pthread_t) * nr_threads);
	if (!threads){
		printf("error malloc\n");
		return -1;
	}

	for (i = 0; i < nr_threads; i++) {
		targs[i].id = i;
		targs[i].obj_h = &obj_h;
		targs[i].c = 256;
		targs[i].gets = 0;
		targs[i].puts = 0;
	}

	for (i = 0; i < nr_threads; i++) {
		r = pthread_create(&threads[i], NULL, thread_test, &targs[i]);
		if (r) {
			printf("error pthread_create\n");
			return -1;
		}
	}

	for (i = 0; i < nr_threads; i++) {
		pthread_join(threads[i], NULL);
		gets +=  targs[i].gets;
		puts += targs[i].puts;
	}
	if (gets - puts != allocations)
		return -1;
	return 0;
}

int basic_test()
{
	unsigned long c = 0;
	int r;
	struct foo *foo_obj;
	struct timeval start, end;

	r = xheap_init(heap, size, al_unit, mem);
	if (r < 0) {
		printf("error heap_init\n");
		return -1;
	}

	xobj_handler_init(&obj_h, mem, FOO_OBJ_H_MAGIC, sizeof(struct foo), heap);
	gettimeofday(&start, NULL);
	foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//	printf("foo_obj: %lx\n", foo_obj);
	while (foo_obj){
		c++;
		foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//		printf("foo_obj[%lu]: %lx\n", c, foo_obj);
	}
	gettimeofday(&end, NULL);
	allocations = c;
	timersub(&end, &start, &end);
	unsigned long us;
	us = end.tv_sec * 1000000 + end.tv_usec;
	printf("Allocated %lu objects of size %d (Total size: %lu)\n",
			c, sizeof(struct foo), c*sizeof(struct foo));
	printf("Total time: %lu us\n", us);
	unsigned long tpa =  (unsigned long) ((us*1000) / allocations);
	printf("Time per allocation: %lu ns\n", tpa);
	unsigned long aps =  (unsigned long) ((allocations * 1000000)/us);
	printf("Allocations per second: %lu\n\n", aps);
	return 0;
}

int get_put_test()
{
	unsigned long c = 0;
	struct foo *foo_obj;
	int r;	
	void **buf;
	unsigned long i;
	buf = malloc(sizeof(void *) * allocations);
	if (!buf) {
		printf("error malloc\n");
		return -1;
	}
	for (i = 0; i < allocations; i++) {
		buf[i] = NULL;
	}

	r = xheap_init(heap, size, al_unit, mem);
	if (r < 0) {
		printf("error heap_init\n");
		return -1;
	}

	xobj_handler_init(&obj_h, mem, FOO_OBJ_H_MAGIC, sizeof(struct foo), heap);

	foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//	printf("foo_obj: %lx\n", foo_obj);
	c = 0;
	while (foo_obj){
		buf[c] = foo_obj;
		c++;
		memset(foo_obj, 1, sizeof(struct foo));
		foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//		printf("foo_obj[%lu]: %lx\n", c, foo_obj);
	}
	if (c != allocations) {
		printf("allocated %lu instead of expected %lu\n", c, allocations);
		return -1;
	}
	
	c = 0;
	for (i = 0; i < allocations; i++) {
		foo_obj = buf[i];
		if (foo_obj == NULL)
			continue;
		c++;
		xobj_put_obj(&obj_h, foo_obj);
	}
	if (c != allocations) {
		printf("put %lu instead of expected %lu\n", c, allocations);
		return -1;
	}

	foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//	printf("foo_obj: %lx\n", foo_obj);
	c = 0;
	while (foo_obj){
		c++;
		foo_obj = xobj_get_obj(&obj_h, X_ALLOC);
//		printf("foo_obj[%lu]: %lx\n", c, foo_obj);
	}
	if (c != allocations) {
		printf("reallocated %lu instead of expected %lu\n", c, allocations);
		return -1;
	}

	return 0;
}


int main(int argc, const char *argv[])
{
	int r;
	heap = malloc(sizeof(struct xheap));

	mem = malloc(size);
	if (!mem) {
		printf("error malloc\n");
		return -1;
	}

	r = basic_test();
	if (r < 0) 
		printf("Basic test failed\n");
	else
		printf("Basic test completed\n");

	r = get_put_test();
	if (r < 0) 
		printf("Get-Put-Get test failed\n");
	else
		printf("Get-Put-Get test completed\n");

	r = test_threads();
	if (r < 0) 
		printf("Threaded test failed\n");
	else
		printf("Threaded test completed\n");

	return 0;
}
