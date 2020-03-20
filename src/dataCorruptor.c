/*
 * File        : dataCorruptor.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to corrupt data between DC and DR.
 *               Through killing DC process or deleting the message
 *               queue being used between the set of DC and DR, create
 *               ealternative (or exceptional) paths to test program.
 */

//cgl

#include "../inc/dataCorruptor.h"

int main() 
{
    int shmid;
    int mid;
    pid_t pid;
    int r;
    key_t shmem_key;
    MasterList *p;

    shmem_key = ftok(".", 16535);
    if(shmem_key == -1)
    {
        printf("(DX) Cannot allocate key\n");
        return 1;
    }
    
    for(int i = 0 ; i < 100; i++)
    {
        if((shmid = shmget(shmem_key, sizeof(MasterList),0))== -1)
        {
            sleep(10);
        }
        else
        {
            break;
        }     
    }

    p = (MasterList *)shmat (shmid, NULL, 0);
    if(p == NULL)
    {
        printf("(DX)Cannot attach to shared memory!\n");
    }

    //Run Wheel of Destruction until number of DC is zero
    while((p->numberOfDCs)>0)
    {
        //Step1: sleep for a random amount of time (10 ~ 30)
        r = (rand() % 21) + 10;
        sleep(r);
        //Step2 : check for the existance of the message queue
        mid = p->msgQueueID;
        if(mid == 0)
        {
            // print log file
            // detach from shared memory
            shmdt (p);
            // remove shared memory
            shmctl(shmid,IPC_RMID,0);
            exit(0);
        }

        r = rand() % 3;

        switch(r)
        {
            case DO_NOTING:
                {
                    // log do notiong
                    break;
                }
            case KILL_DC:
                {
                    // log kill dc
                    r = rand() % (p->numberOfDCs);
                    pid = (p->dc[r]).dcProcessID;
                    kill(pid, SIGHUP);
                    break;
                }
            case DELETE_MSGQ:
                {
                    // log delete message queue
                    msgctl (mid, IPC_RMID, NULL);
                    break;
                }
            default:
                {
                    break;
                }
        }


    }

    //printf("dataCorruptor: %d\n", i);
    
    return (0);
}