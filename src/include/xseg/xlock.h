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

#ifndef _XLOCK_H
#define _XLOCK_H

#include <xseg/util.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define MFENCE() __sync_synchronize()
#define BARRIER() __asm__ __volatile__ ("" ::: "memory")
#define __pause() __asm__ __volatile__ ("pause\n");
#undef __pause
#define __pause()


typedef uint64_t xlock_owner_t;

#define XLOCK_NOONE ((xlock_owner_t)0)

#define XLOCK_CONGESTION_NOTIFY

#ifdef XLOCK_CONGESTION_NOTIFY
/* When XLOCK_CONGESTION_NOTIFY is defined, xlock_acquire will start print
 * congestion warning messages after it has spinned for a minimum of 2^MIN_SHIFT
 * times. After that, each time the spin count doubles, a new message will be
 * logged.
 * At STACKTRACE_SHIFT, the backtrace of the process trying to acquire the lock
 * will be logged, to better get the codepath that led to congestions (or even
 * deadlock).
 */
#define MIN_SHIFT 20
#define STACKTRACE_SHIFT 23
#define MAX_SHIFT ((sizeof(unsigned long) * 8) -1)
#endif /* XLOCK_CONGESTION_NOTIFY */

/* Implement our gettid wrapper to cache gettid results */
static __thread pid_t tid=0;
static pid_t gettid()
{
	if (tid)
		return tid;
	tid = syscall(SYS_gettid);
	return tid;
}

struct xlock {
	xlock_owner_t owner;
	unsigned long pc;
};

/*
 * Owner comprises of three values: The PID of the owner, the TID of the owner
 * and the PC where the lock resides.
 * In order for them to fit into an xlock_owner_t, the 16 LSBs of the PID, the
 * 16 LSBs of the TID and the 32 LSBs from the PC are kept.
 *
 * |-----16-----||-----16-----||------------32------------|
 * |----PID-----||----TID-----||------------PC------------|
 */

#define PID_BITS 16
#define TID_BITS 16
#define PC_BITS 32

/*
_Static_assert(PID_BITS + TID_BITS + PC_BITS == sizeof(xlock_owner_t),
		"Invalid bit definitions")

_Static_assert(PID_BITS <= sizeof(pid_t) * 8,
	"PID_BITS must be lower or equal than the bits of pid_t")

_Static_assert(TID_BITS <= sizeof(pid_t) * 8,
	"TID_BITS must be lower or equal than the bits of pid_t")

_Static_assert(PC_BITS <= sizeof(void *) * 8,
	"PC_BITS must be lower or equal than the bits of a (void *)")
*/

static inline xlock_owner_t xlock_pack_owner(pid_t pid, pid_t tid, void *ip)
{
	xlock_owner_t owner = 0;
	unsigned long pc = (unsigned long)ip;

	pid &= ((xlock_owner_t)1 << PID_BITS) -1;
	tid &= ((xlock_owner_t)1 << TID_BITS) -1;
	pc &= ((xlock_owner_t)1 << PC_BITS) -1;

	owner |= pid;
	owner <<= TID_BITS;
	owner |= tid;
	owner <<= PC_BITS;
	owner |= pc;

	return owner;
}

static inline void xlock_unpack_owner(uint64_t owner, pid_t *pid, pid_t *tid, void **ip)
{
	unsigned long pc;
	if (ip) {
		pc = owner & (((xlock_owner_t)1 << PC_BITS) -1);
		*ip = (void *)pc;
	}
	owner >>= PC_BITS;
	if (tid) {
		*tid = owner & (((xlock_owner_t)1 << TID_BITS) -1);
	}

	owner >>= TID_BITS;
	if (pid) {
		*pid = owner & (((xlock_owner_t)1 << PID_BITS) -1);
	}
}

/* x86_64 specific
 * TODO: Move to an arch specific directory
 */
static inline void * __get_pc()
{
	void * rip;

	__asm__ volatile ("lea (%%rip, 1),  %0" : "=r"(rip));

	return rip;
}

__attribute__((always_inline)) static inline unsigned long xlock_acquire(struct xlock *lock)
{
	xlock_owner_t owner, who;
	pid_t pid, tid, opid, otid;
	void *pc, *opc;
#ifdef XLOCK_CONGESTION_NOTIFY
	unsigned long times = 1;
	unsigned long shift = MIN_SHIFT;
#endif /* XLOCK_CONGESTION_NOTIFY */

	pid = getpid();
	tid = gettid();
	pc = __get_pc();

	who = xlock_pack_owner(pid, tid, pc);

	for (;;) {
		for (; (owner = *(volatile xlock_owner_t*)(&lock->owner)) != XLOCK_NOONE;) {
#ifdef XLOCK_CONGESTION_NOTIFY
			if (!(times & ((1<<shift) -1))) {
				xlock_unpack_owner(owner, &opid, &otid, &opc);
				XSEGLOG2(I, "xlock %p spinned for %llu times"
					"\n\t who: (%d, %d, %p), "
					"owner: (%d, %d, %p) (full pc: %p)",
					(unsigned long) lock, times,
					pid, tid, pc, opid, otid, opc,
					lock->pc);
				if (shift == STACKTRACE_SHIFT)
					xseg_printtrace();
				if (shift < MAX_SHIFT)
					shift++;
			}
			times++;
#endif /* XLOCK_CONGESTION_NOTIFY */
			__pause();
		}

		if (__sync_bool_compare_and_swap(&lock->owner, XLOCK_NOONE, who)) {
			/* Store full program counter in a non-atomic manner,
			 * for debuging reasons only.
			 */
			lock->pc = (unsigned long)pc;
			break;
		}
	}

	return 1;
}

__attribute__((always_inline)) static inline unsigned long xlock_try_lock(struct xlock *lock)
{
	xlock_owner_t owner, who;
	pid_t pid, tid;
	void *pc;

	pid = getpid();
	tid = gettid();
	pc = __get_pc();

	who = xlock_pack_owner(pid, tid, pc);

	owner = *(volatile xlock_owner_t*)(&lock->owner);
	if (owner == XLOCK_NOONE &&
		__sync_bool_compare_and_swap(&lock->owner, XLOCK_NOONE, who)) {
		lock->pc = (unsigned long)pc;
		return 1;
	}
	return 0;
}

static inline void xlock_release(struct xlock *lock)
{
	BARRIER();
	lock->pc = 0;
	lock->owner = XLOCK_NOONE;
}

static inline xlock_owner_t xlock_get_owner(struct xlock *lock)
{
	return *(volatile xlock_owner_t*)(&lock->owner);
}

#endif
