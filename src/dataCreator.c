/*
 * File        : dataCreator.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is a client application for IPC using the technique
 *               of message queue. The client generates a status condition representing
 *               the state of a machine. There are 7 status such as OK, Off-line, Error.
 *               It sends such a message on a random time basis.
 * 
 */
#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include "../inc/dataCreator.h"
#include "../inc/msgQueue.h"
#include "../inc/dataLogger.h"
#include "../inc/debug.h"

int main (void)
{
	key_t	 	    messageKey;                     // a key-type for message queue
	pid_t		    processID;                      // for process ID
	int 		    queueID = -1;                   // for message queue ID
    int             semId = -1;                     // for semaphore ID
    MessageStatus   eMsgStatus;                     // enum for status info.
	MessageData 	sMsgData;                       // stuct for message info.
    char            strLog[MAX_STRING_LOG];         // for logging

    // initialization
    eMsgStatus = OK;                                // the first status
	processID = getpid();                           // used as the machine's ID value
    sMsgData.msgStatus = eMsgStatus;
    sMsgData.msgType = MSG_TYPE;                   
    sMsgData.processID = processID;                   
    
    // setting up semaphore
    semId = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semId == FAILURE)
    {   // EROOR: cannot get any semaphore id
        return -1;
    }
    if(semctl(semId, 0, SETALL, init_values) == FAILURE)
    {   // EROOR: cannot initialize semaphore
        return -2;
    }

    // setting up message queue
	messageKey = ftok (".", 1234);                  // same message key as server's one
	if(messageKey == FAILURE) 
	{   // ERROR: cannot allocate a message key
        dlog(DATA_CREATOR, semId, "DC [%d] - EROOR: cannot allocate any message key");
	    return -3;
	}

    // getting for message queue...
	while((queueID = msgget (messageKey, 0)) == FAILURE) 
	{
        sleep(TIME_INTERVAL_CHECK_QUEUE);           // interval to check for message queue
	}

    // MAIN LOOP
	while(LOOP_FOREVER)                             
	{
        if(msgsnd (queueID, (void *)&sMsgData, (sizeof(MessageData) - sizeof(long)), 0) == FAILURE) 
        {   // ERROR: cannot send a message
            dlog(DATA_CREATOR, semId, "DC [%d] - EROOR: cannot send any message");
            return -4;
        }
        dp("[send] pID: %d, status: %d\n", sMsgData.processID, sMsgData.msgStatus);

        // logging the activity of sending a message
        sprintf (strLog, "DC [%d] - MSG SENT - Satus %d (%s)", 
            sMsgData.processID, sMsgData.msgStatus, kDescriptionStatus[sMsgData.msgStatus]);
        dlog(DATA_CREATOR, semId, strLog);

        if(eMsgStatus != OFF_LINE)                              // status 1 ~ 5
        {
            srand(time(0));
            eMsgStatus = (rand() % (NUM_STATUS - 1)) + 1;       // integer: 1 to 6
            sMsgData.msgStatus = eMsgStatus;
            srand(time(0));
            sleep((rand() % TIME_RANGE_SLEEP) + TIME_MIN_SLEEP);// integer: 10 to 30
        }
        else                                                    // status 6
        {
            break;                                              // to exit
        }
	}
    
	return 0;
}