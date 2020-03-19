/*
 * shared memory test app #1
 * 
 * This app will allocate a block of shared memory and set up a circular buffer of 
 * "coin toss" information. We will generate random coin toss data once every second, 
 * and store in the next avialable spot in the circular buffer either an 'H' or a 'T'. 
 * The buffer will have 1000 bytes of storage for the H and T values, and when the app
 * reaches the last entry of the array, it will reset back to the first to create
 * the circular buffer concept.
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
	int x;

	/* get the secret key_code to the area of shared memory we will allocate */
	shmem_key = ftok (".", 'M');
	if (shmem_key == -1) 
	{ 
	  printf ("(PRODUCER) Cannot allocate key\n");
	  return 1;
	}


	/*
	 * if shared mem exists, use it, otherwise allocate the block of memory
	 */

	if ((shmid = shmget (shmem_key, sizeof (COINTOSSDATA), 0)) == -1) 
	{

		/*
		 * nope, let's create one (user/group read/write perms)
		 */

		printf ("(PRODUCER) No Shared-Memory currently available - so create!\n");
		shmid = shmget (shmem_key, sizeof (COINTOSSDATA), IPC_CREAT | 0660);
		if (shmid == -1) 
		{
		  printf ("(PRODUCER) Cannot allocate a new memory!\n");
		  return 2;
		}
	}

	printf ("(PRODUCER) Our Shared-Memory ID is %d\n", shmid);

	/* now allow the PRODUCER (server) to attach to our shared memory and begin
	   producing data to be read ... */

	p = (COINTOSSDATA *)shmat (shmid, NULL, 0);
	if (p == NULL) 
	{
	  printf ("(PRODUCER) Cannot attach to shared memory!\n");
	  return 3;
	}


	/*
	 * initialize our data to blanks, as initially we have no randomized coin 
         * toss data! and initialize the position to the first location
	 * in our circular buffer.
	 */

	for (x = 0; x < 300; x++) 
	{
	  p->cointoss[x] = 0;
	}

	/* initialize the position variable to point to the head */
	p->pos = 0;

	while (done < 300) 
	{
	  int r;

	  sleep (1);

	  /*
	   * generate random number between 0 and 99 (100 diff numbers)
	   */
	  r = rand() % 100;

	  /*
	   * if in the lower 1/2 of range, it's a HEADS,
	   * else TAILS
	   */
	  if (r < 50)
	  {
		p->cointoss[p->pos] = 'H';
		printf("(PRODUCER)    ... Tossing # %d of Coin -- HEADS ! \n", p->pos);
	  }
	  else
	  {
		p->cointoss[p->pos] = 'T';
		printf("(PRODUCER)    ... Tossing # %d of Coin -- TAILS !\n", p->pos);
	  }

	  /*
	   * increment our position, and wrap at end
	   * of buffer
	   */
	  done++;
	  p->pos++;
	  if (p->pos == 300) 
	  {
	    p->pos = 0;
	    printf("(PRODUCER) Reached the end of the buffer - wrapping index and leaving ...\n");
	    done = 300;
	  }

	  /* FAILSAFE */
	  if((done == 300) && (p->pos > 300))
	  {
	    printf("(PRODUCER) Something bad happened to the circular buffer\n");
	    done=300;
	  }
	    
	}

	/*
	 * detach and clean up our resources
	 */

	  printf("(PRODUCER) Detaching from Shared-Memory\n");
	  shmdt (p);


	  printf("(PRODUCER) Removing the Shared-Memory resource\n");
	  shmctl (shmid, IPC_RMID, 0);
}


