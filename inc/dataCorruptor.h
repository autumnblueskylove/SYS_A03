/*
 * File        : dataCorruptor.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataCorruptor.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include "../inc/shdMemory.h"

#define     KILL_DC         1                   // to kill a client
#define     DELETE_MSGQ     2                   // to delete message queue
#define     DO_NOTING       0

int wod(int shmid, int semid);