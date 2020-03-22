/*
 * File        : dataCorruptor.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to corrupt data between DC and DR.
 *               Through killing DC process or deleting the message
 *               queue being used between the set of DC and DR, create
 *               ealternative (or exceptional) paths to test program
 *               using WOD (Wheel of Destruction) 
 *               - kill process
 *               - delete message queue
 *               - do nothing
 */

#include <sys/sem.h>
#include "../inc/dataCorruptor.h"
#include "../inc/dataLogger.h"

int main() 
{
    int             shmid;
    int             semid;
    int             r;
    key_t           shmem_key;
    

    // semaphore for logger
    // getting semaphore ID
    semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semid == -1)
    {
        dlog(DATA_CORRUPTOR,semid,"(DX) Cannot get semid");
        exit(1);
    }
    if(semctl(semid, 0, SETALL, init_values) == -1)
    {
         dlog(DATA_CORRUPTOR,semid,"(DX) Cannot initialize semid!");
        exit(2);
    }

    // getting key for shared memory
    shmem_key = ftok(".", 16535);
    if(shmem_key == -1)
    {
        dlog(DATA_CORRUPTOR,semid,"(DX) Cannot allocate key!");
        // removing semaphore
        semctl (semid, 0, IPC_RMID,0);
        exit(3);
    }
    
    // 100 times retring to get shared memory
    for(int i = 0 ; i < 100; i++)
    {
        // If shared memory has not yet created
        if((shmid = shmget(shmem_key, sizeof(MasterList),0))== -1)
        {
            // sleeping for 10 sec and try again
            sleep(10);
        }
        else
        {
           while(LOOP_FOREVER)
           {
               if(wod(shmid, semid) < 1)
               {
                   exit(4);
               }
           }
        }       
    }
    
    // removing semaphore
    semctl (semid, 0, IPC_RMID,0);
    
    return 0;
}

/*
 * Function		: wod()
 * Description	: This function is to log each action of DC, DR, and DX Application.
 * Parameters	: int shmid 		: shared memory ID
 *                int semid 	    : semaphore ID for cretical section 
 * Returns		: int
 */
int wod(int shmid, int semid)
{
    int     actionNum = 0;
    int     r;
    int     mid;
    pid_t   pid;
    char    temp[MAX_STRING_LOG];
    MasterList  *p;

    // attaching to shared memory
    p = (MasterList *)shmat (shmid, NULL, 0);
    if(p == NULL)
    {
        dlog(DATA_CORRUPTOR,semid,"(DX)Cannot attach to shared memory!");
        return -1;
    }

    actionNum++;

    // step1: sleeping for a random amount of time (10 ~ 30)
    srand(time(0));
    sleep((rand() % TIME_RANGE_SLEEP) + TIME_MIN_SLEEP);// integer: 10 to 30

    // step2 : checking for the existance of the message queue
    mid = p->msgQueueID;

    if(mid == 0)
    {
        // detaching from shared memory
        shmdt (p);
        // removing shared memory
        shmctl(shmid,IPC_RMID,0);
        // logging
        dlog(DATA_CORRUPTOR,semid,"DX deteched that msgQ is gone - assuming DR/DCs done");
        // removing semaphore
        semctl (semid, 0, IPC_RMID,0);

        return 0;
    }

    // selecting random number for WOD
    srand(time(0));
    r = rand() % 3;

    //WOD (Wheel of Destruction)
    switch(r)
    {
        case DO_NOTING:
            // logging, doing notiong
            dlog(DATA_CORRUPTOR,semid,"do nothing");
            break;
        case KILL_DC:
            srand(time(0));
            r = rand() % (p->numberOfDCs);
            pid = (p->dc[r]).dcProcessID;
            kill(pid, SIGHUP);
            // logging, killing dc
            sprintf(temp,"WOD Action %02d - DC-%02d [%u] TERMINATED",actionNum, r, pid);
            dlog(DATA_CORRUPTOR,semid,temp);
            break;
        case DELETE_MSGQ:
            // loging, deleting message queue
            msgctl (mid, IPC_RMID, NULL);
            dlog(DATA_CORRUPTOR,semid,"DX deleted the msgQ - the DR/DCs can't talk anymore - exiting");
            break;
        default:
            break;
    }

    return 1;
}