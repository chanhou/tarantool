/*
 * Copyright 2010-2017, Tarantool AUTHORS, please see AUTHORS file.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the
 *    following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 *
 * This file contains the common header for all mutex implementations.
 * The sqliteInt.h header #includes this file so that it is available
 * to all source files.  We break it out in an effort to keep the code
 * better organized.
 *
 * NOTE:  source files should *not* #include this header file directly.
 * Source files should #include the sqliteInt.h file and let that file
 * include this one indirectly.
 */

/*
 * Figure out what version of the code to use.  The choices are
 *
 *   SQLITE_MUTEX_OMIT         No mutex logic.  Not even stubs.  The
 *                             mutexes implementation cannot be overridden
 *                             at start-time.
 *
 *   SQLITE_MUTEX_NOOP         For single-threaded applications.  No
 *                             mutual exclusion is provided.  But this
 *                             implementation can be overridden at
 *                             start-time.
 *
 *   SQLITE_MUTEX_PTHREADS     For multi-threaded applications on Unix.
 */
#if !SQLITE_THREADSAFE
#define SQLITE_MUTEX_OMIT
#endif
#if SQLITE_THREADSAFE && !defined(SQLITE_MUTEX_NOOP)
#define SQLITE_MUTEX_PTHREADS
#endif

#ifdef SQLITE_MUTEX_OMIT
/*
 * If this is a no-op implementation, implement everything as macros.
 */
#define sqlite3_mutex_alloc(X)    ((sqlite3_mutex*)8)
#define sqlite3_mutex_free(X)
#define sqlite3_mutex_enter(X)
#define sqlite3_mutex_try(X)      SQLITE_OK
#define sqlite3_mutex_leave(X)
#define sqlite3_mutex_held(X)     ((void)(X),1)
#define sqlite3_mutex_notheld(X)  ((void)(X),1)
#define sqlite3MutexAlloc(X)      ((sqlite3_mutex*)8)
#define sqlite3MutexInit()        SQLITE_OK
#define sqlite3MutexEnd()
#define MUTEX_LOGIC(X)
#else
#define MUTEX_LOGIC(X)            X
#endif				/* defined(SQLITE_MUTEX_OMIT) */
