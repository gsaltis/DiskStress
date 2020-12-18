/*****************************************************************************
 * FILE NAME    : WebConnection.c
 * DATE         : April 22 2020 
 * PROJECT      : BDBF Simulator
 * COPYRIGHT    : Copyright (C) 2020 by Vertiv Company
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "WebConnection.h"
#include "GeneralUtilities/MemoryManager.h"
#include "GeneralUtilities/String.h"
#include "RPiBaseModules/mongoose.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/

/*****************************************************************************!
 * Function : WebConnectionCreate
 *****************************************************************************/
WebConnection*
WebConnectionCreate
(struct mg_connection* InConnection)
{
  WebConnection*                        connection;

  connection = (WebConnection*)GetMemory(sizeof(WebConnection));
  memset(connection, 0x00, sizeof(WebConnection));
  connection->lastReceiveTime = time(NULL);
  connection->next = NULL;
  connection->prev = NULL;
  connection->connection = InConnection;
  return connection;
}

/*****************************************************************************!
 * Function : WebConnectionDestroy
 *****************************************************************************/
void
WebConnectionDestroy
(WebConnection* InConnection)
{
  if ( NULL == InConnection ) {
    return;
  }
  FreeMemory(InConnection);
}

/*****************************************************************************!
 * Function : WebConnectionClose
 *****************************************************************************/
void
WebConnectionClose
(WebConnection* InConnection)
{
  if ( NULL == InConnection ) {
    return;
  }
  mg_send_websocket_frame(InConnection->connection, WEBSOCKET_OP_CLOSE,
                          NULL, 0);
}

/*****************************************************************************!
 * Function : WebConnectionListCreate
 *****************************************************************************/
WebConnectionList*
WebConnectionListCreate
()
{
  WebConnectionList*                    list;

  list = (WebConnectionList*)GetMemory(sizeof(WebConnectionList));
  pthread_mutex_init(&list->lock, NULL);
  list->first = NULL;
  list->last  = NULL; 
  return list;
}

/*****************************************************************************!
 * Function : WebConnectionListAppend
 *****************************************************************************/
void
WebConnectionListAppend
(WebConnectionList* InList, WebConnection* InConnection)
{
  if ( NULL == InList || NULL == InConnection ) {
    return;
  }
  pthread_mutex_lock(&InList->lock);
  if ( InList->first ) {
    InList->last->next = InConnection;
    InConnection->prev = InList->last;
    InList->last = InConnection;
  } else {
    InList->first = InConnection;
    InList->last = InConnection;
  }
  pthread_mutex_unlock(&InList->lock);
}

/*****************************************************************************!
 * Function : WebConnectionListRemove
 *****************************************************************************/
void
WebConnectionListRemove
(WebConnectionList* InList, WebConnection* InConnection)
{
  WebConnection*                        n;
  WebConnection*                        p;

  pthread_mutex_lock(&InList->lock);
  if ( NULL == InList || NULL == InConnection ) {
    pthread_mutex_unlock(&InList->lock);
    return;
  }
  if ( InConnection == InList->first ) {
    if ( InList->first->next == NULL ) {
      InList->first = NULL;
      InList->last  = NULL;
    } else {
      InList->first = InList->first->next;
    }
  } else if ( InConnection == InList->last ) {
    if ( InList->last->prev == NULL ) {
      InList->first = NULL;
      InList->last  = NULL;
    } else {
      InList->last = InList->last->prev;
      InList->last->next = NULL;
    }
  } else {
    p = InConnection->prev;
    n = InConnection->next;
    n->prev = p;
    p->next = n;
  }
  pthread_mutex_unlock(&InList->lock);
}

/*****************************************************************************!
 * Function : WebConnectionTimeUpdate
 *****************************************************************************/
void
WebConnectionTimeUpdate
(WebConnection* InConnection, time_t InCurrentTime)
{
  if ( InConnection == NULL || InCurrentTime == 0 ) {
    return;
  }
  InConnection->lastReceiveTime = InCurrentTime;
}

/*****************************************************************************!
 * Function : WebConnectionListFind
 *****************************************************************************/
WebConnection*
WebConnectionListFind
(WebConnectionList* InList, struct mg_connection* InConnection)
{
  WebConnection*                        connection;

  pthread_mutex_lock(&InList->lock);
  if ( NULL == InList || NULL == InConnection ) {
    pthread_mutex_unlock(&InList->lock);
    return NULL;
  }
  for ( connection = InList->first; connection; connection = connection->next ) {
    if ( connection->connection == InConnection ) {
      pthread_mutex_unlock(&InList->lock);
      return connection;
    }
  }
  pthread_mutex_unlock(&InList->lock);
  return NULL;
}

/*****************************************************************************!
 * Function : WebConnectionListCount
 *****************************************************************************/
int
WebConnectionListCount
(WebConnectionList* InList)
{
  WebConnection*                        connection;
  int                                   count;
 
  pthread_mutex_lock(&InList->lock);
  if ( InList == NULL ) {
    pthread_mutex_unlock(&InList->lock);
    return 0;
  }
 
  count = 0; 
  for ( connection = InList->first; connection; connection = connection->next ) {
    count++;
  }
  pthread_mutex_unlock(&InList->lock);
  return count;
}

/*****************************************************************************!
 * Function : WebConnectionListGetByIndex
 *****************************************************************************/
WebConnection*
WebConnectionListGetByIndex
(WebConnectionList* InList, int InIndex)
{
  int                                   count;
  WebConnection*                        connection;

  pthread_mutex_lock(&InList->lock);
  if ( InList == NULL || InIndex < 0 ) {
    pthread_mutex_unlock(&InList->lock);
    return NULL;
  }

  count = 0;

  for ( connection = InList->first; connection; connection = connection->next ) {
    if ( count == InIndex ) {
      pthread_mutex_unlock(&InList->lock);
      return connection;
    }
    count++;
  }
  pthread_mutex_unlock(&InList->lock);
  return NULL;
}
     
/*****************************************************************************!
 * Function : WebConnectionListDisplay
 *****************************************************************************/
void
WebConnectionListDisplay
(WebConnectionList* InList)
{
  int                                   i;
  WebConnection*                        connection;
  char                                  s2[24];
  uint32_t                              address;
  int                                   n;
  string                                s;
  char                                  timeString[65];
  time_t                                t;

  pthread_mutex_lock(&InList->lock);
  i = 1;
  connection = InList->first;
  t = time(NULL);
  while (connection) {
    address = connection->connection->sa.sin.sin_addr.s_addr;
    sprintf(s2, "%d.%d.%d.%d", (address & 0xFF), ((address >> 8) & 0xFF), ((address >> 16) & 0xFF), ((address >> 24) & 0xFF));
    n = printf("%2d : %s:%d ", i, s2, connection->connection->sa.sin.sin_port);
    n = 32 - n;
    s = StringFill(' ', n);
    printf("%s", s);
    FreeMemory(s);
    strftime(timeString, 64, "%m/%d/%G %H:%M:%S", localtime(&connection->lastReceiveTime));
    printf("%s %d\n", timeString, (int)(t - connection->lastReceiveTime));
    connection = connection->next;
    i++;
  }
  pthread_mutex_unlock(&InList->lock);
}
 
