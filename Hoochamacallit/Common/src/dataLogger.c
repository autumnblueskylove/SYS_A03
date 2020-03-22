/*
 * File        : dataLogger.c
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This program is to generate log files for DC, DR, and DX
 *               using semaphore to prevent file crash.
 *               It will be stored in tmp folder with seperate name
 *               DC - /tmp/dataCreator.log
 *               DR - /tmp/dataMonitor.log
 *               DX - /tmp/dataCorruptor.log
 *               Open designated files and check semaphore 
 *               after writting log messages, close files and Exit semaphore
 */

#define __DATA_LOGGER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include "../../Common/inc/dataLogger.h"



/* =============================================================================*/
/* Name		: dlog                              								*/
/* PURPOSE  : to log each action of DC, DR, and DX Application					*/
/* INPUTS   : progID     int	      - program ID of DC, DR, and DX    		*/
/*            semid      int          - semaphore ID for cretical section       */
/*            contents   char array   - message for writting log files          */
/* RETURNS  : Nothing															*/
/* =============================================================================*/

/*
 * Function		: dlog()
 * Description	: This function is to log each action of DC, DR, and DX Application.
 * Parameters	: int progID 		: program ID of DC, DR, and DX  
 *                int semid 	    : semaphore ID for cretical section 
 * 				  char contents[]   : message for writting log files  
 * Returns		: nothing
 */
void dlog(int progID, int semid, char contents[MAX_STRING_LOG])
{
    char filePath[MAX_STRING_LOG] = {""};
    time_t 		    t;
    FILE            *fp;
    struct tm*      localTime; 


    // get program ID of DC, DR, and DX
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

    // Start critical region
    if (semop (semid, &acquire_operation,1) == -1)
    {
        printf("(Logger) Cannot start critical region\n");
        exit(1);
    }

    // open file for logging messages
    if ((fp = fopen(filePath,"a+"))== NULL)
    {
        printf("(Logger) Cannot write to shared file\n");
        semctl ( semid, 0, IPC_RMID);
        exit (2);
    }

    //Time stamp for log file
    t = time(NULL);
    localTime = localtime(&t); 
    //printf("Local time and date: %s\n", asctime(localTime)); 
    fprintf(fp,"[%04d-%02d-%02d %02d:%02d:%02d] : ", (localTime->tm_year + 1900), (localTime->tm_mon + 1), localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

    // write log message
    fprintf(fp,"%s\n",contents);

    // close flie
    fclose(fp);

    // exit semaphore
    if(semop(semid, &release_operation,1) == -1)
    {
        printf("(Logger) Can't exit critical region\n");
        semctl ( semid, 0, IPC_RMID);
        exit (2);
    }

}