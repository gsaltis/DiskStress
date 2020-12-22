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
#include "GeneralUtilities/String.h"
#include "GeneralUtilities/MemoryManager.h"
#include "GeneralUtilities/ANSIColors.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static string
mainProgramName = "diskstress";

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
MainDisplayHelp
();

void
MainProcessCommandLine
(int argc, char** argv);

/*****************************************************************************!
 * Function : main
 *****************************************************************************/
int
main(int argc, char**argv)
{
  // Let the threads set there defaults before we process the command line with
  //   options that may override them.
  UserInputServerThreadInit();
  HTTPServerThreadInit();
  WebSocketServerThreadInit();
  DiskStressThreadInit();

  MainProcessCommandLine(argc, argv);
  DiskInformationInitialize();

  HTTPServerThreadStart();
  
  pthread_join(UserInputGetThreadID(), NULL);
  pthread_join(HTTPServerGetThreadID(), NULL);
  pthread_join(WebSocketServerGetThreadID(), NULL);
  pthread_join(DiskStressGetThreadID(), NULL);
  
  return EXIT_SUCCESS;
}

/*****************************************************************************!
 * Function : MainProcessCommandLine
 *****************************************************************************/
void
MainProcessCommandLine
(int argc, char** argv)
{
  int                                   i;
  string                                command;

  for (i = 1; i < argc; i++) {
    command = argv[i];
    if ( StringEqualsOneOf(command, "-h", "--help", NULL) ) {
      MainDisplayHelp();
      exit(EXIT_FAILURE);
    }
    if ( StringEqualsOneOf(command, "-d", "--directory", NULL) ) {
      i++;
      if ( i == argc ) {
        fprintf(stderr, "%s\"%s\"%s %srequires a directory name%s\n", ColorRed, command, ColorReset, ColorYellow, ColorReset);
        MainDisplayHelp();
        exit(EXIT_FAILURE);
      }
      DiskStressThreadSetDirectory(argv[i]);      
      continue;
    }
    fprintf(stderr, "%s\"%s\"%s %sis not a valid command%s\n", ColorRed, command, ColorReset, ColorYellow, ColorReset);
    MainDisplayHelp();
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : MainDisplayHelp
 *****************************************************************************/
void
MainDisplayHelp
()
{
  fprintf(stdout, "Usage : %s {options}\n", mainProgramName);
  fprintf(stdout, "        %s-h, --help      %s: %sDisplay this message%s\n", ColorGreen, ColorReset, ColorYellow, ColorReset);
  fprintf(stdout, "        %s-d, --directory %s: %sSpecify the file base directory%s\n", ColorGreen, ColorReset, ColorYellow, ColorReset);
}
