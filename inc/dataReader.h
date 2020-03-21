/*
 * File        : dataReader.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataReader.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define LOOP_FOREVER                    1           // for an infinite loop
#define MSG_TYPE                        1234        // for message queue
#define MAX_DC_ROLES                    10          // the maximum number of clients
#define FAILURE                         -1
#define MICRO_SECOND                    1000000


typedef enum
{
    OK,                                             // means Everything is OKAY
    FAILURE_HYDRAULIC_FAILURE,                      // means Hydraulic Pressure Failure
    FAILURE_SAFETY_BUTTON,                          // means Safety Button Failure
    NO_RAW_MATERIAL,                                // means No Raw Material in the Process
    OUT_OPERATING_TEMP,                             // means Operating Temperature Out of Range
    ERROR_OPERATOR,                                 // means Operator Error
    OFF_LINE                                        // means Machine is Off-line
} MessageStatus;

typedef struct                                      // related to a message queue structure
{
    long msgType;
    int processID;
	int msgStatus;
} MessageData;

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