/*
 * File        : dataReader.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataReader.c.
 */

#define MAX_DC_ROLES                    10          // the maximum number of clients

typedef struct
{
    pid_t   dcProcessID;
    time_t  lastTimeHeardFrom;
} DCInfo;

typedef struct                          // related to a shared memory structure
{
    int     msgQueueID;
    int     numberOfDCs;
    DCInfo  dc[MAX_DC_ROLES];
} MasterList;