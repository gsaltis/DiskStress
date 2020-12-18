/*****************************************************************************
 * FILE NAME    : DiskInformation.h
 * DATE         : December 14 2020
 * PROJECT      : NONE
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _diskinformation_h_
#define _diskinformation_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <sys/statvfs.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "JSONOut.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Type : DiskInformation
 *****************************************************************************/
struct _DiskInformation
{
  unsigned long long                    freeBlocks;
  unsigned long long                    freeBytes;
  unsigned long long                    freeInodes;
  unsigned long long                    totalBytes;
  unsigned long long                    totalBlocks;
  unsigned long long                    totalInodes;
  unsigned long long                    blockSize;
};
typedef struct _DiskInformation DiskInformation;

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
JSONOut*
DiskInformationToJSON
();

void
DiskInformationDisplay
();

void
DiskInformationRefresh
();

void
DiskInformationInitialize
();

#endif // _diskinformation_h_
