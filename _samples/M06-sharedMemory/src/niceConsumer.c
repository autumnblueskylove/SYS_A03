/*
 * shared memory test app #2
 * 
 * This app will locate an existing block
 * of shared memory, and attach to it.
 * It will present a menu of options: option
 * 1 will allow you to see the latest
 * coin toss result, and option 2 will tell you
 * the 1000 toss history.
 *
 * This information will be pulled from the shared
 * information. The main app generates data once
 * a second, and we will read from that shared
 * block whenever our 2nd app is run.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int done = 0;

/*
 * definition of our shared memory
 * structure. Both applications will
 * have access to all this memory!
 */

typedef struct tagCOINTOSSDATA 
{
  int pos;	// goes from 0 to 299 inclusive
  char cointoss[300];
} COINTOSSDATA;

int main (void)
{
	int shmid;
	key_t shmem_key;
	COINTOSSDATA *p;
	char buffer[128];
	int x, nextToss;
	int heads, tails;

	/* get the secret key_code to the area of shared memory we will allocate */	
	shmem_key = ftok (".", 'M');
	if (shmem_key == -1) 
	{ 
	  printf ("(CONSUMER) Cannot allocate key\n");
	  return 1;
	}


	/*
	 * if shared mem exists, use it, or
	 * allocate a new block
	 */

	if ((shmid = shmget (shmem_key, sizeof (COINTOSSDATA), 0)) == -1) 
	{
		// note: you could try setting up your own fork/exec here
		// to launch the 2nd app!!

		printf ("(CONSUMER) Shared-Memory doesn't exist. run the PRODUCER!\n");
		return 2;
	}

	printf ("(CONSUMER) Our Shared-Memory ID is %d\n", shmid);

	/*
	 * now attach to our shared memory
	 */

	p = (COINTOSSDATA *)shmat (shmid, NULL, 0);
	if (p == NULL) 
	{
	  printf ("(CONSUMER) Cannot attach to Shared-Memory!\n");
	  return 3;
	}

	nextToss = 0;
	while(!done)
	{
	  printf ("\n\nMenu\n====\n");
	  printf ("1 ... Get Current Producer Toss\n");
	  printf ("2 ... Get Next Toss to be Read\n");
	  printf ("3 ... Get Toss Statistics\n");
	  printf ("5 ... Quit\n");
	  printf ("\nEnter your choice: ");
	  gets (buffer);
	  x = atoi (buffer);
	  
	  switch (x) 
	  {
	   case 1:
	   {
	        /* read from the current position in the shared memory */
	        x = p->pos;
		x--;
		if  (x < 0) x = 299;      /* wrap around */
		printf ("(CONSUMER) Toss # %d is: %c\n", x, p->cointoss[x]);
		break;
	   }
	   case 2:
	   {
	     printf("(CONSUMER) Toss # %d is: %c\n", nextToss, p->cointoss[nextToss]);
	     nextToss++;
	     break;
	   }
	   case 3:
	   {
	        /* walk through the entire shared memory and count the # of H and T in the buffer */
		heads = 0;
		tails = 0;
		for (x = 0; x < 300; x++) 
		{
		  if (p->cointoss[x] == 'H') heads++;
		  if (p->cointoss[x] == 'T') tails++;
		}
		printf ("(CONSUMER) Statistics :\n           ==========\n\tHeads: %d times\n", heads);
		printf ("\tTails: %d times\n", tails);
		break;
	   }
	   case 5:
	   {
	     done = 1;
	   }
	  }
	}

	/*
	 * detach and clean up our client resources
	 */

	shmdt (p);
}


