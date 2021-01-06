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
#include <unistd.h>
#include <errno.h>

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
FileInfoBlock*
fileInfoBlockSet = NULL;

int
fileInfoBlockSetSize = 0;

string
fileInfoBlockPrefix = "DiskFileInfo";

/*****************************************************************************!
 * Function : FileInfoBlockSetCreate
 *****************************************************************************/
void
FileInfoBlockSetCreate
(int InSetSize)
{
  int                                   i, n;

  if ( InSetSize == 0 ) {
	return;
  }

  n = InSetSize * sizeof(FileInfoBlock);
  fileInfoBlockSet = (FileInfoBlock*)GetMemory(n);
  memset(fileInfoBlockSet, 0x00, n);
  fileInfoBlockSetSize = InSetSize;

  for ( i = 0 ; i < InSetSize ; i++ ) {
    fileInfoBlockSet[i].index = i + 1;
  }
}

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
()
{
  struct tm*                            t;
  FileInfoBlock*                        infoBlock;
  int                                   i;

  for ( i = 0 ; i < fileInfoBlockSetSize ; i++ ) {
	if ( fileInfoBlockSet[i].filesize == 0 ) {
	  continue;
	}
	infoBlock = &(fileInfoBlockSet[i]);
    t = localtime(&infoBlock->filetime);
    printf("%6d  %10lld  %02d/%02d/%04d %02d:%02d:%02d\n",
		   infoBlock->index, 
           infoBlock->filesize,
           t->tm_mon + 1, t->tm_mday, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
  }
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
()
{
  int									i, count;
  count = 0;
  for (i = 0; i < fileInfoBlockSetSize ; i++ ) {
	if ( fileInfoBlockSet[i].filesize ) {
	  count++;
	}
  }
  return count;
}

/*****************************************************************************!
 * Function : FileInfoBlockGetSize
 *****************************************************************************/
uint64_t
FileInfoBlockGetSize
(FileInfoBlock* InHead)
{
  uint64_t                              size;
  int                                   i;

  size = 0;

  for (i = 0; i < fileInfoBlockSetSize ; i++ ) {
	if ( fileInfoBlockSet[i].filesize ) {
	  size += fileInfoBlockSet[i].filesize;
	}
  }
  return size;
}

/*****************************************************************************!
 * Function : FileInfoBlockGetBlock
 *****************************************************************************/
FileInfoBlock*
FileInfoBlockGetBlock
(int InIndex)
{
  if ( InIndex < 0 || InIndex >= fileInfoBlockSetSize ) {
	return NULL;
  }
  return &(fileInfoBlockSet[InIndex]);
}

/*****************************************************************************!
 * Function : FileInfoBlockSetBlock
 *****************************************************************************/
void
FileInfoBlockSetBlock
(FileInfoBlock* InBlock, int InSize)
{
  if ( InBlock == NULL || InSize == 0 ) {
	return;
  }
  InBlock->filetime = time(NULL);
  InBlock->filesize = InSize;
  
}

/*****************************************************************************!
 * Function : FileInfoBlockClearBlock
 *****************************************************************************/
void
FileInfoBlockClearBlock
(FileInfoBlock* InBlock)
{
  if ( InBlock == NULL ) {
	return;
  }
  InBlock->filesize = 0;
  InBlock->filetime = 0;
}

/*****************************************************************************!
 * Function : FileInfoBlockCreateFile
 *****************************************************************************/
void
FileInfoBlockCreateFile
(FileInfoBlock* InBlock, string InDirectory)
{
  char									filename[32];
  FILE*                                 file;
  int                                   i;
  string                                s;

  if ( InBlock == NULL ) {
	return;
  }

  sprintf(filename, "%s%08d", fileInfoBlockPrefix, InBlock->index);
  s = StringConcat(InDirectory, filename);
  file = fopen(s, "wb");
  if ( NULL == file ) {
	return;
  }
  for (i = 0; i < InBlock->filesize ; i++ ) {
	fputc(' ', file);
  }
  FreeMemory(s);
  fclose(file);
}

/*****************************************************************************!
 * Function : FileInfoBlockRemoveFile
 *****************************************************************************/
void
FileInfoBlockRemoveFile
(FileInfoBlock* InBlock, string InDirectory)
{
  char									filename[32];
  string                                s;

  if ( NULL == InBlock ) {
	return;
  }

  if ( InBlock->filesize == 0 ) {
	return;
  }

  sprintf(filename, "%s%08d", fileInfoBlockPrefix, InBlock->index);
  s = StringConcat(InDirectory, filename);
  if ( unlink(s) ) {
    fprintf(stderr, "Could not remove file %s : %s\n", s, strerror(errno));
  }
  FreeMemory(s);
}

/*****************************************************************************!
 * Function : FileInfoBlockSetGetMap
 *****************************************************************************/
void
FileInfoBlockSetGetMap
(int* InMapSize, uint64_t** InMap)
{
  int                                   i, size;
  int                                   bitIndex, byteIndex;
  int                                   byteCount;
  uint64_t*                             map;
  uint64_t                              b;
  
  byteCount = fileInfoBlockSetSize / 64;
  if ( fileInfoBlockSetSize % 64 > 0 ) {
	byteCount++;
  }

  size = sizeof(uint64_t) * byteCount; 
  map = (uint64_t*)GetMemory(size);
  memset(map, 0x00, size);
  bitIndex = 0;
  byteIndex = 0;
  b = 0;

  for ( i = 0 ; i < fileInfoBlockSetSize ; i++ ) {
    if ( fileInfoBlockSet[i].filesize > 0 ) {
      b |= (1 << bitIndex); 
    }
	bitIndex++;
	if ( bitIndex == 64 ) {
	  map[byteIndex] = b;
	  byteIndex++;
	  bitIndex = 0;
	  b = 0;
	}
  }
  if ( bitIndex > 0 ) {
	map[byteIndex] = b;
	byteIndex++;
  }
  *InMap = map;
  *InMapSize= byteIndex;
}

/*****************************************************************************!
 * 
 *****************************************************************************/
int
FileInfoBlockSetGetSize
()
{
  return fileInfoBlockSetSize;
}

/*****************************************************************************!
 * Function : FileInfoBlockSetToJSON 
 *****************************************************************************/
JSONOut*
FileInfoBlockSetToJSON
()
{
  JSONOut*						        jsonOut;
  int                                   i;
  string                                s;

  jsonOut = JSONOutCreateObject("filemapinfo");

  s = (string)GetMemory(fileInfoBlockSetSize + 1);

  for ( i  = 0 ; i < fileInfoBlockSetSize ; i ++ ) {
	if ( fileInfoBlockSet[i].filesize > 0 ) {
	  s[i] = '1';
	} else {
	  s[i] = '0';
	}
  }
  s[fileInfoBlockSetSize] = 0;

  JSONOutObjectAddObject(jsonOut, JSONOutCreateString("map", s));
  JSONOutObjectAddObject(jsonOut, JSONOutCreateInt("mapsize", fileInfoBlockSetSize));
  return jsonOut;
}

