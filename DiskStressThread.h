/*****************************************************************************
 * FILE NAME    : DiskStressThread.h
 * DATE         : December 13 2020
 * PROJECT      : NONE
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _diskstressthread_h_
#define _diskstressthread_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "GeneralUtilities/String.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
void
DiskStressThreadSetDirectory
(string InDirectoryName);

void
DiskStressFileList
();

uint32_t
DiskStressGetFileCount
();

string
DiskStressGenFilename
();

pthread_t
DiskStressGetThreadID
();

void
DiskStressThreadStart
();

void
DiskStressThreadInit
();

#endif // _diskstressthread_h_
