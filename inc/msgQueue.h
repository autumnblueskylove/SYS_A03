/*
 * File        : msgQueue.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions and 
 *               the data stuructors for a message queue.
 */

#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#define MSG_TYPE                        1234        // for message queue

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

typedef struct                                  
{
    long msgType;
    int processID;
	int msgStatus;
} MessageData;
#endif