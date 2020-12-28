/*****************************************************************************
 * FILE NAME    : FileInfoBlock.c
 * DATE         : December 15 2020
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "FileInfoBlock.h"
#include "GeneralUtilities/MemoryManager.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/

/*****************************************************************************!
 * Function : FileInfoBlockCreate
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockCreate
(string InFilename, uint64_t InFileSize)
{
  FileInfoBlock*                        infoBlock;
  if ( NULL == InFilename || 0 == InFileSize ) {
    return NULL;
  }

  infoBlock = (FileInfoBlock*)GetMemory(sizeof(FileInfoBlock));
  memset(infoBlock, 0x00, sizeof(FileInfoBlock));
  infoBlock->filename = StringCopy(InFilename);
  infoBlock->filesize = InFileSize;
  infoBlock->filetime = time(NULL);
  return infoBlock;
}

/*****************************************************************************!
 * Function : FileInfoBlockDestroy
 *****************************************************************************/
void
FileInfoBlockDestroy
(FileInfoBlock* InInfoBlock)
{
  if ( NULL == InInfoBlock ) {
    return;
  }
  FreeMemory(InInfoBlock->filename);
  FreeMemory(InInfoBlock);
}

/*****************************************************************************!
 * Function : FileInfoBlockAppend
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockAppend
(FileInfoBlock* InHead, FileInfoBlock* InInfoBlock)
{
  FileInfoBlock*                        infoBlock;
  if ( NULL == InInfoBlock ) {
    return NULL;
  }

  InInfoBlock->next = NULL;
  InInfoBlock->prev = NULL;
  
  if ( NULL == InHead ) {
    return InInfoBlock;
  }

  infoBlock = InHead;

  while ( infoBlock->next ) {
    infoBlock = infoBlock->next;
  }
  infoBlock->next = InInfoBlock;
  InInfoBlock->prev = infoBlock;
  return InHead;
}

/*****************************************************************************!
 * Function : FileInfoBlockRemove
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockRemove
(FileInfoBlock* InHead, FileInfoBlock* InInfoBlock, bool InDestroy)
{
  FileInfoBlock*                        prevInfoBlock;
  FileInfoBlock*                        infoBlock;
  if ( NULL == InHead || NULL == InInfoBlock ) {
    return InHead;
  }

  if ( InHead == InInfoBlock ) {
    infoBlock = InHead->next;
    if ( InDestroy ) {
      FileInfoBlockDestroy(InInfoBlock);
    }
    infoBlock->prev = NULL;
    return infoBlock;
  }

  prevInfoBlock = InHead;
  infoBlock = InHead->next;
  while ( infoBlock && infoBlock != InInfoBlock ) {
    prevInfoBlock = infoBlock;
    infoBlock = infoBlock->next;
  }

  if ( NULL == infoBlock ) {
    return InHead;
  }
  prevInfoBlock->next = infoBlock->next;
  if ( infoBlock->next ) {
    infoBlock->next->prev = prevInfoBlock;
  }
  if ( InDestroy ) {
    FileInfoBlockDestroy(infoBlock);
  }
  return InHead;
}


/*****************************************************************************!
 * Function : FileInfoBlockDisplay
 *****************************************************************************/
void
FileInfoBlockDisplay
(FileInfoBlock* InInfoBlock)
{
  struct tm*                            t;
  FileInfoBlock*                        infoBlock;
  if ( NULL == InInfoBlock ) {
    return;
  }
  for ( infoBlock = InInfoBlock ; infoBlock ; infoBlock = infoBlock->next ) {
    t = localtime(&infoBlock->filetime);
    printf("%34s  %10lld  %02d/%02d/%04d %02d:%02d:%02d\n",
           infoBlock->filename, infoBlock->filesize,
           t->tm_mon + 1, t->tm_mday, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
  }
}

/*****************************************************************************!
 * Function : FileInfoBlockCreateFile
 *****************************************************************************/
bool
FileInfoBlockCreateFile
(FileInfoBlock* InInfoBlock)
{
  int                                   i;
  FILE*                                 file;
  if ( NULL == InInfoBlock ) {
    return false;
  }

  file = fopen(InInfoBlock->filename, "wb");
  if ( NULL == file ) {
    return false;
  }
  
  for (i = 0; i < InInfoBlock->filesize; i++) {
    fputc(' ', file);
  }
  fclose(file);
  return true;
}

/*****************************************************************************!
 * Function : FileInfoBlockFindByName
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockFindByName
(FileInfoBlock* InHead, string  InFileName)
{
  FileInfoBlock*                        infoBlock;
  if ( NULL == InHead || NULL == InFileName ) {
    return NULL;
  }

  for ( infoBlock = InHead; infoBlock ; infoBlock = infoBlock->next ) {
    if ( StringEqual(InFileName, infoBlock->filename) ) {
      return infoBlock;
    }
  }
  return NULL;
}

/*****************************************************************************!
 * Function : FileInfoBlockRemoveByName
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockRemoveByName
(FileInfoBlock* InHead, string InFilename, bool InDestroy)
{
  FileInfoBlock*                        infoBlock;
  if ( NULL == InHead || NULL == InFilename ) {
    return NULL;
  }
  infoBlock = FileInfoBlockFindByName(InHead, InFilename);
  if ( NULL == infoBlock ) {
    return InHead;
  }
  return FileInfoBlockRemove(InHead, infoBlock, InDestroy);
}


/*****************************************************************************!
 * Function : FileInfoBlockGetCount
 *****************************************************************************/
uint32_t
FileInfoBlockGetCount
(FileInfoBlock* InHead)
{
  FileInfoBlock*                        infoBlock;
  int                                   i;
  if ( NULL == InHead ) {
    return 0;
  }

  i = 0;

  for ( infoBlock = InHead; infoBlock ; infoBlock = infoBlock->next ) {
    i++;
  }
  return i;
}


/*****************************************************************************!
 * Function : FileInfoBlockGetSize
 *****************************************************************************/
uint64_t
FileInfoBlockGetSize
(FileInfoBlock* InHead)
{
  FileInfoBlock*                        infoBlock;
  uint64_t                              size;
  if ( NULL == InHead ) {
    return 0;
  }

  size = 0;

  for ( infoBlock = InHead; infoBlock ; infoBlock = infoBlock->next ) {
    size += infoBlock->filesize;
  }
  return size;
}
