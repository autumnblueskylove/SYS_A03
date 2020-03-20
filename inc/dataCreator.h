﻿/*
 * File        : dataCreator.h
 * Project     : Hoochmacallit
 * By          : Hyungbum Kim and Charng Gwon Lee
 * Date        : March 21, 2020
 * Description : This header file contains all the required definitions for the
 *               source file of dataCreator.c.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum
{
    OK,                             // means Everything is OKAY
    FAILURE_HYDRAULIC_FAILURE,      // means Hydraulic Pressure Failure
    FAILURE_SAFETY_BUTTON,          // means Safety Button Failure
    NO_RAW_MATERIAL,                // means No Raw Material in the Process
    OUT_OPERATING_TEMP,             // means Operating Temperature Out of Range
    ERROR_OPERATOR,                 // means Operator Error
    OFF_Line                        // means Machine is Off-line
} MESSAGESTATUS;