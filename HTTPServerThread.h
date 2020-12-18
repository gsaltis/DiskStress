/*****************************************************************************
 * FILE NAME    : HTTPServerThread.h
 * DATE         : December 13 2020
 * PROJECT      : NONE
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _httpserverthread_h_
#define _httpserverthread_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
pthread_t
HTTPServerGetThreadID
();

void
HTTPServerThreadStart
();

void
HTTPServerThreadInit
();

#endif // _httpserverthread_h_
