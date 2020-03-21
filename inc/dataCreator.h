/*
 * File        : dataCreator.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataCreator.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSG_TYPE                        1234        // for message queue
#define TIME_INTERVAL_CHECK_QUEUE       10          // to check for message queue
#define NUM_STATUS                      7           // for the description of the status
#define FAILURE                         -1
#define NUM_STRING                      40

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

// description of the client's status
const char kDescriptionStatus[NUM_STATUS][NUM_STRING] = { 
    "Everything is OKAY",
    "Hydraulic Pressure Failure",
    "Safety Button Failure",
    "No Raw Material in the Process",
    "Operating Temperature Out of Range",
    "Operator Error",
    "Machine is Off-line"
};