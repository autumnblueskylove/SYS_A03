/*
 * File        : dataReader.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataReader.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define LOOP_FOREVER                    1           // for an infinite loop
#define TIME_OUT                        35          // for non-responsive clients
#define FAILURE                         -1
#define MICRO_SECOND                    1000000
