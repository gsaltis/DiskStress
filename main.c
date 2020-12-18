/*****************************************************************************
 * FILE NAME    : main.c
 * DATE         : December 13 2020
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
#include <pthread.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "main.h"
#include "UserInputServerThread.h"
#include "WebSocketServerThread.h"
#include "DiskStressThread.h"
#include "HTTPServerThread.h"
#include "DiskInformation.h"

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
 * Function : main
 *****************************************************************************/
int
main(int argc, char**argv)
{
  DiskInformationInitialize();
  DiskInformationDisplay();
  UserInputServerThreadInit();
  HTTPServerThreadInit();
  WebSocketServerThreadInit();
  DiskStressThreadInit();
  
  HTTPServerThreadStart();
  
  pthread_join(UserInputGetThreadID(), NULL);
  pthread_join(HTTPServerGetThreadID(), NULL);
  pthread_join(WebSocketServerGetThreadID(), NULL);
  pthread_join(DiskStressGetThreadID(), NULL);
  
  return EXIT_SUCCESS;
}
