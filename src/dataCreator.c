/*
 * File        : dataCreator.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to generate an S19 download file format or an
 *               assembly file which are both readable by human.
 *               This utility takes any binary input file and fransforms it
 *               into S-Record output file, or an assembly file for use in an
 *               embedded software development environment.
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
#include "../inc/dataCreator.h"
#include "../inc/msgQueue.h"
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

	if (messageKey == FAILURE) 
	{ 
	    printf ("ERROR: cannot allocate a message key\n");
	    return -1;
	}

	printf("LOOP: checking for message queue\n");
	while ((queueID = msgget (messageKey, 0)) == FAILURE) 
	{
        //sleep(TIME_INTERVAL_CHECK_QUEUE);           // interval to check for message queue
        sleep(1);           // interval to check for message queue
	}
	printf ("(CLIENT) The message queue ID is %d\n", queueID);

    printf("LOOP: sending messages");
	while(eMsgStatus != OFF_LINE)
	{
        sMsgData.msgStatus = eMsgStatus;
        if (msgsnd (queueID, (void *)&sMsgData, (sizeof(MessageData) - sizeof(long)), 0) == FAILURE) 
        {
            printf ("ERROR: cannot send a message\n");
            return -2;
        }
        dp("[send a message] ID: %d, status: %d\n", sMsgData.processID, sMsgData.msgStatus);
        

        // if(eMsgStatus != OK)
        // {
            srand(time(0));
            eMsgStatus = (rand() % 6) + 1;	        // integer: 1 to 6
            sMsgData.msgStatus = eMsgStatus;
            srand(time(0));
            // sleep((rand() % 21) + 10);              // integer: 10 to 30
            int i = ((rand() % 2) + 3);              // integer: 10 to 30
            printf("sleep(%d)\n", i);
            sleep(i);              // integer: 10 to 30
        // }
	}
	return 0;
}