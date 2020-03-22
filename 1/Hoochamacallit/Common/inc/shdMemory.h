/*
 * File        : shdMemory.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definition and 
 *               the data stuructors for a shared memory.
 */

#ifndef __SHD_MEMORY_H__
#define __SHD_MEMORY_H__

#define MAX_DC_ROLES                10          // the maximum number of clients

typedef struct
{
    pid_t   dcProcessID;
    time_t  lastTimeHeardFrom;
} DCInfo;

typedef struct                  
{
    int     msgQueueID;
    int     numberOfDCs;
    DCInfo  dc[MAX_DC_ROLES];
} MasterList;

#endif