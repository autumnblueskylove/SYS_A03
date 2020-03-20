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

#ifndef __DATA_LOGGER_H__
#define __DATA_LOGGER_H__

/* ==========================================================================
 * Why does this "#ifndef" have an "#else" clause?!?!?
 *
 * What we are doing in the lines below is effectively declaring
 * global variables (acquire_opertion, release_operation and init_values)
 * and in order to avoid any compiler errors about redefinition or
 * redeclaration of reinitialization we can only declare these
 * variables once.  So the "#ifndef" portion of this include file
 * actually declares and initializes them.  So when the first of 
 * the source files is compiled - it will actually execute this
 * "#ifndef" portion.  And when it does, as you know it declares
 * a compiler variable called "__SEMAPHORE_STRUCT_H__" which will
 * prevent any other source modules from doing this ...
 *
 * !!BUT!! we need the second (and any other) source files to
 * **know** about the existence of the acquire_opertion, release_operation 
 * and init_values variables - we just don't want those other source files
 * to redeclare or reinitialize them ... so how do we handle this?
 *    --> this is the purpose of the "#else" clause below
 * =========================================================================== */

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

/* ===========================================================================
 * By using the keyword "extern" here - you are making any source file that
 * includes this .h file (after the 1st one which exercises the "#ifndef"
 * above) - you are making them aware of the fact that there are these
 * GLOBAL structures out there (acquire_operation and release_operation)
 * that exist - and have been declared / initialized / had memory space allocated
 * and that you (this subsequent source file) are able to access them.
 *
 * Basically, the keyword "extern" allows source files to access and use
 * global structures and variables without having to declare / initialize them
 * =========================================================================== */
extern struct sembuf acquire_operation;
extern struct sembuf release_operation;
unsigned short init_values[1] = { 1 };

#endif
