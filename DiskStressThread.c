/*****************************************************************************
 * FILE NAME    : DiskStressThread.c
 * DATE         : December 13 2020
 * PROJECT      : NONE
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "DiskStressThread.h"
#include "GeneralUtilities/ANSIColors.h"
#include "UserInputServerThread.h"
#include "FileInfoBlock.h"
#include "GeneralUtilities/MemoryManager.h"
#include "DiskInformation.h"
#include "Log.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static string
diskStressDirectory = NULL;

static string
diskStressDirectoryDefault = "./DiskStressFiles/";

static string
DiskStressFilenameBase = "DiskStressFile";

static long long
DiskStressFilenameIndex = 0;

static FileInfoBlock*
diskStressFileHead;

static pthread_t
DiskStressThreadID;

static uint64_t
diskStressMaxFileSize = 500000;

static uint64_t
diskStressThreadAvailableBytes;

static uint64_t
diskStressThreadMaxFiles = 0;

static uint64_t
diskStressThreadFilesRemovedCount = 0;

static uint64_t
diskStressThreadFilesCreatedCount = 0;

static int
diskStressThreadSleepPeriod = 250000;

static int
diskStressThreadSleepPeriodMin = 10000;

static time_t
diskStressThreadStartTime = 0;

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void*
DiskStressThread
(void* InParameters);

static void
DiskStressThreadCleanFiles
();

/*****************************************************************************!
 * Function : DiskStressThreadInit

 *****************************************************************************/
void
DiskStressThreadInit
()
{
  diskStressFileHead = NULL;
  diskStressDirectory = StringCopy(diskStressDirectoryDefault);
}

/*****************************************************************************!
 * Function : DiskStressThreadStart
 *****************************************************************************/
void
DiskStressThreadStart
()
{
  DiskStressThreadCleanFiles();
  DiskInformationInitialize();

  if ( pthread_create(&DiskStressThreadID, NULL, DiskStressThread, NULL) ) {
    fprintf(stderr, "%sCould not start \"DiskStress Thread\"%s\n", ColorRed, ColorReset);
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : DiskStressThread
 *****************************************************************************/
void*
DiskStressThread
(void* InParameters)
{
  long long                             filesize;
  FileInfoBlock*                        infoBlock;
  int                                   index;

  diskStressThreadAvailableBytes = DiskInformationGetAvailableBytes();

  // If the user has not already set the maximum number of files, set it now
  if ( diskStressThreadMaxFiles == 0 ) {
    diskStressThreadMaxFiles = diskStressThreadAvailableBytes / diskStressMaxFileSize;
    diskStressThreadMaxFiles++;
  }
  FileInfoBlockSetCreate(diskStressThreadMaxFiles);
  LogAppend("Disk Stress Thread      : started");
  LogAppend("  Files Directory       : %s", diskStressDirectory);
  LogAppend("  Available Bytes       : %lld", diskStressThreadAvailableBytes);
  LogAppend("  Max Files             : %lld", diskStressThreadMaxFiles);
  LogAppend("  Max File Size         : %lld", diskStressMaxFileSize);

  printf("%sDisk Stress Thread       :%s started%s\n"
	     "  %sFiles Directory        : %s%s%s\n"
		 "  %sAvailable Bytes        : %s%llu%s\n" 
		 "  %sMax Files              : %s%llu%s\n"
		 "  %sMax File Size          : %s%llu%s\n",

		 ColorGreen, ColorYellow, ColorReset, 
		 ColorCyan, ColorYellow, diskStressDirectory, ColorReset,
		 ColorCyan, ColorYellow, diskStressThreadAvailableBytes, ColorReset,
		 ColorCyan, ColorYellow, diskStressThreadMaxFiles, ColorReset,
		 ColorCyan, ColorYellow, diskStressMaxFileSize, ColorReset);
  UserInputServerThreadStart();

  filesize = diskStressMaxFileSize;
  diskStressThreadStartTime = time(NULL);
  while ( true ) {
	index = rand();
	index %= diskStressThreadMaxFiles;
	infoBlock = FileInfoBlockGetBlock(index);
	if ( infoBlock ) {
	  if ( infoBlock->filesize == 0 ) {
		FileInfoBlockSetBlock(infoBlock, filesize);
		FileInfoBlockCreateFile(infoBlock, diskStressDirectory);
		diskStressThreadFilesCreatedCount++;
	  } else {
		FileInfoBlockRemoveFile(infoBlock, diskStressDirectory);
		FileInfoBlockClearBlock(infoBlock);
		diskStressThreadFilesRemovedCount++;
	  }
	}
    usleep(diskStressThreadSleepPeriod);
    DiskInformationRefresh();
  }
}

/*****************************************************************************!
 * Function : DiskStressGetThreadID
 *****************************************************************************/
pthread_t
DiskStressGetThreadID
()
{
  return DiskStressThreadID;
}

/*****************************************************************************!
 * Function : DiskStressGenFilename
 *****************************************************************************/
string
DiskStressGenFilename
()
{
  int                                   n;
  string                                s;
  
  DiskStressFilenameIndex++;

  n = strlen(DiskStressFilenameBase) + 16;
  s = (string)GetMemory(n);
  sprintf(s, "%s%lld", DiskStressFilenameBase, DiskStressFilenameIndex);
  return s;
}

/*****************************************************************************!
 * Function : DiskStressGetFileCount
 *****************************************************************************/
uint32_t
DiskStressGetFileCount
()
{
  return FileInfoBlockGetCount(diskStressFileHead);
}


/*****************************************************************************!
 * Function : DiskStressFileList
 *****************************************************************************/
void
DiskStressFileList
()
{
  FileInfoBlockDisplay(diskStressFileHead);
}

/*****************************************************************************!
 * Function : DiskStressThreadSetDirectory
 *****************************************************************************/
void
DiskStressThreadSetDirectory
(string InDirectoryName)
{
  if ( diskStressDirectory ) {
    FreeMemory(diskStressDirectory);
  }
  diskStressDirectory = StringCopy(InDirectoryName);
  if ( StringEndsWith(diskStressDirectory, "/") ) {
    return;
  }
  diskStressDirectory = StringConcatTo(diskStressDirectory, "/");
}

/*****************************************************************************!
 * Function : DiskStressGetFileSize
 *****************************************************************************/
uint64_t
DiskStressGetFileSize
()
{
  return FileInfoBlockGetSize(diskStressFileHead);
}

/*****************************************************************************!
 * Function : DiskStressThreadCleanFiles
 *****************************************************************************/
void
DiskStressThreadCleanFiles
()
{
  DIR*	                                dir;
  struct dirent*						entry;
  string								fullname;
  int 									n;

  dir = opendir(diskStressDirectory);
  if ( NULL == dir ) {
	return;
  }

  n = 0;
  for ( entry = readdir(dir) ; entry ; entry = readdir(dir) ) {
	if ( StringEqualsOneOf(entry->d_name, ".", "..", NULL) ) {
	  continue;
	}
	fullname = StringConcat(diskStressDirectory, entry->d_name);
    if ( unlink(fullname) ) {
	  fprintf(stderr, "%sCould remove file %s%s : %s%s%s\n", ColorRed, ColorBrightRed, fullname, ColorRed, strerror(errno), ColorReset);
	  exit(EXIT_FAILURE);
	}
	FreeMemory(fullname);
	n++;
  }
  if ( n > 1 ) {
	printf("%sFiles removed            : %s%d%s\n", ColorGreen, ColorYellow, n, ColorReset);
  }
}

/*****************************************************************************!
 * Function : DiskStressThreadSetMaxFiles
 *****************************************************************************/
void
DiskStressThreadSetMaxFiles
(uint64_t InMaxFiles)
{
  if ( InMaxFiles == 0 ) {
	return;
  }

  diskStressThreadMaxFiles = InMaxFiles;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetFilesRemovedCount
 *****************************************************************************/
uint64_t
DiskStressThreadGetFilesRemovedCount
()
{
  return diskStressThreadFilesRemovedCount;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetFilesCreatedCount 
 *****************************************************************************/
uint64_t
DiskStressThreadGetFilesCreatedCount
()
{
  return diskStressThreadFilesCreatedCount;
}

/*****************************************************************************!
 * Function : DiskStressThreadSetMaxFileSize
 *****************************************************************************/
void
DiskStressThreadSetMaxFileSize
(uint64_t InMaxFileSize)
{
  if ( InMaxFileSize == 0 ) {
	return;
  }

  diskStressMaxFileSize = InMaxFileSize;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetMaxFileSize
 *****************************************************************************/
uint64_t
DiskStressThreadGetMaxFileSize
()
{
  return diskStressMaxFileSize;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetMaxFiles
 *****************************************************************************/
uint64_t
DiskStressThreadGetMaxFiles
()
{
  return diskStressThreadMaxFiles;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetStartTime
 *****************************************************************************/
time_t
DiskStressThreadGetStartTime
()
{
  return diskStressThreadStartTime;
}

/*****************************************************************************!
 * Function : DiskStressThreadSetSleepPeriod
 *****************************************************************************/
void
DiskStressThreadSetSleepPeriod
(int InSleepPeriod)
{
  if ( InSleepPeriod < diskStressThreadSleepPeriodMin ) {
    return;
  }
  diskStressThreadSleepPeriod = InSleepPeriod;
}

/*****************************************************************************!
 * Function : DiskStressThreadValidateSleepPeriod
 *****************************************************************************/
bool
DiskStressThreadValidateSleepPeriod
(int InSleepPeriod)
{
  return InSleepPeriod >= diskStressThreadSleepPeriodMin;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetSleepPeriod
 *****************************************************************************/
int
DiskStressThreadGetSleepPeriod
()
{
  return diskStressThreadSleepPeriod;
}

/*****************************************************************************!
 * Function : DiskStressThreadGetSleepPeriodMin
 *****************************************************************************/
int
DiskStressThreadGetSleepPeriodMin
()
{
  return diskStressThreadSleepPeriodMin;
}

