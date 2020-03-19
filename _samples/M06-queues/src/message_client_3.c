/*
 * message client application sample
 *
 * This application will talk to a message server
 * and be able to send LOW, MEDIUM and HIGH priority
 * "jobs" to it ... as well, the server will respond
 * with specific messages back to the client for the
 * different jobs ...
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
	key_t	 	message_key;
	pid_t		myPID;
	int 		mid; // message ID
	int	 	rc;	// return code from message processing
	int 		done = 0;
	int 		count = 0;
	int 		x;
	int		menuChoice;
	char 		exitClient  = FALSE;
	char		buffer[100];
	MSGENVELOPE 	msg;

	/* this client needs to know their own PID */
	myPID = getpid();

	/*
	 * open the same message key/queue as server
	 */

	message_key = ftok (".", 4578);
	if (message_key == -1) 
	{ 
	  printf ("(CLIENT) Cannot allocate key\n");
	  return 1;
	}	/* endif */


	/*
	 * the client will wait around for upto 5 tries of checking
	 * for the message queue (to give the server time to start)
	 */
	printf("(CLIENT) Checking for message queue ...\n");
	while ((mid = msgget (message_key, 0)) == -1) 
	{
		count++;
		if (count == 5) 
		{
		  printf ("(CLIENT (%d)) Unable to establish connection to queue!\n", myPID);
		  return 2;
		}	/* endif */

		/*
		 * not available yet ... check in 2 seconds
		 */
		printf("(CLIENT) Queue not available ... sleeping for 2 seconds ...\n");
		sleep (2);
	}	/* end while */

	/* use count to keep track of the # of jobs sent to the server ... */
	count = 0;

	printf ("(CLIENT) My PID is %d\n", myPID);
	printf ("(CLIENT) Message queue ID is %d\n", mid);
	while(! exitClient)
	{
	  printf ("1 ... Send a LOW priority job\n");
	  printf ("2 ... Send a MEDIUM priority job\n");
	  printf ("3 ... Send a HIGH priority job\n");
	  printf ("4 ... Check for SERVER response\n");
	  printf ("5 ... Exit Program\n");
	  printf ("\n");
	  printf ("Please specify your choice: ");
	    gets (buffer);
	    x = atoi (buffer);

	  switch (x) 
	  {
	    case LOW_CHOICE:
	    case MED_CHOICE:
	    case HIGH_CHOICE:
	      {
		count++;
		if(x == LOW_CHOICE) msg.type = LOW_PRIORITY;
		if(x == MED_CHOICE) msg.type = MEDIUM_PRIORITY;
		if(x == HIGH_CHOICE) msg.type = HIGH_PRIORITY;
		msg.data.jobNumber = count;
		msg.data.requestingClient = myPID;

		/* this menuchoice is a sending */
		menuChoice = SEND;
		break;
	      }

	    case RESP_CHOICE:
	      {
		/* this menuchoice is a receive */
		menuChoice = RECEIVE;
		break;
	      }

	    case KILL_CLIENT:
	      {
		exitClient = TRUE;
		break;
	      }
	  }

	
	  if(!exitClient)
	  {
		/* if we are sending a message, then do so ... */
		if(menuChoice == SEND )
		{
		  rc = msgsnd (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0);
		  if (rc == -1) 
		  {
		    printf ("(CLIENT) Send error!\n");
		    return 4;
		  }
		  printf("(CLIENT) Successfully send message ...\n");
		}

		if(menuChoice == RECEIVE)
		{
		  /* let's get a response from the server that has been directed at this client */
                  /*  - by specifically asking for a message of type myPID                      */
		  rc = msgrcv (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), myPID, 0);
		  if (rc == -1) 
		  {
		    printf ("(CLIENT) Receive error!\n");
		    return 5;
		  }
		  printf ("\n(CLIENT)   >> For job #%d, the result is: %d\n", msg.data.jobNumber, msg.data.result);
		}	      
	  }
	}
	return 0;
}

