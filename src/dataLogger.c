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



#include "../inc/dataLogger.h"

void dlog(int progID, int semid, char *contents);

int main ()
{
    int semid;
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
    dlog(1,semid,"Hello World");

    printf ("(Logger) Release the semaphores\n");
    semctl (semid, 0, IPC_RMID,0);
    return 1;
}

void dlog(int progID, int semid, char *contents)
{
    char filePath[255] = {""};
    FILE *fp;


    // get semaphore ID
    switch (progID)
    {
        case 1:
            {
                strcpy(filePath,"/tmp/dataCreator.log");
                break;
            }
        case 2:
            {
                strcpy(filePath,"/tmp/dataMonitor.log");
                break;
            }
        case 3:
            {
                strcpy(filePath,"/tmp/dataCorruptor.log");
                break;
            }
    }
    if (semop (semid, &acquire_operation,1) == -1)
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
    fprintf(fp,"%s",contents);
    fclose(fp);

    if(semop(semid, &release_operation,1) == -1)
    {
        printf("(Logger) Can't exit critical region\n");
        semctl ( semid, 0, IPC_RMID);
        exit (2);
    }

}