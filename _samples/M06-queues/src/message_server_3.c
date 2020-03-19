/*
 * message server application sample
 *
 * This application will receive commands from multiple clients,
 * (LOW, MEDIUM and HIGH priority) and in turn, respond to them ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/message_struct_3.h"


int main (void)
{
	key_t 		message_key;
	int 		mid = -1; // message ID
	int 		rc;	// return code from message processing
	int 		exitServer = FALSE;
	int		menuChoice;
	int		x;
	int 		msgPriority;
	char		buffer[100];
	MSGENVELOPE 	msg;

	message_key = ftok (".", 4578);
	if (message_key == -1) 
	{ 
		printf ("(SERVER) Cannot allocate key\n");
		return 1;
	}	/* endif */


	/*
	 * if message queue exists, use it, or
	 * allocate a new one
	 */

	if ((mid = msgget (message_key, 0)) == -1) 
	{
		printf ("(SERVER) No queue available, create!\n");

		/*
		 * nope, let's create one (user/group read/write perms)
		 */

		mid = msgget (message_key, IPC_CREAT | 0660);
		if (mid == -1) 
		{
			printf ("(SERVER) Cannot allocate a new queue!\n");
			return 2;
		}
	}

	printf ("(SEVER) Our message queue ID is %d\n", mid);
	while(! exitServer)
	{
	  printf ("1 ... Receive a LOW priority job\n");
	  printf ("2 ... Receive a MEDIUM priority job\n");
	  printf ("3 ... Receive a HIGH priority job\n");
	  printf ("5 ... Exit Program\n");
	  printf ("\n");
	  printf ("choice: ");
	  gets (buffer);
	  x = atoi (buffer);

	  switch (x) 
	  {
	    case LOW_CHOICE:
	    case MED_CHOICE:
	    case HIGH_CHOICE:
		{
		  /* determine the priority of the message to receive */
		  msgPriority = LOW_PRIORITY;
		  if(x == MED_CHOICE) msgPriority = MEDIUM_PRIORITY;
		  if(x == HIGH_CHOICE) msgPriority = HIGH_PRIORITY;

		  rc = msgrcv (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), msgPriority, 0);
		  if (rc == -1) break;
  		  
		  printf ("(SERVER) Received: msgType: %ld\n", msg.type);
		  printf ("         This message came from CLIENT PID=%d and was jobNumber %d\n", msg.data.requestingClient, msg.data.jobNumber);

		  printf("          ... Response being sent ...\n");
		
		  /* we need to 'address' this response to the specific client */
		  msg.type = msg.data.requestingClient;
		  msg.data.result = -1 * msg.data.jobNumber;	// result is -jobNumber ...
		  break;
		}
	    case KILL_SERVER:
		{
		  exitServer = TRUE;
		  break;
		}
	
	  }

	  /* if we are not leaving - then send the response ... */
	  if(exitServer == FALSE)
	  {
		rc = msgsnd (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0);
		if (rc == -1) break;
	  }
	}

	/*
	 * our server is done, so shut down the queue
	 */

	printf ("(SERVER) Exiting ... removing msgQ and leaving ...\n");
	msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);
	return 0;
}

