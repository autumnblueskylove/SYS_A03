/*
 * File        : dataCorruptor.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataCorruptor.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define     MAX_DC_ROLES    10

#define     DO_NOTING       0
#define     KILL_DC         1
#define     DELETE_MSGQ     2

typedef struct 
{
    pid_t   dcProcessID;
    int     lastTimeHeardFrom;
} DCInfo;


typedef struct 
{
    int     msgQueueID;
    int     numberOfDCs;
    DCInfo  dc[MAX_DC_ROLES];
} MasterList;
