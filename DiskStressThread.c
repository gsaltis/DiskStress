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

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "DiskStressThread.h"
#include "GeneralUtilities/ANSIColors.h"
#include "UserInputServerThread.h"
#include "FileInfoBlock.h"
#include "GeneralUtilities/MemoryManager.h"
#include "DiskInformation.h"

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
diskStressThreadAvailableBytes;

static uint64_t
diskStressMaxFileSize = 200000;

static uint64_t
diskStressThreadMaxFiles;

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
  diskStressThreadMaxFiles = diskStressThreadAvailableBytes / diskStressMaxFileSize;
  diskStressThreadMaxFiles++;

  FileInfoBlockSetCreate(diskStressThreadMaxFiles);
  printf("%sDisk Stress Thread       :%s started%s\n"
	     "  %sFiles Directory        : %s%s%s\n"
		 "  %sAvailable Bytes        : %s%llu%s\n" 
		 "  %sMax Files              : %s%llu%s\n", 
		 ColorGreen, ColorYellow, ColorReset, 
		 ColorCyan, ColorYellow, diskStressDirectory, ColorReset,
		 ColorCyan, ColorYellow, diskStressThreadAvailableBytes, ColorReset,
		 ColorCyan, ColorYellow, diskStressThreadMaxFiles, ColorReset);
  UserInputServerThreadStart();

  filesize = diskStressMaxFileSize;
  while ( true ) {
	index = rand();
	index %= diskStressThreadMaxFiles;
	infoBlock = FileInfoBlockGetBlock(index);
	if ( infoBlock ) {
	  if ( infoBlock->filesize == 0 ) {
		FileInfoBlockSetBlock(infoBlock, filesize);
		FileInfoBlockCreateFile(infoBlock, diskStressDirectory);
	  } else {
		FileInfoBlockRemoveFile(infoBlock);
		FileInfoBlockClearBlock(infoBlock);
	  }
	}
    sleep(5);
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

