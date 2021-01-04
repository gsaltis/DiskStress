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
  int									index;
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
();

uint32_t
FileInfoBlockGetCount
();

FileInfoBlock*
FileInfoBlockRemoveByName
(FileInfoBlock* InHead, string InFilename, bool InDestroy);

FileInfoBlock*
FileInfoBlockFindByName
(FileInfoBlock* InHead, string  InFileName);

void
FileInfoBlockCreateFile
(FileInfoBlock* InInfoBlock, string InDirectory);

void
FileInfoBlockDisplay
();

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

void
FileInfoBlockSetCreate
(int InSetSize);

void
FileInfoBlockRemoveFile
(FileInfoBlock* InBlock);

void
FileInfoBlockCreateFile
(FileInfoBlock* InBlock, string InDirectory);

void
FileInfoBlockClearBlock
(FileInfoBlock* InBlock);

void
FileInfoBlockSetBlock
(FileInfoBlock* InBlock, int InSize);

FileInfoBlock*
FileInfoBlockGetBlock
(int InIndex);

void
FileInfoBlockSetGetMap
(int* InMapSize, uint64_t** InMap);

int
FileInfoBlockSetGetSize
();

#endif /* _fileinfoblock_h_*/
