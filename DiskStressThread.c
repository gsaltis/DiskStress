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
diskStressDirectoryDefault = "./";

static string
DiskStressFilenameBase = "DiskStressFile";

static long long
DiskStressFilenameIndex = 0;

static FileInfoBlock*
diskStressFileHead;

static pthread_t
DiskStressThreadID;

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void*
DiskStressThread
(void* InParameters);

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
  string                                fullFilename;
  long long                             filesize;
  string                                filename;
  int                                   i;
  FileInfoBlock*                        infoBlock;
  printf("%s\"Disk Stress Thread\" started%s : %s\n", ColorGreen, ColorReset, diskStressDirectory);
  UserInputServerThreadStart();
  i = 0;
  while ( true ) {
    i++;
    if ( i <= 100 ) {
      filename = DiskStressGenFilename();
      fullFilename = StringConcat(diskStressDirectory, filename);
      filesize = 100;
      infoBlock = FileInfoBlockCreate(fullFilename, filesize);
      FileInfoBlockCreateFile(infoBlock);
      FreeMemory(filename);
      FreeMemory(fullFilename);
      diskStressFileHead = FileInfoBlockAppend(diskStressFileHead, infoBlock);
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
