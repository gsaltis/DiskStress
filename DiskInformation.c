/*****************************************************************************
 * FILE NAME    : DiskInformation.c
 * DATE         : December 14 2020
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
#include <sys/statvfs.h>
#include <errno.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "DiskInformation.h"
#include "GeneralUtilities/String.h"
#include "GeneralUtilities/NumericTypes.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static DiskInformation
DiskInfoRoot = { } ;

static string
DiskRootName = "/";

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/

/*****************************************************************************!
 * Function : DiskInformationInitialize
 *****************************************************************************/
void
DiskInformationInitialize
()
{
  DiskInformationRefresh();
}

/*****************************************************************************!
 * Function : DiskInformationRefresh
 *****************************************************************************/
void
DiskInformationRefresh
()
{
  struct statvfs                        vbuf;
  
  if ( statvfs(DiskRootName, &vbuf) ) {
    fprintf(stderr, "Could not get the volume information for %s : %s\n", DiskRootName, strerror(errno));
    exit(EXIT_FAILURE);
  }

  DiskInfoRoot.blockSize    = vbuf.f_bsize;
  DiskInfoRoot.totalBytes   = vbuf.f_blocks;
  DiskInfoRoot.totalBytes  *= DiskInfoRoot.blockSize;

  DiskInfoRoot.freeBytes    = vbuf.f_bfree;
  DiskInfoRoot.freeBytes   *= DiskInfoRoot.blockSize;

  DiskInfoRoot.totalInodes  = vbuf.f_files;
  DiskInfoRoot.freeInodes   = vbuf.f_favail;
  DiskInfoRoot.totalBlocks  = vbuf.f_blocks;
  DiskInfoRoot.freeBlocks   = vbuf.f_bfree;
}

/*****************************************************************************!
 * Function : DiskInformationDisplay
 *****************************************************************************/
void
DiskInformationDisplay
()
{
  printf(" Total Bytes : %lld\n", DiskInfoRoot.totalBytes);
  printf("Total Inodes : %lld\n", DiskInfoRoot.totalInodes);
  printf("Total Blocks : %lld\n", DiskInfoRoot.totalBlocks);
  printf("  Free Bytes : %lld\n", DiskInfoRoot.freeBytes);
  printf(" Free Blocks : %lld\n", DiskInfoRoot.freeBlocks);
  printf(" Free Inodes : %lld\n", DiskInfoRoot.freeInodes);
}

/*****************************************************************************!
 * Function : DiskInformationToJSON
 *****************************************************************************/
JSONOut*
DiskInformationToJSON
()
{
  JSONOut*                              jsonOut;
  char                                  s1[32];

  jsonOut = JSONOutCreateObject("diskinfo");
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("totalbytes", DiskInfoRoot.totalBytes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("totalinodes", DiskInfoRoot.totalInodes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("totalblocks", DiskInfoRoot.totalBlocks));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("blocksize", DiskInfoRoot.blockSize));

  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("freebytes", DiskInfoRoot.freeBytes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("freeinodes", DiskInfoRoot.freeInodes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("freeblocks", DiskInfoRoot.freeBlocks));

  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("usedbytes", DiskInfoRoot.totalBytes - DiskInfoRoot.freeBytes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("usedinodes", DiskInfoRoot.totalInodes - DiskInfoRoot.freeInodes));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateLongLong("usedblocks", DiskInfoRoot.totalBlocks - DiskInfoRoot.freeBlocks));
  
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("totalbytesstring", ConvertLongLongToCommaString(DiskInfoRoot.totalBytes, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("freebytesstring", ConvertLongLongToCommaString(DiskInfoRoot.freeBytes , s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("totalblocksstring", ConvertLongLongToCommaString(DiskInfoRoot.totalBlocks, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("freeblocksstring", ConvertLongLongToCommaString(DiskInfoRoot.freeBlocks, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("freeblocksstring", ConvertLongLongToCommaString(DiskInfoRoot.freeBlocks, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("totalinodesstring", ConvertLongLongToCommaString(DiskInfoRoot.totalInodes, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("freeinodesstring", ConvertLongLongToCommaString(DiskInfoRoot.freeInodes, s1)));

  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("usedbytesstring", ConvertLongLongToCommaString(DiskInfoRoot.totalBytes - DiskInfoRoot.freeBytes, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("usedinodesstring", ConvertLongLongToCommaString(DiskInfoRoot.totalInodes - DiskInfoRoot.freeInodes, s1)));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("usedblocksstring", ConvertLongLongToCommaString(DiskInfoRoot.totalBlocks - DiskInfoRoot.freeBlocks, s1)));
  
  return jsonOut;
}

/*****************************************************************************!
 * Function : DiskInformationGetAvailableBytes
 *****************************************************************************/
uint64_t
DiskInformationGetAvailableBytes
()
{
  return DiskInfoRoot.freeBytes;
}
