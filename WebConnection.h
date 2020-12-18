/*****************************************************************************
 * FILE NAME    : WebConnection.h
 * DATE         : April 22 2020
 * PROJECT      : BDFB Simulator
 * COPYRIGHT    : Copyright (C) 2020 by Vertiv Company
 *****************************************************************************/
#ifndef _webconnection_h_
#define _webconnection_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "RPiBaseModules/mongoose.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Type : WebConnection
 *****************************************************************************/
struct _WebConnection
{
  struct mg_connection*                 connection;
  time_t                                lastReceiveTime;
  struct _WebConnection*                next;
  struct _WebConnection*                prev;   
};
typedef struct _WebConnection WebConnection;

/*****************************************************************************!
 * Exported Type : WebConnectionList
 *****************************************************************************/
struct _WebConnectionList
{
  WebConnection*                        first;
  WebConnection*                        last;
  pthread_mutex_t                       lock;
};
typedef struct _WebConnectionList WebConnectionList;

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
WebConnection*
WebConnectionCreate
(struct mg_connection* InConnection);

void
WebConnectionDestroy
(WebConnection* InConnection);

void
WebConnectionClose
(WebConnection* InConnection);

WebConnectionList*
WebConnectionListCreate
();

void
WebConnectionListAppend
(WebConnectionList* InList, WebConnection* InConnection);

void
WebConnectionListRemove
(WebConnectionList* InList, WebConnection* InConnection);

void
WebConnectionTimeUpdate
(WebConnection* InConnection, time_t InCurrentTime);

WebConnection*
WebConnectionListFind
(WebConnectionList* InList, struct mg_connection* InConnection);

int
WebConnectionListCount
(WebConnectionList* InList);

WebConnection*
WebConnectionListGetByIndex
(WebConnectionList* InList, int InIndex);

void
WebConnectionListDisplay
(WebConnectionList* InList);

#endif /* _webconnection_h_*/
