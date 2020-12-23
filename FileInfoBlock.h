/*****************************************************************************
 * FILE NAME    : FileInfoBlock.h
 * DATE         : December 15 2020
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _fileinfoblock_h_
#define _fileinfoblock_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "GeneralUtilities/String.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Type : FileInfoBlock
 *****************************************************************************/
struct _FileInfoBlock
{
  string                                filename;
  time_t                                filetime;
  uint64_t                              filesize;
  struct _FileInfoBlock*                next;
  struct _FileInfoBlock*                prev;
};
typedef struct _FileInfoBlock FileInfoBlock;

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
uint64_t
FileInfoBlockGetSize
(FileInfoBlock* InHead);

uint32_t
FileInfoBlockGetCount
(FileInfoBlock* InHead);

FileInfoBlock*
FileInfoBlockRemoveByName
(FileInfoBlock* InHead, string InFilename, bool InDestroy);

FileInfoBlock*
FileInfoBlockFindByName
(FileInfoBlock* InHead, string  InFileName);

bool
FileInfoBlockCreateFile
(FileInfoBlock* InInfoBlock);

void
FileInfoBlockDisplay
(FileInfoBlock* InInfoBlock);

FileInfoBlock*
FileInfoBlockRemove
(FileInfoBlock* InHead, FileInfoBlock* InInfoBlock, bool InDestroy);

FileInfoBlock*
FileInfoBlockAppend
(FileInfoBlock* InHead, FileInfoBlock* InInfoBlock);

void
FileInfoBlockDestroy
(FileInfoBlock* InInfoBlock);

FileInfoBlock*
FileInfoBlockCreate
(string InFilename, uint64_t InFileSize);

#endif /* _fileinfoblock_h_*/
