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
	pid_t		processID;
	key_t	 	messageKey;
	int 		queueID = -1;                       // message queue ID
    MessageStatus   eMsgStatus;
	MessageData 	sMsgData;

    // initialization
    eMsgStatus = OK;
    sMsgData.msgStatus = eMsgStatus;
    sMsgData.msgType = MSG_TYPE;                   
	processID = getpid();                           // used as the machine's ID value
    sMsgData.processID = processID;                   
	printf ("(CLIENT) My PID is %d\n", processID);
    
	messageKey = ftok (".", 1234);                  // same message key as server

	if(messageKey == FAILURE) 
	{ 
	    printf ("ERROR: cannot allocate a message key\n");
	    return -1;
	}

	printf("(CLIENT) checking for message queue, 1 second\n");
	while((queueID = msgget (messageKey, 0)) == FAILURE) 
	{
        //sleep(TIME_INTERVAL_CHECK_QUEUE);           // interval to check for message queue
        sleep(1);
	}
	printf ("(CLIENT) The message queue ID is %d\n", queueID);


// get semaphore ID
    int semid;
    semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semid == -1)
    {
        printf("(Logger) Cannot get semid\n");
        exit(1);
    }
    printf ("(Logger) semID is %d\n", semid);
    if(semctl(semid, 0, SETALL, init_values) == -1)
    {
         printf("(Logger) Cannot initialize semid\n");
        exit(2);
    }


	while(LOOP_FOREVER)                             // MAIN LOOP
	{
        if(msgsnd (queueID, (void *)&sMsgData, (sizeof(MessageData) - sizeof(long)), 0) == FAILURE) 
        {
            printf ("ERROR: cannot send a message\n");
            return -2;
        }
        dp("[send] pID: %d, status: %d\n", sMsgData.processID, sMsgData.msgStatus);
        // logging the activity of sending a message
        char log[200];
        sprintf (log, "DC [%d] - MSG SENT - Satus %d (%s)", sMsgData.processID, sMsgData.msgStatus, kDescriptionStatus[sMsgData.msgStatus]);
        dlog(DATA_CREATOR, semid, log);

    
        if(eMsgStatus != OFF_LINE)
        {
            srand(time(0));
            eMsgStatus = (rand() % 6) + 1;	        // integer: 1 to 6
            sMsgData.msgStatus = eMsgStatus;
            srand(time(0));
            // sleep((rand() % 21) + 10);              // integer: 10 to 30
            int i = ((rand() % 2) + 3);              // integer: 10 to 30
            printf("sleep(%d)\n", i);
            sleep(i);              // integer: 10 to 30
        }
        else
        {
            break;                                  // to exit
        }
	}
    
	return 0;
}