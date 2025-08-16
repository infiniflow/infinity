/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * N.B. You most likely do _not_ want to use SpinLock or any other
 * kind of spinlock.  Use SharedMutex instead.
 *
 * In short, spinlocks in preemptive multi-tasking operating systems
 * have serious problems and fast mutexes like SharedMutex are almost
 * certainly the better choice, because letting the OS scheduler put a
 * thread to sleep is better for system responsiveness and throughput
 * than wasting a timeslice repeatedly querying a lock held by a
 * thread that's blocked, and you can't prevent userspace
 * programs blocking.
 *
 * Spinlocks in an operating system kernel make much more sense than
 * they do in userspace.
 *
 * -------------------------------------------------------------------
 *
 * Two Read-Write spin lock implementations.
 *
 *  Ref: http://locklessinc.com/articles/locks
 *
 *  Both locks here are faster than pthread_rwlock and have very low
 *  overhead (usually 20-30ns).  They don't use any system mutexes and
 *  are very compact (4/8 bytes), so are suitable for per-instance
 *  based locking, particularly when contention is not expected.
 *
 *  For a spinlock, SpinLock is a reasonable choice.  (See the note
 *  about for why a spin lock is frequently a bad idea generally.)
 *  SpinLock has minimal overhead, and comparable contention
 *  performance when the number of competing threads is less than or
 *  equal to the number of logical CPUs.  Even as the number of
 *  threads gets larger, SpinLock can still be very competitive in
 *  READ, although it is slower on WRITE, and also inherently unfair
 *  to writers.
 *
 *  RWTicketSpinLock shows more balanced READ/WRITE performance.  If
 *  your application really needs a lot more threads, and a
 *  higher-priority writer, prefer one of the RWTicketSpinLock locks.
 *
 *  Caveats:
 *
 *    RWTicketSpinLock locks can only be used with GCC on x86/x86-64
 *    based systems.
 *
 *    RWTicketSpinLock<32> only allows up to 2^8 - 1 concurrent
 *    readers and writers.
 *
 *    RWTicketSpinLock<64> only allows up to 2^16 - 1 concurrent
 *    readers and writers.
 *
 *    RWTicketSpinLock<..., true> (kFavorWriter = true, that is, strict
 *    writer priority) is NOT reentrant, even for lock_shared().
 *
 *    The lock will not grant any new shared (read) accesses while a thread
 *    attempting to acquire the lock in write mode is blocked. (That is,
 *    if the lock is held in shared mode by N threads, and a thread attempts
 *    to acquire it in write mode, no one else can acquire it in shared mode
 *    until these N threads release the lock and then the blocked thread
 *    acquires and releases the exclusive lock.) This also applies for
 *    attempts to reacquire the lock in shared mode by threads that already
 *    hold it in shared mode, making the lock non-reentrant.
 *
 *    SpinLock handles 2^30 - 1 concurrent readers.
 *
 * @author Xin Liu <xliux@fb.com>
 */

/*
========================================================================
Benchmark on (Intel(R) Xeon(R) CPU  L5630  @ 2.13GHz)  8 cores(16 HTs)
========================================================================

------------------------------------------------------------------------------
1. Single thread benchmark (read/write lock + unlock overhead)
Benchmark                                    Iters   Total t    t/iter iter/sec
-------------------------------------------------------------------------------
*      BM_RWSpinLockRead                     100000  1.786 ms  17.86 ns   53.4M
+30.5% BM_RWSpinLockWrite                    100000  2.331 ms  23.31 ns  40.91M
+85.7% BM_RWTicketSpinLock32Read             100000  3.317 ms  33.17 ns  28.75M
+96.0% BM_RWTicketSpinLock32Write            100000    3.5 ms     35 ns  27.25M
+85.6% BM_RWTicketSpinLock64Read             100000  3.315 ms  33.15 ns  28.77M
+96.0% BM_RWTicketSpinLock64Write            100000    3.5 ms     35 ns  27.25M
+85.7% BM_RWTicketSpinLock32FavorWriterRead  100000  3.317 ms  33.17 ns  28.75M
+29.7% BM_RWTicketSpinLock32FavorWriterWrite 100000  2.316 ms  23.16 ns  41.18M
+85.3% BM_RWTicketSpinLock64FavorWriterRead  100000  3.309 ms  33.09 ns  28.82M
+30.2% BM_RWTicketSpinLock64FavorWriterWrite 100000  2.325 ms  23.25 ns  41.02M
+ 175% BM_PThreadRWMutexRead                 100000  4.917 ms  49.17 ns   19.4M
+ 166% BM_PThreadRWMutexWrite                100000  4.757 ms  47.57 ns  20.05M

------------------------------------------------------------------------------
2. Contention Benchmark      90% read  10% write
Benchmark                    hits       average    min       max        sigma
------------------------------------------------------------------------------
---------- 8  threads ------------
SpinLock       Write       142666     220ns      78ns      40.8us     269ns
SpinLock       Read        1282297    222ns      80ns      37.7us     248ns
RWTicketSpinLock Write       85692      209ns      71ns      17.9us     252ns
RWTicketSpinLock Read        769571     215ns      78ns      33.4us     251ns
pthread_rwlock_t Write       84248      2.48us     99ns      269us      8.19us
pthread_rwlock_t Read        761646     933ns      101ns     374us      3.25us

---------- 16 threads ------------
SpinLock       Write       124236     237ns      78ns      261us      801ns
SpinLock       Read        1115807    236ns      78ns      2.27ms     2.17us
RWTicketSpinLock Write       81781      231ns      71ns      31.4us     351ns
RWTicketSpinLock Read        734518     238ns      78ns      73.6us     379ns
pthread_rwlock_t Write       83363      7.12us     99ns      785us      28.1us
pthread_rwlock_t Read        754978     2.18us     101ns     1.02ms     14.3us

---------- 50 threads ------------
SpinLock       Write       131142     1.37us     82ns      7.53ms     68.2us
SpinLock       Read        1181240    262ns      78ns      6.62ms     12.7us
RWTicketSpinLock Write       83045      397ns      73ns      7.01ms     31.5us
RWTicketSpinLock Read        744133     386ns      78ns        11ms     31.4us
pthread_rwlock_t Write       80849      112us      103ns     4.52ms     263us
pthread_rwlock_t Read        728698     24us       101ns     7.28ms     194us

*/

module;

#include "builtin.h"

export module infinity_core:spinlock;

import :infinity_type;

namespace infinity {

/*
 * A simple, small (4-bytes), but unfair rwlock.  Use it when you want
 * a nice writer and don't expect a lot of write/read contention, or
 * when you need small rwlocks since you are creating a large number
 * of them.
 *
 * Note that the unfairness here is extreme: if the lock is
 * continually accessed for read, writers will never get a chance.  If
 * the lock can be that highly contended this class is probably not an
 * ideal choice anyway.
 *
 * It currently implements most of the Lockable, SharedLockable and
 * UpgradeLockable concepts except the TimedLockable related locking/unlocking
 * interfaces.
 */
export class SpinLock {
    enum : i32 { READER = 4, UPGRADED = 2, WRITER = 1 };

public:
    constexpr SpinLock() : bits_(0) {}

    SpinLock(SpinLock const &) = delete;

    SpinLock &operator=(SpinLock const &) = delete;

    // Lockable Concept
    void lock() noexcept {
        while (!try_lock()) {
            PAUSE;
        }
    }

    // Writer is responsible for clearing up both the UPGRADED and WRITER bits.
    void unlock() noexcept {
        static_assert(READER > WRITER + UPGRADED, "wrong bits!");
        bits_.fetch_and(~(WRITER | UPGRADED), std::memory_order::release);
    }

    // SharedLockable Concept
    void lock_shared() noexcept {
        while (!try_lock_shared()) {
            PAUSE;
        }
    }

    void unlock_shared() noexcept { bits_.fetch_add(-READER, std::memory_order::release); }

    // Downgrade the lock from writer status to reader status.
    void unlock_and_lock_shared() noexcept {
        bits_.fetch_add(READER, std::memory_order::acquire);
        unlock();
    }

    // UpgradeLockable Concept
    void lock_upgrade() noexcept {
        while (!try_lock_upgrade()) {
            PAUSE;
        }
    }

    void unlock_upgrade() noexcept { bits_.fetch_add(-UPGRADED, std::memory_order::acq_rel); }

    // unlock upgrade and try to acquire write lock
    void unlock_upgrade_and_lock() noexcept {
        while (!try_unlock_upgrade_and_lock()) {
            PAUSE;
        }
    }

    // unlock upgrade and read lock atomically
    void unlock_upgrade_and_lock_shared() noexcept { bits_.fetch_add(READER - UPGRADED, std::memory_order::acq_rel); }

    // write unlock and upgrade lock atomically
    void unlock_and_lock_upgrade() noexcept {
        // need to do it in two steps here -- as the UPGRADED bit might be OR-ed at
        // the same time when other threads are trying do try_lock_upgrade().
        bits_.fetch_or(UPGRADED, std::memory_order::acquire);
        bits_.fetch_add(-WRITER, std::memory_order::release);
    }

    // Attempt to acquire writer permission. Return false if we didn't get it.
    bool try_lock() noexcept {
        i32 expect = 0;
        return bits_.compare_exchange_strong(expect, WRITER, std::memory_order::acq_rel);
    }

    // Try to get reader permission on the lock. This can fail if we
    // find out someone is a writer or upgrader.
    // Setting the UPGRADED bit would allow a writer-to-be to indicate
    // its intention to write and block any new readers while waiting
    // for existing readers to finish and release their read locks. This
    // helps avoid starving writers (promoted from upgraders).
    bool try_lock_shared() noexcept {
        // fetch_add is considerably (100%) faster than compare_exchange,
        // so here we are optimizing for the common (lock success) case.
        i32 value = bits_.fetch_add(READER, std::memory_order::acquire);
        if (value & (WRITER | UPGRADED)) {
            bits_.fetch_add(-READER, std::memory_order::release);
            return false;
        }
        return true;
    }

    // try to unlock upgrade and write lock atomically
    bool try_unlock_upgrade_and_lock() noexcept {
        i32 expect = UPGRADED;
        return bits_.compare_exchange_strong(expect, WRITER, std::memory_order::acq_rel);
    }

    // try to acquire an upgradable lock.
    bool try_lock_upgrade() noexcept {
        i32 value = bits_.fetch_or(UPGRADED, std::memory_order::acquire);

        // Note: when failed, we cannot flip the UPGRADED bit back,
        // as in this case there is either another upgrade lock or a write lock.
        // If it's a write lock, the bit will get cleared up when that lock's done
        // with unlock().
        return ((value & (UPGRADED | WRITER)) == 0);
    }

    // mainly for debugging purposes.
    [[nodiscard]] i32 bits() const noexcept { return bits_.load(std::memory_order::acquire); }

private:
    std::atomic<i32> bits_;
};

export class ScopedSpinLock {
public:
    explicit ScopedSpinLock(SpinLock &lock) : lock_(lock) { lock_.lock(); }

    ~ScopedSpinLock() { lock_.unlock(); }

private:
    ScopedSpinLock(const ScopedSpinLock &) = delete;

    ScopedSpinLock &operator=(const ScopedSpinLock &) = delete;

    SpinLock &lock_;
};

export struct ScopedTryLock {
    ScopedTryLock(SpinLock &lock) : lock_(lock) { locked_ = lock_.try_lock(); }

    ~ScopedTryLock() {
        if (locked_)
            lock_.unlock();
    }

    bool is_locked() const { return locked_; }

    bool locked_;
    SpinLock &lock_;
};

} // namespace infinity
