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
#include <time.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "GeneralUtilities/String.h"
#include "JSONOut.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
uint64_t
DiskStressGetFileSize
();

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

void
DiskStressThreadSetMaxFiles
(uint64_t InMaxFiles);

uint64_t
DiskStressThreadGetFilesRemovedCount
();

uint64_t
DiskStressThreadGetFilesCreatedCount
();

void
DiskStressThreadSetMaxFileSize
(uint64_t InMaxFileSize);

uint64_t
DiskStressThreadGetMaxFileSize
();

uint64_t
DiskStressThreadGetMaxFiles
();

time_t
DiskStressThreadGetStartTime
();

void
DiskStressThreadSetSleepPeriod
(int InSleepPeriod);

bool
DiskStressThreadValidateSleepPeriod
(int InSleepPeriod);

int
DiskStressThreadGetSleepPeriodMin
();

int
DiskStressThreadGetSleepPeriod
();

JSONOut*
DiskStressThreadStressInfoToJSON
();

int
DiskStressThreadGetHighPercent
();

int
DiskStressThreadGetLowPercent
();

void
DiskStressThreadSetLowPercent
(int InLowPercent);

void
DiskStressThreadSetHighPercent
(int InHighPercent);

#endif // _diskstressthread_h_
