/*
 * File        : dataReader.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to generate an S19 download file format or an
 *               assembly file which are both readable by human.
 *               This utility takes any binary input file and fransforms it
 *               into S-Record output file, or an assembly file for use in an
 *               embedded software development environment.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../inc/dataReader.h"

int main (void)
{
    key_t	 	    messageKey;
	key_t           shmKey;
	int 		    queueID = -1;                   // message queue ID
	int             shmId = -1;
    MessageStatus   eMsgStatus;
    MessageData 	sMsgData;
	MasterList      *pMasterList;
	int             counter = 0;
	    
    // initialization 

    // MESSAGE QUEUE
    messageKey = ftok (".", 1234);                  // same message key as server

	if (messageKey == FAILURE) 
	{ 
	    printf ("ERROR: cannot allocate a message key\n");
	    return -1;
	}

    printf("SERVER: checking for message queue\n");
	if ((queueID = msgget (messageKey, 0)) == FAILURE) 
	{
		printf ("(SERVER) No queue available, create!\n");

		queueID = msgget (message_key, IPC_CREAT | 0660);
		if (queueID == FAILURE) 
		{
			printf ("(SERVER) Cannot allocate a new queue!\n");
			return -2;
		}
	}
	printf ("(SERVER) The message queue ID is %d\n", queueID);

    // SHARED MEMORY
	shmKey = ftok (".", 16535);                     // for shared memory
	if (shmKey == FAILURE) 
    { 
        printf ("ERROR: cannot allocate a shared memory key\n");
	    return -1;
	}

	if ((shmId = shmget (shmKey, sizeof (MasterList), 0)) == FAILURE) 
	{
	    printf ("(PRODUCER) No Shared-Memory currently available - so create!\n");
	    shmId = shmget (shmKey, sizeof (MasterList), IPC_CREAT | 0660);
	    if (shmId == FAILURE) 
	    {
	        printf ("(PRODUCER) Cannot allocate a new memory!\n");
	        return -2;
	    }
	}

	printf ("(PRODUCER) Our Shared-Memory ID is %d\n", shmId);

    // attatch to the shared memory
	pMasterList = (MasterList *)shmat (shmId, NULL, 0); 
	if (pMasterList == NULL) 
	{
	    printf ("(PRODUCER) Cannot attach to shared memory!\n");
	    return -3;
	}

	// initialize the data of the shared memory our data to blanks
    pMasterList->msgQueueID = 0;
    pMasterList->numberOfDCs = 0;
    
	for (counter = 0; counter < MAX_DC_ROLES; counter++) 
	{
	    pMasterList->dc[counter].dcProcessID = 0;
	    pMasterList->dc[counter].lastTimeHeardFrom = 0;
	}

    // MAIN LOOP
    while(1) 
    {
        // msgrcv, the first message on the queue shall be received
		if ((msgrcv (queueId, (void *)&sMsgData, sizeof(int), 0, 0)) == FAILURE)
        {
            // ERROR
            break;
        }

        if(eMsgStatus != OK)
        {
        }
    }

	// remove the queue
	msgctl (queueID, IPC_RMID, (struct msqid_ds *)NULL);

	// detach the shared memory
	printf("(PRODUCER) Detaching from Shared-Memory\n");
	shmdt (pMasterList);
	
    // remove the shared memory
    printf("(PRODUCER) Removing the Shared-Memory resource\n");
	shmctl (shmId, IPC_RMID, (struct shmid_ds *)NULL);

    return 0;
}