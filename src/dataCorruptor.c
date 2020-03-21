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

//cgl
#include <sys/sem.h>
#include "../inc/dataCorruptor.h"

extern unsigned short init_values[];
extern struct sembuf acquire_operation;
extern struct sembuf release_operation;

int main() 
{
    int shmid;
    int mid;
    int semid;
    pid_t pid;
    int r;
    key_t shmem_key;
    MasterList *p;
    char temp[255];
    int actionNum = 0;

    // get key for shared memory
    shmem_key = ftok(".", 16535);
    if(shmem_key == -1)
    {
        printf("(DX) Cannot allocate key\n");
        return 1;
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
            break;
        }
        // Cannot find 10sec * 100 times  then exit program.
        if(i == 99)
        {
            exit(0);
        }     
    }

    // attach to shared memory
    p = (MasterList *)shmat (shmid, NULL, 0);
    if(p == NULL)
    {
        printf("(DX)Cannot attach to shared memory!\n");
    }

    // semaphore for logger
    // get semaphore ID
    semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if(semid == -1)
    {
        printf("(Logger) Cannot get semid\n");
        exit(1);
    }
    printf ("(Logger) semID is %d\n", semid);
    if(semctl(semid, 0, SETALL, init_values) == -1)
    {
         printf("(Logger) Cannot initialize semid\n");
        exit(2);
    }

    //Run Wheel of Destruction until number of DC is zero
    while((p->numberOfDCs)>0)
    {
        actionNum++;
        //Step1: sleep for a random amount of time (10 ~ 30)
        r = (rand() % 21) + 10;
        sleep(r);
        //Step2 : check for the existance of the message queue
        mid = p->msgQueueID;
        if(mid == 0)
        {
            // detach from shared memory
            shmdt (p);
            // remove shared memory
            shmctl(shmid,IPC_RMID,0);
            // print log file
            dlog(DATA_CORRUPTOR,semid,"DX deteched that msgQ is gone - assuming DR/DCs done");
            // remove semaphore
            semctl (semid, 0, IPC_RMID,0);

            exit(3);
        }

        // select random number for WOD
        r = rand() % 3;

        //WOD (Wheel of Destruction)
        switch(r)
        {
            case DO_NOTING:
                {
                    // log do notiong
                    dlog(DATA_CORRUPTOR,semid,"do nothing");
                    break;
                }
            case KILL_DC:
                {
                    r = rand() % (p->numberOfDCs);
                    pid = (p->dc[r]).dcProcessID;
                    kill(pid, SIGHUP);
                    // log kill dc
                    sprintf(temp,"WOD Action %2d - DC-%2d [%u] TERMINATED",r, actionNum, pid);
                    dlog(DATA_CORRUPTOR,semid,temp);
                    break;
                }
            case DELETE_MSGQ:
                {
                    // log delete message queue
                    msgctl (mid, IPC_RMID, NULL);
                    dlog(DATA_CORRUPTOR,semid,"DX deleted the msgQ - the DR/DCs can't talk anymore - exiting");
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    
    // remove semaphore
    semctl (semid, 0, IPC_RMID,0);
    
    return (0);
}