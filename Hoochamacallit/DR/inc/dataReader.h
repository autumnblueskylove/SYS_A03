/*
 * File        : dataReader.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataReader.c.
 */

#include <time.h>
#include "../inc/msgQueue.h"
#include "../inc/shdMemory.h"

#define LOOP_FOREVER                1               // for an infinite loop
#define TIME_OUT                    35              // for non-responsive clients
#define FAILURE                     -1
#define MICRO_SECOND                1000000
#define TIME_ALLOC_SLEEP            2 // 15
#define TIME_MICRO_NEXT             1.5

int RemoveAndCollapse(int, MasterList *, int);
void OperationNonResponsive(MasterList *, int);
void OperationIncomming(MasterList *, MessageData, time_t, int);