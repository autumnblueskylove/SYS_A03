/*
 * File        : dataCreator.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataCreator.c.
 */

#define LOOP_FOREVER                    1           // for an infinite loop
#define FAILURE                         -1
#define NUM_STATUS                      7           // for the description of the status
#define NUM_STRING                      40          // for the description of the status
#define TIME_INTERVAL_CHECK_QUEUE       10          // to check for message queue

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