/*
 * message client application sample
 *
 * This application will talk to a message server
 * and request some simple operations of it. This is
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
	int mid; // message ID
	int rc;	// return code from message processing
	int done = 0;
	int count = 0;
	MSGENVELOPE msg;
	char buffer[100];
	int x;
	char exitClient  = FALSE;

	/*
	 * open the same message key/queue as server
	 */

	message_key = ftok (".", 'Z');
	if (message_key == -1) 
	{ 
	  printf ("(CLIENT) Cannot allocate key\n");
	  return 1;
	}	/* endif */


	/* if message queue exists, use it - otherwise, assume that the server hasn't been
	   started - so start it and try again */
	while ((mid = msgget (message_key, 0)) == -1) 
	{
		pid_t pid;

		count++;
		if (count == 10) 
		{
		  printf ("(CLIENT) Unable to establish connection to queue!\n");
		  return 2;
		}	/* endif */

		/*
		 * not available yet ... fork/exec() our server now
		 */
		
		pid = fork();
		if (pid == -1) 
		{
		  printf ("(CLIENT) Cannot fork!\n");
		  return 3;
		}	/* endif */

		if (pid == 0)	// this is the CHILD process
		{
		   // in context of child, launch the server app!!
		   execl ("./msgServer2", "msgServer2", NULL);
		   exit(0);	// after launching the process, the child is done
		}
		else
		{
		   // otherwise, parent ... sleep for 2 second and try again
		   sleep (2);
		}
	}	

	while(! exitClient)
	{

	  printf ("(CLIENT) My PID is %d\n", getpid());
	  printf ("(CLIENT) Message queue ID is %d\n", mid);

	  printf ("1 ... Date\n");
	  printf ("2 ... Time\n");
	  printf ("3 ... Random between two values\n");
	  printf ("4 ... Add two values\n");
	  printf ("5 ... Terminate Server\n");
	  printf ("6 ... Exit Program\n");
	  printf ("\n");
	  printf ("Please enter your choice >> ");
	    gets (buffer);
	    x = atoi (buffer);

	  msg.type = CLIENT_2_MSG;

	  switch (x) 
	  {
	    case GET_DATE:
	      {
		msg.data.message_command = GET_DATE;
		break;
	      }

	    case GET_TIME:
	      {
		msg.data.message_command = GET_TIME;
		break;
	      }

	    case GET_RND_NUM:
	      {
		msg.data.message_command = GET_RND_NUM;
		printf ("\n  >> Choose lower-bound of random number: ");
		  gets (buffer);
		  msg.data.data1 = atoi (buffer);
		printf ("  >> Choose upper-bound of random number: ");
		  gets (buffer);
		  msg.data.data2 = atoi (buffer);
		break;
	      }

	    case ADD_VALUES:
	      {
		msg.data.message_command = ADD_VALUES;
		printf ("\n  >> Specify one of the values to add: ");
		  gets (buffer);
		  msg.data.data1 = atoi (buffer);
		printf ("  >> Specify the other value to add: ");
		  gets (buffer);
		  msg.data.data2 = atoi (buffer);
		break;
	      }

	    case TERM_SERVER:
	      {
		msg.data.message_command = KILL_SERVER;
		break;
	      }

	    case TERM_CLIENT:
	      {
		exitClient = TRUE;
		break;
	      }
	  }	/* switch */


	  if(!exitClient)
	  {
	      // send the message structure to the server
	      rc = msgsnd (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0);
	      if (rc == -1) 
	      {
		  printf ("(CLIENT) Send error!\n");
		  return 4;
	      }
	      
	      // now wait for a response ...
	      rc = msgrcv (mid, (void *)&msg, sizeof (SAMPLEMESSAGE), 0, 0);
	      if (rc == -1) 
	      {
		  printf ("(CLIENT) Receive error!\n");
		  return 5;
	      }
	      
	      printf ("\n(CLIENT)    >> Result Status Code is: %d\n", msg.data.result);
	      printf ("(CLIENT)    >> Result String is: %s\n\n", msg.data.resultstring);
	    }
	}
	return 0;
}

