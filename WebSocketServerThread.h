/*****************************************************************************
 * FILE NAME    : WebSocketServerThread.h
 * DATE         : December 14 2020
 * PROJECT      : NONE
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _websocketserverthread_h_
#define _websocketserverthread_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "GeneralUtilities/String.h"

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
WebSocketServerGetThreadID
();

void
WebSocketServerThreadStart
();

void
WebSocketServerThreadInit
();

void
WebSocketServerCreateInfoScript
();

void
WebSocketServerSetDirectory
(string InWWWDirectory);

#endif // _websocketserverthread_h_
