/*
 * File        : dataLogger.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataLogger.c.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define DATA_CREATOR            0
#define DATA_MONITOR            1
#define DATA_CORRUPTOR          2
#define MAX_STRING_LOG          255             // maximum length of string for logging
void dlog(int progID, int semid, char contents[MAX_STRING_LOG]);

#ifdef __DATA_LOGGER_H__

/*
 * the following are operation structures for semaphore control
 * the acquire will decrement the semaphore by 1
 * and the release will increment the semaphore by 1.
 * Both of these structs are initialized to work with the 1st
 * semaphore we allocate.
 */
struct sembuf acquire_operation = { 0, -1, SEM_UNDO };
struct sembuf release_operation = { 0, 1, SEM_UNDO };

/*
 * the initial value of the semaphore will be 1, indicating that
 * our critical region is ready is ready for use by the first
 * task that can successfully decrement our shared semaphore
 */
unsigned short init_values[1] = { 1 };

#else

extern unsigned short init_values[];
extern struct sembuf acquire_operation;
extern struct sembuf release_operation;

#endif
