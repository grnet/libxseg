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

#include <stdlib.h>
#include <string.h>
#include <xseg/xheap.h>
#include <stdint.h>
#include <pthread.h>

struct xheap heap;
uint32_t al_unit = 12;
unsigned long size = 1048576;
unsigned long chunk = 4096;
unsigned long nr_threads = 4;
unsigned long nr_frees;
void *mem;

#define full_size(_x_) ((_x_) + sizeof(struct xheap_header))
void print_ptr(void *ptr)
{
	uint64_t size = xheap_get_chunk_size(ptr);
	printf("ptr: %lx, bytes: %llu (actual: %llu)\n", 
			(unsigned long) ptr, 
			(unsigned long long) size, 
			(unsigned long long) full_size(size));
}

unsigned long test(struct xheap *heap)
{
	int r = xheap_init(heap, size, al_unit, mem);
	if (r < 0){
		printf("xheap init error\n");
		return -1;
	}
	unsigned long i = 0;
	uint64_t alloc_size = 0, start = heap->cur;
	printf("heap starts with %llu\n", start);
	void *ptr;
	do {
		ptr = xheap_allocate(heap, chunk);
		if (ptr != NULL){
			i++;
			if ((unsigned long) ptr & ((1 << al_unit) -1)) {
				printf("ptr %x not aligned with al_unit %lu\n",
						ptr, al_unit);
				return -1;
			}
			alloc_size += xheap_get_chunk_size(ptr);
		}
	}while (ptr != NULL);
	printf("Allocated %lu chunks of size %lu with allocation unit %lu (%lu bytes)\n",
			i, chunk, al_unit, (1<<al_unit));
	printf("Total allocated space: %llu\n", (unsigned long long) alloc_size);
	alloc_size += i*sizeof(struct xheap_header);
	printf("Total allocated space plus headers: %llu\n", 
			(unsigned long long) alloc_size);
	alloc_size += start + 2 * sizeof(struct xheap_header);
	printf("Total allocated space plus headers and start/end padding: %llu\n", 
			(unsigned long long) alloc_size);
	printf("Lost space: %llu\n", (unsigned long long) (heap->size - alloc_size));
	return i;
}

int test_align(struct xheap *heap)
{
	uint64_t bytes = heap->size;
	int m = sizeof(bytes)*8 - __builtin_clzl(bytes -1);
	unsigned long i;
	void *ptr;
	for (i = 5; i < m; i++){
		int r = xheap_init(heap, size, i, mem);
		if (r < 0){
			printf("align: xheap init error\n");
			return -1;
		}
		do {
			ptr = xheap_allocate(heap, chunk);
			if (ptr != NULL){
				if ((unsigned long) ptr & ((1 << i) -1)) {
					printf("ptr %x not aligned with al_unit %lu\n",
							ptr, i);
					return -1;
				}
			}
		}while (ptr != NULL);
	}
	return 0;
}

int test_reuse(struct xheap *heap)
{
	int r = xheap_init(heap, size, al_unit, mem);
	if (r < 0){
		printf("xheap init error\n");
		return -1;
	}
	void *ptr1, *ptr2;
	ptr1 = xheap_allocate(heap, chunk);
	if (ptr1 == NULL) {
		printf("couldn't allocate\n");
		return -1;
	}
	xheap_free(ptr1);
	ptr2 = xheap_allocate(heap, chunk);
	if (ptr2 == NULL) {
		printf("couldn't allocate\n");
		return -1;
	}
	if (ptr1 != ptr2) {
		printf("alloc-free-alloc return diffrent ptr\n");
		return -1;
	}
	return 0;
}

struct thread_arg{
	int id;
	struct xheap *heap;
	unsigned long c;
	unsigned long allocations;
	unsigned long frees;
};

void *thread_test(void *arg)
{
	struct thread_arg *targ = (struct thread_arg *) arg;
	struct xheap *heap = targ->heap;
	int id = targ->id;
	unsigned long c = targ->c;
	void *ptr;
	unsigned long i = 0;
	do {
		ptr = xheap_allocate(heap, chunk);
		if (ptr != NULL){
			i++;
			memset(ptr, 1, xheap_get_chunk_size(ptr));
			if (c) {
				xheap_free(ptr);
				c--;
			}
		}
	}while (ptr != NULL);

	targ->allocations = i;
	targ->frees = targ->c - c;
	return NULL;
}

unsigned long test_threads(struct xheap *heap)
{
	int i;
	unsigned long allocations = 0;
	nr_frees = 0;

	int r = xheap_init(heap, size, al_unit, mem);
	if (r < 0){
		printf("threads: xheap init error\n");
		return -1;
	}

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
		targs[i].heap = heap;
		targs[i].c = 256;
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
		allocations +=  targs[i].allocations;
		nr_frees += targs[i].frees;
	}
	return allocations;
}

int main(int argc, const char *argv[])
{
	unsigned long alloc, expected, i =0;
	int r;
	if (argc < 5){
		printf("Usage: %s al_unit size chunk_size nr_threads\n", argv[0]);
		return -1;
	}
	al_unit = atol(argv[1]); 
	size = atol(argv[2]); 
	chunk = atol(argv[3]);
	nr_threads = atol(argv[4]);
	mem = malloc(size);
	r = test(&heap);
	alloc = r;
	
	printf("Testing align: ");
	r = test_align(&heap);
	if (r < 0) 
		printf("Failed\n");
	else
		printf("Success\n");
	
	printf("Testing reuse: ");
	r= test_reuse(&heap);
	if (r < 0) 
		printf("Failed\n");
	else
		printf("Success\n");
	expected = alloc + nr_frees;
	r = expected;
	while(r == expected && i < 100) {
		r = test_threads(&heap);
		expected = alloc + nr_frees;
		if (r != expected)
			printf("test_threads failed r: %lu vs expected %lu\n", r, expected);
		i++;
	}
	return 0;
}
