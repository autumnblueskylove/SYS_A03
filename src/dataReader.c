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
	int 			orderCurrentClient = 0;			// the order of the current incomming client in the list
	int 			orderTimeOutClient = 0;  		// the order of the time ounted client in the list
	bool			flagRemovedAll = false;
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

		queueID = msgget (messageKey, IPC_CREAT | 0660);
		if (queueID == FAILURE) 
		{
			printf ("(SERVER) Cannot allocate a new queue!\n");
			return -2;
		}
	}
	printf ("(SERVER) The message queue ID is %d\n", queueID);
	pMasterList->msgQueueID = queueID;


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

    // Waiting after allocating the resources
    //sleep(15);
	

    // MAIN LOOP
    while(flagRemovedAll == false) 
    {
		// initialization
		t = 0;
		orderCurrentClient = 0;
		orderTimeOutClient = 0;

        // msgrcv, the first message on the queue shall be received
		if ((msgrcv(queueID, (void *)&sMsgData, sizeof(int), 0, 0)) == FAILURE)
        {
            // ERROR
            break;
        }
		else
		{
			// Get the localtime 
			t = time(NULL);
			localTime = localtime(&t); 
			printf("Local time and date: %s\n", asctime(localTime)); 
			printf("[%d-%d-%d %d:%d:%d] : DDDD\n", (localTime->tm_year + 1900), (localTime->tm_mon + 1), localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

			//sMsgData.msgStatus
			//sMsgData.msgType

			//pMasterList->dc[pMasterList->numberOfDCs].dcProcessID = sMsgData.msgType; 
			//pMasterList->dc[pMasterList->numberOfDCs].lastTimeHeardFrom = t; 
			//pMasterList->numberOfDCs; 
		}
		
		// seach for the client of the same message type
		for(int i=0; i < pMasterList->numberOfDCs; i++)
		{
			if(sMsgData.msgType == pMasterList->dc[pMasterList->numberOfDCs].dcProcessID)
			{
				orderCurrentClient = i + 1;
			}
		}

//1st operation
		if(orderCurrentClient = 0)					// new client
		{
			//add
			pMasterList->dc[orderCurrentClient].dcProcessID = sMsgData.msgType;
			pMasterList->dc[orderCurrentClient].lastTimeHeardFrom = t;
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
				pMasterList->dc[orderCurrentClient - 1].dcProcessID = sMsgData.msgType;
				pMasterList->dc[orderCurrentClient - 1].lastTimeHeardFrom = t;
				//log
			}
		}
//2nd operation
		//if absent during more than 35 seconds, 

		while(orderTimeOutClient != 0)						// if there is a time outed client or more
		{
			// seach for the client
			for(int i=0; i < pMasterList->numberOfDCs; i++)
			{

				t = time(NULL);
				if( (t - pMasterList->dc[pMasterList->numberOfDCs].lastTimeHeardFrom) > 35)     //more than 35 seconds
				{
					orderTimeOutClient = i + 1;
				} 
			}
			//remove()
			// log1: non-responsive
				//remove() - collapsing
		}

//3rd operation
		//if status 6,
		//remove
		//log1: offline
			//remove() - collapsing
			for(int i=(orderCurrentClient - 1); i< (pMasterList->numberOfDCs - 1); i++)  // all element except for the last one
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

//4th operation
	//usleep(1.5 * 1000);    					// 1.5 seconds
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