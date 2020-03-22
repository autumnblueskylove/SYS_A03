/*
 * File        : dataReader.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is a server application for IPC using the techniques
 *               both message queue and shared momory. The server keep track of the
 *               number of different and active clients present in the system. It
 *               can communicate with upto 10 clients.
 */
#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "../inc/dataReader.h"
#include "../inc/dataLogger.h"

int main (void)
{
    key_t	 	    messageKey;						// a key-type for message queue
	key_t           shmKey;							// a key-type for shared memory
	int 		    queueID = -1;                   // for message queue ID
	int             shmId = -1;						// for shared memory ID
	int             semId = -1;                     // for semaphore ID
    MessageStatus   eMsgStatus;                     // enum for status info.
    MessageData 	sMsgData;                       // stuct for message info.
	MasterList      *pMasterList;                   // for stuct for list info.
    char            strLog[MAX_STRING_LOG];         // for logging
	time_t 			t;
	int             counter = 0;
	    
    // initialization 
	sMsgData.msgType = 0;
	sMsgData.processID = 0;
	sMsgData.msgStatus = 0;

    // setting up semaphore
    semId = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semId == FAILURE)
    {   // ERROR: cannot get any semaphore id
        return -1;
    }
    if(semctl(semId, 0, SETALL, init_values) == FAILURE)
    {   // ERROR: cannot initialize semaphore
        return -2;
    }

    // setting up message queue
    messageKey = ftok (".", 1234);                  // same message key as server
	if(messageKey == FAILURE) 
	{   // ERROR: cannot allocate a message key
        dlog(DATA_MONITOR, semId, "DR - ERROR: cannot allocate any message key");
	    return -3;
	}
    // getting or creating message queue...
	if((queueID = msgget(messageKey, 0)) == FAILURE) 
	{	// ERROR: cannot allocate any message key
		queueID = msgget(messageKey, IPC_CREAT | 0660);
		if(queueID == FAILURE) 
		{
			dlog(DATA_MONITOR, semId, "DR - ERROR: cannot allocate any message key");
			return -4;
		}
	}

    // setting up shared memory
	shmKey = ftok (".", 16535);                     // for shared memory
	if(shmKey == FAILURE) 
    { 	// ERROR: cannot allocate any shared memory key
		dlog(DATA_MONITOR, semId, "DR - ERROR: cannot allocate any shared memory key");
	    return -5;
	}
	if((shmId = shmget(shmKey, sizeof (MasterList), 0)) == FAILURE) 
	{	
	    shmId = shmget(shmKey, sizeof (MasterList), IPC_CREAT | 0660);
	    if(shmId == FAILURE) 
	    {	// ERROR: cannot allocate any shared memory
			dlog(DATA_MONITOR, semId, "DR - ERROR: cannot allocate any shared memory");
	        return -6;
	    }
	}

    // attatching to the shared memory
	pMasterList = (MasterList *)shmat (shmId, NULL, 0); 
	if (pMasterList == NULL) 
	{	// ERROR: cannot attach to shared memory
		dlog(DATA_MONITOR, semId, "DR - ERROR: cannot attach to shared memory");
	    return -7;
	}
	// initialize the data of the shared memory
    pMasterList->msgQueueID = queueID;
    pMasterList->numberOfDCs = 0;
	for(counter = 0; counter < MAX_DC_ROLES; counter++) 
	{
	    pMasterList->dc[counter].dcProcessID = 0;
	    pMasterList->dc[counter].lastTimeHeardFrom = 0;
	}

    // Waiting after allocating the resources
    sleep(TIME_ALLOC_SLEEP);						// 15 seconds

	// MAIN LOOP
    while(LOOP_FOREVER) 							
    {
		// initialization
		t = 0;

        // receiving a message on the queue
		if((msgrcv(queueID, (void *)&sMsgData, (sizeof(MessageData) - sizeof(long)), MSG_TYPE, 0)) == FAILURE)
        {	// ERROR: ERROR: cannot receive any message
			dlog(DATA_MONITOR, semId, "DR - ERROR: cannot receive any message");
            break;
        }
		else
		{
			// getting the localtime 
			t = time(NULL);
		}

		// 1st operation, processing an incomming message
		OperationIncomming(pMasterList, sMsgData, t, semId);

		// 2nd operation, checking if there is a non-responsive client during more than 35 seconds
		OperationNonResponsive(pMasterList, semId);

		// 3rd operation, checking if all clients are removed from the list
		if(pMasterList->numberOfDCs == 0)
		{
			// logging the event of TERMINATING because the total number of machines reaches zero
			dlog(DATA_MONITOR, semId, "All DCs have gone offline or terminated - DR TERMINATING");
			break;
		}

		// 4th operation, being suspended for a while before going back to check new message
		usleep(TIME_MICRO_NEXT * MICRO_SECOND);    	// 1.5 seconds
    }

	// releasing the queue
	msgctl(queueID, IPC_RMID, (struct msqid_ds *)NULL);

	// detaching the shared memory
	shmdt(pMasterList);
	
    // releasing the shared memory
	shmctl(shmId, IPC_RMID, (struct shmid_ds *)NULL);

    return 0;
}

/*
 * Function		: RemoveAndCollapse()
 * Description	: This function gets rid of an element of a list collapsing the list.
 * Parameters	: int orderClient 			: indicates the element of clients registerd
 *                MasterList *pMasterList	: a list for registerd clients
 * 				  int semId					: a semaphore id
 * Returns		: int    					: if SUCESS, returns (0), if FIALURE, returns (-11)
 */
int RemoveAndCollapse(int orderClient, MasterList *pMasterList, int semId)
{
	int counter = 0;
	// all element except for the last one
	for(counter = (orderClient - 1); counter < (pMasterList->numberOfDCs - 1); counter++)
	{
		pMasterList->dc[counter].dcProcessID = pMasterList->dc[counter+1].dcProcessID;
		pMasterList->dc[counter].lastTimeHeardFrom = pMasterList->dc[counter+1].lastTimeHeardFrom;
	}

	// the last one
	pMasterList->dc[pMasterList->numberOfDCs -1].dcProcessID =	0;
	pMasterList->dc[pMasterList->numberOfDCs -1].lastTimeHeardFrom = 0;

	// decreasing in the total number of whole clients
	pMasterList->numberOfDCs--;						

	if(pMasterList->numberOfDCs < 0)
	{	// ERROR: unknown
		dlog(DATA_MONITOR, semId, "DR - ERROR: unknown");
		return -11;
	}

	return 0;
}

/*
 * Function		: OperationNonResponsive()
 * Description	: This function searches non-responsive clients, and then
 *                removes them from a list.
 * Parameters	: MasterList *pMasterList	: a list for registerd clients
 * 				  int semId					: a semaphore id
 * Returns		: nothing
 */
void OperationNonResponsive(MasterList *pMasterList, int semId)
{
	int orderNonResponsiveClient = 0;				// 0: no non-responsive client
	int startingPointConsecutive = 0;				// consecutive starting point for the counter
	char strLog[MAX_STRING_LOG];         			// for logging
	int counter = 0;
	time_t t;

	while(LOOP_FOREVER)						
	{
		// initialization
		orderNonResponsiveClient = 0;

		// searching for non-responsive clients
		for(counter = startingPointConsecutive; counter < pMasterList->numberOfDCs; counter++)
		{
			t = time(NULL);
			if((t - pMasterList->dc[counter].lastTimeHeardFrom) > TIME_OUT)	// more than 35 seconds     
			{	
				startingPointConsecutive = counter;
				orderNonResponsiveClient = counter + 1;
				break;
			} 
		}
		if(orderNonResponsiveClient > 0)
		{
			int dcProcessID = pMasterList->dc[orderNonResponsiveClient - 1].dcProcessID;

			// removing
			RemoveAndCollapse(orderNonResponsiveClient, pMasterList, semId);

			// logging the activity of removing the element from the list (non-responsive)
			sprintf (strLog, "DC-%02d [%d] removed from master list - NON-RESPONSIVE", 
				orderNonResponsiveClient, dcProcessID);
			dlog(DATA_MONITOR, semId, strLog);
		}
		else										// no non-responsive client
		{
			break;
		}
	}
}

/*
 * Function		: OperationIncomming()
 * Description	: This function is to manipulate a list on shared memory
 *                such as adding, update and removing.
 * Parameters	: MasterList *pMasterList   : a list for registerd clients
 *                MessageData sMsgData      : a list for registerd clients
 *                time_t t                  : indicates a calendar time
 * 				  int semId					: a semaphore id
 * Returns		: nothing
 */
void OperationIncomming(MasterList *pMasterList, MessageData sMsgData, time_t t, int semId)
{
	int counter = 0;
	char strLog[MAX_STRING_LOG];         			// for logging
	int orderIncomingClient = 0;					// the order of the registered client in the list, 
	                                        		// ... whose process id is same as the new one
	// searching for the client of the same process id
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

		// logging the activity of adding a new element to the list
		sprintf (strLog, "DC-%02d [%d] added to the master list - NEW DC - Status %d (%s)", 
			orderIncomingClient + 1, sMsgData.processID, sMsgData.msgStatus, kDescriptionStatus[sMsgData.msgStatus]);
		dlog(DATA_MONITOR, semId, strLog);
	}
	else											// registered client
	{
		if(sMsgData.msgStatus == OFF_LINE)			// status 6
		{
			// removing
			RemoveAndCollapse(orderIncomingClient, pMasterList, semId);

       		 // logging the activity of removing the element from the list (OFFLINE)
			sprintf (strLog, "DC-%02d [%d] has gone OFFLINE - removing from master-list", 
				orderIncomingClient, sMsgData.processID);
			dlog(DATA_MONITOR, semId, strLog);
		}
		else										// status 1 ~ 5
		{
			// updating
			pMasterList->dc[orderIncomingClient - 1].lastTimeHeardFrom = t;

       		 // logging the activity of updating the list
			sprintf (strLog, "DC-%02d [%d] updated in the master list - MSG RECEIVED - Satus %d (%s)", 
				orderIncomingClient, sMsgData.processID, sMsgData.msgStatus, kDescriptionStatus[sMsgData.msgStatus]);
			dlog(DATA_MONITOR, semId, strLog);
		}
	}
}