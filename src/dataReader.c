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
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "../inc/dataReader.h"
#include "../inc/msgQueue.h"
#include "../inc/shdMemory.h"
#include "../inc/debug.h"

#define LOOP_FOREVER                    1           // for an infinite loop
#define TIME_OUT                        35          // for non-responsive clients
#define FAILURE                         -1
#define MICRO_SECOND                    1000000

int RemoveAndCollapse(int orderClient, MasterList *pMasterList)
{
	// all element except for the last one
	for(int i=(orderClient - 1); i< (pMasterList->numberOfDCs - 1); i++)  
	{
		pMasterList->dc[i].dcProcessID = pMasterList->dc[i+1].dcProcessID;
		pMasterList->dc[i].lastTimeHeardFrom = pMasterList->dc[i+1].lastTimeHeardFrom;
	}

	// the last one
	pMasterList->dc[pMasterList->numberOfDCs -1].dcProcessID =	0;
	pMasterList->dc[pMasterList->numberOfDCs -1].lastTimeHeardFrom = 0;

	// decrease in the total number of whole clients
	pMasterList->numberOfDCs--;						

	if(pMasterList->numberOfDCs < 0)
	{
		//ERROR: unknown
		return -11;
	}
}

void OperationNonResponsive(MasterList *pMasterList)
{
	time_t t;
	int counter = 0;
	int orderNonResponsiveClient = 0;				// 0: no non-responsive client
	int startingPointConsecutive = 0;				// consecutive starting point for the counter

	while(LOOP_FOREVER)						
	{
		//initialization
		orderNonResponsiveClient = 0;

		// seaching for non-responsive clients
		for(counter = startingPointConsecutive; counter < pMasterList->numberOfDCs; counter++)
		{
			t = time(NULL);
			if( (t - pMasterList->dc[counter].lastTimeHeardFrom) > TIME_OUT)     
			{	//more than 35 seconds
				startingPointConsecutive = counter;
				orderNonResponsiveClient = counter + 1;
				break;
			} 
		}
		// log1: non-responsive
		if(orderNonResponsiveClient > 0)
		{
			RemoveAndCollapse(orderNonResponsiveClient, pMasterList);
		}
		else
		{
			break;
		}
	}
}

void OperationIncomming(MasterList *pMasterList, MessageData sMsgData, time_t t)
{
	int counter = 0;
	int orderIncomingClient = 0;					// the order of the registered client in the list, 
	                                        		// ... whose process id is same as the new one

	// seaching for the client of the same process id
	for(counter = 0; counter < pMasterList->numberOfDCs; counter++)
	{
		if(sMsgData.processID == pMasterList->dc[counter].dcProcessID)
		{
			orderIncomingClient = counter + 1;		// 1-indexed ID
			break;
		}
	}		

	if(orderIncomingClient == 0)					// new client
	{
		// adding
		pMasterList->dc[pMasterList->numberOfDCs].dcProcessID = sMsgData.processID;
		pMasterList->dc[pMasterList->numberOfDCs].lastTimeHeardFrom = t;
		pMasterList->numberOfDCs++;
		dp("[add] dc1-indexedID: %d, totalClient: %d\n", orderIncomingClient + 1, pMasterList->numberOfDCs);
		///log
	}
	else											// registered client
	{
		if(sMsgData.msgStatus == OFF_LINE)			// status 6
		{
			// removing
			RemoveAndCollapse(orderIncomingClient, pMasterList);
			dp("[remove] dc1-indexedID: %d, totalClient: %d\n", orderIncomingClient, pMasterList->numberOfDCs);
			///log
		}
		else										// status 1 ~ 5
		{
			// updating
			pMasterList->dc[orderIncomingClient - 1].lastTimeHeardFrom = t;
			dp("[update] dc1-indexedID: %d, totalClient: %d\n", orderIncomingClient, pMasterList->numberOfDCs);
			///log
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
    pMasterList->msgQueueID = queueID;
    pMasterList->numberOfDCs = 0;
	for (counter = 0; counter < MAX_DC_ROLES; counter++) 
	{
	    pMasterList->dc[counter].dcProcessID = 0;
	    pMasterList->dc[counter].lastTimeHeardFrom = 0;
	}

    // Waiting after allocating the resources
	printf("stop watch: start, 2 seconds\n");
    //sleep(15);
    sleep(2);

    // MAIN LOOP
    while(LOOP_FOREVER) 
    {
		// initialization
		t = 0;

        // a message on the queue shall be received
		if ((msgrcv(queueID, (void *)&sMsgData, (sizeof(MessageData) - sizeof(long)), MSG_TYPE, 0)) == FAILURE)
        {
            // ERROR
            break;
        }
		else
		{
			dp("[receive] pID: %d, status: %d\n", sMsgData.processID, sMsgData.msgStatus);
			// Get the localtime 
			t = time(NULL);
		}

		//1st operation, processing an incomming message
		OperationIncomming(pMasterList, sMsgData, t);

		//2nd operation, checking if there is a non-responsive client during more than 35 seconds
		OperationNonResponsive(pMasterList);

		//3rd operation, checking if all clients are removed from the list
		if(pMasterList->numberOfDCs == 0)
		{
			//log: The total number of machines reaches zero, then TERMINATING
			break;
		}

		//4th operation, being suspended for a while before going back to check new message
		usleep(1.5 * MICRO_SECOND);    					// 1.5 seconds
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