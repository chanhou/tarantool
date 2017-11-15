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
 * This header file (together with is companion C source-code file
 * "os.c") attempt to abstract the underlying operating system so that
 * the SQLite library will work on POSIX.
 *
 * This header file is #include-ed by sqliteInt.h and thus ends up
 * being included by every source file.
 */
#ifndef _SQLITE_OS_H_
#define _SQLITE_OS_H_

/* If the SET_FULLSYNC macro is not defined above, then make it
 * a no-op
 */
#ifndef SET_FULLSYNC
#define SET_FULLSYNC(x,y)
#endif

/*
 * The default size of a disk sector
 */
#ifndef SQLITE_DEFAULT_SECTOR_SIZE
#define SQLITE_DEFAULT_SECTOR_SIZE 4096
#endif

/*
 * Temporary files are named starting with this prefix followed by 16 random
 * alphanumeric characters, and no file extension. They are stored in the
 * OS's standard temporary file directory, and are deleted prior to exit.
 * If sqlite is being embedded in another program, you may wish to change the
 * prefix to reflect your program's name, so that if your program exits
 * prematurely, old temporary files can be easily identified. This can be done
 * using -DSQLITE_TEMP_FILE_PREFIX=myprefix_ on the compiler command line.
 *
 * 2006-10-31:  The default prefix used to be "sqlite_".  But then
 * Mcafee started using SQLite in their anti-virus product and it
 * started putting files with the "sqlite" name in the c:/temp folder.
 * This annoyed many windows users.  Those users would then do a
 * Google search for "sqlite", find the telephone numbers of the
 * developers and call to wake them up at night and complain.
 * For this reason, the default name prefix is changed to be "sqlite"
 * spelled backwards.  So the temp files are still identified, but
 * anybody smart enough to figure out the code is also likely smart
 * enough to know that calling the developer will not help get rid
 * of the file.
 */
#ifndef SQLITE_TEMP_FILE_PREFIX
#define SQLITE_TEMP_FILE_PREFIX "etilqs_"
#endif

/*
 * The following values may be passed as the second argument to
 * sqlite3OsLock(). The various locks exhibit the following semantics:
 *
 * SHARED:    Any number of processes may hold a SHARED lock simultaneously.
 * RESERVED:  A single process may hold a RESERVED lock on a file at
 *            any time. Other processes may hold and obtain new SHARED locks.
 * PENDING:   A single process may hold a PENDING lock on a file at
 *            any one time. Existing SHARED locks may persist, but no new
 *            SHARED locks may be obtained by other processes.
 * EXCLUSIVE: An EXCLUSIVE lock precludes all other locks.
 *
 * PENDING_LOCK may not be passed directly to sqlite3OsLock(). Instead, a
 * process that requests an EXCLUSIVE lock may actually obtain a PENDING
 * lock. This can be upgraded to an EXCLUSIVE lock by a subsequent call to
 * sqlite3OsLock().
 */
#define NO_LOCK         0
#define SHARED_LOCK     1
#define RESERVED_LOCK   2
#define PENDING_LOCK    3
#define EXCLUSIVE_LOCK  4

/*
 * File Locking Notes:
 *
 *
 * LockFile() prevents not just writing but also reading by other processes.
 * A SHARED_LOCK is obtained by locking a single randomly-chosen
 * byte out of a specific range of bytes. The lock byte is obtained at
 * random so two separate readers can probably access the file at the
 * same time, unless they are unlucky and choose the same lock byte.
 * An EXCLUSIVE_LOCK is obtained by locking all bytes in the range.
 * There can only be one writer.  A RESERVED_LOCK is obtained by locking
 * a single byte of the file that is designated as the reserved lock byte.
 * A PENDING_LOCK is obtained by locking a designated byte different from
 * the RESERVED_LOCK byte.
 *
 * The following #defines specify the range of bytes used for locking.
 * SHARED_SIZE is the number of bytes available in the pool from which
 * a random byte is selected for a shared lock.  The pool of bytes for
 * shared locks begins at SHARED_FIRST.
 *
 * Changing the value of PENDING_BYTE results in a subtly incompatible
 * file format.  Depending on how it is changed, you might not notice
 * the incompatibility right away, even running a full regression test.
 * The default location of PENDING_BYTE is the first byte past the
 * 1GB boundary.
 *
 */
#ifdef SQLITE_OMIT_WSD
#define PENDING_BYTE     (0x40000000)
#else
#define PENDING_BYTE      sqlite3PendingByte
#endif
#define RESERVED_BYTE     (PENDING_BYTE+1)
#define SHARED_FIRST      (PENDING_BYTE+2)
#define SHARED_SIZE       510

/*
 * Wrapper around OS specific sqlite3_os_init() function.
 */
int sqlite3OsInit(void);

/*
 * Functions for accessing sqlite3_file methods
 */
void sqlite3OsClose(sqlite3_file *);
int sqlite3OsRead(sqlite3_file *, void *, int amt, i64 offset);
int sqlite3OsWrite(sqlite3_file *, const void *, int amt, i64 offset);
int sqlite3OsTruncate(sqlite3_file *, i64 size);
int sqlite3OsSync(sqlite3_file *, int);
int sqlite3OsFileSize(sqlite3_file *, i64 * pSize);
int sqlite3OsLock(sqlite3_file *, int);
int sqlite3OsUnlock(sqlite3_file *, int);
int sqlite3OsCheckReservedLock(sqlite3_file * id, int *pResOut);
int sqlite3OsFileControl(sqlite3_file *, int, void *);
void sqlite3OsFileControlHint(sqlite3_file *, int, void *);
#define SQLITE_FCNTL_DB_UNCHANGED 0xca093fa0
int sqlite3OsSectorSize(sqlite3_file * id);
int sqlite3OsDeviceCharacteristics(sqlite3_file * id);
int sqlite3OsShmMap(sqlite3_file *, int, int, int, void volatile **);
int sqlite3OsShmLock(sqlite3_file * id, int, int, int);
void sqlite3OsShmBarrier(sqlite3_file * id);
int sqlite3OsShmUnmap(sqlite3_file * id, int);
int sqlite3OsFetch(sqlite3_file * id, i64, int, void **);
int sqlite3OsUnfetch(sqlite3_file *, i64, void *);

/*
 * Functions for accessing sqlite3_vfs methods
 */
int sqlite3OsOpen(sqlite3_vfs *, const char *, sqlite3_file *, int, int *);
int sqlite3OsDelete(sqlite3_vfs *, const char *, int);
int sqlite3OsAccess(sqlite3_vfs *, const char *, int, int *pResOut);
int sqlite3OsFullPathname(sqlite3_vfs *, const char *, int, char *);
#ifndef SQLITE_OMIT_LOAD_EXTENSION
void *sqlite3OsDlOpen(sqlite3_vfs *, const char *);
void sqlite3OsDlError(sqlite3_vfs *, int, char *);
void (*sqlite3OsDlSym(sqlite3_vfs *, void *, const char *)) (void);
void sqlite3OsDlClose(sqlite3_vfs *, void *);
#endif				/* SQLITE_OMIT_LOAD_EXTENSION */
int sqlite3OsRandomness(sqlite3_vfs *, int, char *);
int sqlite3OsSleep(sqlite3_vfs *, int);
int sqlite3OsGetLastError(sqlite3_vfs *);
int sqlite3OsCurrentTimeInt64(sqlite3_vfs *, sqlite3_int64 *);

/*
 * Convenience functions for opening and closing files using
 * sqlite3_malloc() to obtain space for the file-handle structure.
 */
int sqlite3OsOpenMalloc(sqlite3_vfs *, const char *, sqlite3_file **, int,
			int *);
void sqlite3OsCloseFree(sqlite3_file *);

#endif				/* _SQLITE_OS_H_ */
