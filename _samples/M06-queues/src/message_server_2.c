/*
 * message server application sample
 *
 * This application will receive commands from a client,
 * and provide responses (integer and string). It is 
 * a demonstration of simple message passing support in Unix
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/message_struct_2.h"


int main (void)
{
	key_t message_key;
	int mid = -1; // message ID
	int rc;	// return code from message processing
	int done = 0;
	MSGENVELOPE msg;

	message_key = ftok (".", 'Z');
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
		}	/* endif */
	}	/* endif */

	printf ("(SEVER) Our message queue ID is %d\n", mid);

	/*
	 * we now have a message queue ID, use it to
	 * send/receive messages
	 */

	while (!done) 
	{
		rc = msgrcv (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0, 0);
		if (rc == -1) break;

		printf ("(SERVER) Received: type: %ld command: %d\n", msg.type, msg.data.message_command);

		switch (msg.data.message_command) 
		{
		  case KILL_SERVER:
		  {
		    // quit server
		    msg.data.result = 0xFFFFFFFF;	// flag to indicate we're quitting
		    sprintf (msg.data.resultstring, "%d", msg.data.result);
		    done = 1;
		    break;
		  }

		  case GET_DATE:	
		  {
		    // let's hard-code the date value ... 
		    msg.data.result = 0;
		    sprintf (msg.data.resultstring, "2020-02-17");
		    break;
		  }

		  case GET_TIME:
		  {
		    // let;s hard-code the time value
		    msg.data.result = 0;
		    sprintf (msg.data.resultstring, "20:00:00");
		    break;
		  }

		  case GET_RND_NUM:
		  {
		    // random number between X and Y
		    msg.data.result = (rand() % (msg.data.data2 - msg.data.data1)) + msg.data.data1;
		    sprintf (msg.data.resultstring, "%d", msg.data.result);
		    break;
		  }

		  case ADD_VALUES:
		  {
		    // sum of X and Y
		    msg.data.result = msg.data.data1 + msg.data.data2;
		    sprintf (msg.data.resultstring, "%d", msg.data.result);
		    break;
		  }
		
		  default:
		  {
			break;
		  }
		}

		// send result back
	
		rc = msgsnd (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0);
		if (rc == -1) break;
	}

	/* our server is done, so shut down the queue */

	msgctl (mid, IPC_RMID, (struct msqid_ds *)NULL);

	return 0;
}

