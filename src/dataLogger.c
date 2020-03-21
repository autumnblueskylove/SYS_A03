/*
 * File        : dataLogger.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to generate log files for DC, DR, and DX
 *               It will be stored in tmp folder with seperate name
 *               DC - /tmp/dataCreator.log
 *               DR - /tmp/dataMonitor.log
 *               DX - /tmp/dataCorruptor.log
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include "../inc/dataLogger.h"

// int main ()
// {
//     int semid;
//     // get semaphore ID
//     semid = semget (IPC_PRIVATE, 1, IPC_CREAT | 0666);
//     if(semid == -1)
//     {
//         printf("(Logger) Cannot get semid\n");
//         exit(1);
//     }
//     printf ("(Logger) semID is %d\n", semid);
//     if(semctl(semid, 0, SETALL, init_values) == -1)
//     {
//          printf("(Logger) Cannot initialize semid\n");
//         exit(2);
//     }
//     dlog(DATA_CORRUPTOR,semid,"Hello World");

//     printf ("(Logger) Release the semaphores\n");
//     semctl (semid, 0, IPC_RMID,0);
//     return 1;
// }

void dlog(int progID, int semid, char contents[255])
{
    char filePath[255] = {""};
    time_t 		    t;
    FILE            *fp;
    struct tm*      localTime; 


    // get semaphore ID
    switch (progID)
    {
        case DATA_CREATOR:
            {
                strcpy(filePath,"/tmp/dataCreator.log");
                break;
            }
        case DATA_MONITOR:
            {
                strcpy(filePath,"/tmp/dataMonitor.log");
                break;
            }
        case DATA_CORRUPTOR:
            {
                strcpy(filePath,"/tmp/dataCorruptor.log");
                break;
            }
    }
    if (semop (semid, &acquire_oper,1) == -1)
    {
        printf("(Logger) Cannot start critical region\n");
        exit(1);
    }

    if ((fp = fopen(filePath,"w+"))== NULL)
    {
        printf("(Logger) Cannot write to shared file\n");
        semctl ( semid, 0, IPC_RMID);
        exit (2);
    }

    //Time stamp for log file
    t = time(NULL);
    localTime = localtime(&t); 
    //printf("Local time and date: %s\n", asctime(localTime)); 
    fprintf(fp,"[%d-%d-%d %d:%d:%d] : ", (localTime->tm_year + 1900), (localTime->tm_mon + 1), localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);


    fprintf(fp,"%s\n",contents);
    fclose(fp);

    if(semop(semid, &release_oper,1) == -1)
    {
        printf("(Logger) Can't exit critical region\n");
        semctl ( semid, 0, IPC_RMID);
        exit (2);
    }

}