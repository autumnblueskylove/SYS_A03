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
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>
#include "../inc/dataReader.h"

int RemoveAndCollapse(int orderIncomingClient, MasterList *pMasterList, bool flagRemovedAll)
{
	for(int i=(orderIncomingClient - 1); i< (pMasterList->numberOfDCs - 1); i++)  // all element except for the last one
	{
		pMasterList->dc[i].dcProcessID = pMasterList->dc[i+1].dcProcessID;
		pMasterList->dc[i].lastTimeHeardFrom = pMasterList->dc[i+1].lastTimeHeardFrom;
	}

	pMasterList->numberOfDCs--;
	if(pMasterList->numberOfDCs < 0)
	{
		//ERROR
		return -11;
	}
	else if(pMasterList->numberOfDCs == 0)
	{
		flagRemovedAll = true;			// all elements removed
		//log2: TERMINATING
	}
}

void OperationNonResponsive(int orderNonResponsiveClient, MasterList *pMasterList, bool flagRemovedAll)
{
	time_t t;
	while(orderNonResponsiveClient != 0)						// if there is a time outed client or more
	{
		// seach for the client
		for(int i=0; i < pMasterList->numberOfDCs; i++)
		{

			t = time(NULL);
			if( (t - pMasterList->dc[pMasterList->numberOfDCs].lastTimeHeardFrom) > 35)     //more than 35 seconds
			{
				orderNonResponsiveClient = i + 1;
			} 
		}
		// log1: non-responsive
		RemoveAndCollapse(orderNonResponsiveClient, pMasterList, flagRemovedAll);
	}
}

void OperationIncomming(int orderIncomingClient, MasterList *pMasterList, MessageData sMsgData, time_t t)
{
	if(orderIncomingClient == 0)					// new client
	{
		//add
		pMasterList->dc[orderIncomingClient].dcProcessID = sMsgData.processID;
		pMasterList->dc[orderIncomingClient].lastTimeHeardFrom = t;
		pMasterList->numberOfDCs++;
		//log
	}
	else										// known client
	{
		if(sMsgData.msgStatus == OFF_LINE)		// status 6
		{
			//last element
			pMasterList->dc[pMasterList->numberOfDCs].dcProcessID =0;
			pMasterList->dc[pMasterList->numberOfDCs].lastTimeHeardFrom = 0;
			//log
		}
		else									// status 1 ~ 5
		{
			//update
			pMasterList->dc[orderIncomingClient - 1].dcProcessID = sMsgData.processID;;
			pMasterList->dc[orderIncomingClient - 1].lastTimeHeardFrom = t;
			//log
		}
	}
}

int main (void)
{
    key_t	 	    messageKey;						// key for a message queue
	key_t           shmKey;							// key for a shared memory
	int 		    queueID = -1;                   // message queue ID
	int             shmId = -1;						// shared memory ID
    MessageStatus   eMsgStatus;
    MessageData 	sMsgData;
	MasterList      *pMasterList;
	time_t 			t;
    struct tm*      localTime;  
	int 			orderIncomingClient = 0;			// the order of the current incomming client in the list
	int 			orderNonResponsiveClient = 0;  		// the order of the time ounted client in the list
	bool			flagRemovedAll = false;
	int             counter = 0;
	    
    // initialization 
	sMsgData.msgType = 0;
	sMsgData.processID = 0;
	sMsgData.msgStatus = 0;

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

		queueID = msgget (messageKey, IPC_CREAT | 0660);
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
    // pMasterList->msgQueueID = 0;
    // pMasterList->numberOfDCs = 0;
    
	// for (counter = 0; counter < MAX_DC_ROLES; counter++) 
	// {
	//     pMasterList->dc[counter].dcProcessID = 0;
	//     pMasterList->dc[counter].lastTimeHeardFrom = 0;
	// }

    // Waiting after allocating the resources
	printf("stop watch: start, 2 seconds\n");
    //sleep(15);
    sleep(2);
	

int j = 0;
    // MAIN LOOP
    while(flagRemovedAll == false) 
    {
		// initialization
		t = 0;
		orderIncomingClient = 0;
		orderNonResponsiveClient = 0;

        // a message on the queue shall be received
		if ((msgrcv(queueID, (void *)&sMsgData, sizeof(int), MSG_TYPE, 0)) == FAILURE)
        {
            // ERROR
            break;
        }
		else
		{
			// Get the localtime 
			t = time(NULL);
		}
		
		// seach for the client of the same message type
		for(int i=0; i < pMasterList->numberOfDCs; i++)
		{
			if(sMsgData.processID == pMasterList->dc[pMasterList->numberOfDCs].dcProcessID)
			{
				orderIncomingClient = i + 1;
			}
		}

		//1st operation, processing incomming messages
		OperationIncomming(orderIncomingClient, pMasterList, sMsgData, t);

		//2nd operation, checking if th during more than 35 seconds
		OperationNonResponsive(orderNonResponsiveClient, pMasterList, flagRemovedAll);

		//3rd operation
		if(sMsgData.msgStatus == OFF_LINE)
		{
			RemoveAndCollapse(orderIncomingClient, pMasterList, flagRemovedAll);
			//log1: offline
		}

		//4th operation
			printf("stop watch[%d]: 1.5 seconds\n", j++);
			usleep(1.5 * 1000000);    					// 1.5 seconds
    }

	// release the queue
	msgctl (queueID, IPC_RMID, (struct msqid_ds *)NULL);

	// detach the shared memory
	printf("(PRODUCER) Detaching from Shared-Memory\n");
	shmdt (pMasterList);
	
    // release the shared memory
    printf("(PRODUCER) Removing the Shared-Memory resource\n");
	shmctl (shmId, IPC_RMID, (struct shmid_ds *)NULL);

    return 0;
}