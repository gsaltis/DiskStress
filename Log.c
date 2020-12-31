/*****************************************************************************
 * FILE NAME    : Log.c
 * DATE         : December 29 2020
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
#include <time.h>
#include <stdarg.h>
#include <unistd.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "Log.h"
#include "GeneralUtilities/String.h"
#include "GeneralUtilities/MemoryManager.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
string
logFilename = NULL;

string
logFilenameDefault = "./LogFile.txt";

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/


/*****************************************************************************!
 * Function : NewFunction
 *****************************************************************************/
void
LogInitialize
()
{
  logFilename = StringCopy(logFilenameDefault);
}

/*****************************************************************************!
 * Function : LogSetFilename
 *****************************************************************************/
void
LogSetFilename
(string InFilename)
{
  if ( NULL == InFilename ) {
	return;
  }
  if ( logFilename ) {
	FreeMemory(logFilename);
  }

  logFilename = StringCopy(InFilename);
}

/*****************************************************************************!
 * Function : LogGetFilename
 *****************************************************************************/
string
LogGetFilename
()
{
  return logFilename;
}

/*****************************************************************************!
 * Function : LogAppend
 *****************************************************************************/
void
LogAppend
(string InMessage, ...)
{
  FILE*									file;
  time_t								t;
  struct tm*							t2;
  va_list                               ap;

  t = time(NULL);
  t2 = localtime(&t);

  if ( logFilename == NULL ) {
	return;
  }

  if ( InMessage == NULL ) {
	return;
  }

  file = fopen(logFilename, "a");
  if ( NULL == file ) {
	return;
  }
  va_start(ap, InMessage);

  fprintf(file, "%02d:%02d:%02d %02d/%02d/%04d : ", t2->tm_hour, t2->tm_min, t2->tm_sec, t2->tm_mon + 1, t2->tm_mday, t2->tm_year + 1900);
  vfprintf(file, InMessage, ap);
  fprintf(file, "\n");
  fclose(file);
}

/*****************************************************************************!
 * Function : LogFileRemove
 *****************************************************************************/
void
LogFileRemove
()
{
  if ( NULL == logFilename ) {
	return;
  }
  unlink(logFilename);
}

