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
#include "../../Common/inc/dataLogger.h"

int main() 
{
    int             shmid;
    int             semid;
    int             r;
    key_t           shmem_key;
    

    // semaphore for logger
    // get semaphore ID
    semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semid == -1)
    {
        //printf("(DX) Cannot get semid\n");
        dlog(DATA_CORRUPTOR,semid,"(DX) Cannot get semid");
        exit(1);
    }
    //printf ("(DX) semID is %d\n", semid);
    if(semctl(semid, 0, SETALL, init_values) == -1)
    {
         //printf("(DX) Cannot initialize semid\n");
         dlog(DATA_CORRUPTOR,semid,"(DX) Cannot initialize semid!");
        exit(2);
    }

    // get key for shared memory
    shmem_key = ftok(".", 16535);
    if(shmem_key == -1)
    {
        //printf("(DX) Cannot allocate key\n");
        dlog(DATA_CORRUPTOR,semid,"(DX) Cannot allocate key!");
        // remove semaphore
        semctl (semid, 0, IPC_RMID,0);
        exit(3);
    }
    
    // 100 retry to get shared memory
    for(int i = 0 ; i < 100; i++)
    {
        // If shared memory has not yet created
        if((shmid = shmget(shmem_key, sizeof(MasterList),0))== -1)
        {
            // sleep for 10 sec and try again
            sleep(10);
        }
        else
        {
           while(1)
           {
               if(wod(shmid, semid) < 1)
               {
                   exit(4);
               }
           }
        }       
    }
    
    // remove semaphore
    semctl (semid, 0, IPC_RMID,0);
    
    return (0);
}


int wod(int shmid, int semid)
{
    int     actionNum = 0;
    int     r;
    int     mid;
    pid_t   pid;
    char    temp[255];
    MasterList  *p;

    // attach to shared memory
    p = (MasterList *)shmat (shmid, NULL, 0);
    if(p == NULL)
    {
        dlog(DATA_CORRUPTOR,semid,"(DX)Cannot attach to shared memory!");
        //printf("(DX)Cannot attach to shared memory!\n");
        return -1;
    }

    actionNum++;
    //Step1: sleep for a random amount of time (10 ~ 30)
    r = (rand() % 21) + 10;
    //printf("(Logger) sleep1 %d\n",r);
    sleep(r);
    //Step2 : check for the existance of the message queue
    mid = p->msgQueueID;
    //printf("(Logger) msgQueueID %d\n",mid);

    if(mid == 0)
    {
        // detach from shared memory
        shmdt (p);
        // remove shared memory
        shmctl(shmid,IPC_RMID,0);
        //printf("DX deteched that msgQ is gone - assuming DR/DCs done\n");
        // print log file
        dlog(DATA_CORRUPTOR,semid,"DX deteched that msgQ is gone - assuming DR/DCs done");
        // remove semaphore
        semctl (semid, 0, IPC_RMID,0);

        return 0;
    }

    // select random number for WOD
    r = rand() % 3;

    //WOD (Wheel of Destruction)
    switch(r)
    {
        case DO_NOTING:
            {
                // log do notiong
                //printf("do nothing\n");
                dlog(DATA_CORRUPTOR,semid,"do nothing");
                break;
            }
        case KILL_DC:
            {
                r = rand() % (p->numberOfDCs);
                pid = (p->dc[r]).dcProcessID;
                kill(pid, SIGHUP);
                // log kill dc
                //printf("WOD Action %02d - DC-%02d [%u] TERMINATED\n",actionNum, r, pid);
                sprintf(temp,"WOD Action %02d - DC-%02d [%u] TERMINATED",actionNum, r, pid);
                dlog(DATA_CORRUPTOR,semid,temp);
                break;
            }
        case DELETE_MSGQ:
            {
                // log delete message queue
                msgctl (mid, IPC_RMID, NULL);
                //printf("DX deleted the msgQ - the DR/DCs can't talk anymore - exiting\n");
                dlog(DATA_CORRUPTOR,semid,"DX deleted the msgQ - the DR/DCs can't talk anymore - exiting");
                break;
            }
        default:
            {
                break;
            }
    }
    return 1;
}