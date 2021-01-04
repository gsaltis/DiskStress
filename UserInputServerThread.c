/*****************************************************************************
 * FILE NAME    : UserInputServerThread.c
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
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "UserInputServerThread.h"
#include "GeneralUtilities/String.h"
#include "RPiBaseModules/linenoise.h"
#include "HTTPServerThread.h"
#include "GeneralUtilities/ANSIColors.h"
#include "DiskStressThread.h"
#include "DiskInformation.h"
#include "FileInfoBlock.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static string
UserInputCommandPrompt = "Enter Command : ";

static pthread_t
UserInputServerID;

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
UserInputProcessCommandHelp
(StringList* InCommand);

void
UserInputProcessCommandDisk
(StringList* InCommand);

void
UserInputProcessCommandFile
(StringList* InCommand);

void
UserInputProcessCommandQuit
(StringList* InCommand);

void
UserInputProcessCommand
(StringList* InCommand);

void
UserInputProcessCommandMap
(StringList* InCommand);

void*
UserInputServerThread
(void* InParameter);

StringList*
UserInputParseCommandLine
(string InCommandLine);

/*****************************************************************************!
 * Function : UserInputServerThreadStart
 *****************************************************************************/
void
UserInputServerThreadStart
()
{
  if ( pthread_create(&UserInputServerID, NULL, UserInputServerThread, NULL) ) {
    fprintf(stderr, "Could not start User Input Server : %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : UserInputServerThreadInit
 *****************************************************************************/
void
UserInputServerThreadInit
()
{
}

/*****************************************************************************!
 * Function : UserInputGetThreadID
 *****************************************************************************/
pthread_t
UserInputGetThreadID
()
{
  return UserInputServerID;
}

/*****************************************************************************!
 * Function : UserInputServerThread
 *****************************************************************************/
void*
UserInputServerThread
(void* InParameter)
{
  StringList*                           command;
  string                                userInputString;
  while (true) {
    userInputString = linenoise(UserInputCommandPrompt);
    command = UserInputParseCommandLine(userInputString);
    if ( NULL == command ) {
      continue;
    }
    if ( command->stringCount == 0 ) {
      free(userInputString);
      StringListDestroy(command);
      continue;
    }
    UserInputProcessCommand(command);
    StringListDestroy(command);
    linenoiseHistoryAdd(userInputString);
    free(userInputString);
  }
  return NULL;
}

/*****************************************************************************!
 * Function : UserInputParseCommandLine
 *****************************************************************************/
StringList*
UserInputParseCommandLine
(string InCommandLine)
{
  string                                s;
  int                                   n;
  string                                start;
  string                                end;
  StringList*                           commands;
  enum
  {
   InStart = 1,
   InSpaces,
   InToken,
   InSingleQuotedString,
   InDoubleQuotedString,
   InDone
  } state = InStart;
  
  if ( NULL == InCommandLine ) {
    return NULL;
  }
  if ( StringContainsChar(InCommandLine, '"') ) {
    commands = StringListCreate();
    start = InCommandLine;
    end = start;
    while ( state != InDone ) {
      if ( *end == 0x00 ) {
        switch (state) {
          case InStart : {
            break;
          }
          case InDone : {
            break;
          }
          case InSpaces : {
            break;
          }
          case InToken : {
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            break;
          }
          case InDoubleQuotedString : {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            break;
          }
          case InSingleQuotedString : {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            break;
          }
        }
        state = InDone;
        continue;
      }
      switch (state) {
        case InDone : {
          break;
        }
          
          //!
        case InStart : {
          if ( *start == 0x00 ) {
            state = InDone;
            break;
          }
          if (isspace(*start) ) {
            start++;
            state = InSpaces;
            break;
          }
          if ( *start == '"' ) {
            end = start;
            end++;
            state = InDoubleQuotedString;
            break;
          }
          if  ( *start == '\'' ) {
            end = start;
            end++;
            state = InSingleQuotedString;
            break;
          }
          end = start;
          end++;
          state = InToken;
          break;
        }
          //!
        case InSpaces : {
          if ( *start == 0x00 ) {
            state = InDone;
            break;
          }
          
          if ( isspace(*start) ) {
            start++;
            break;
          }
          if ( *start == '"' ) {
            end = start;
            end++;
            state = InDoubleQuotedString;
            break;
          }
          if  ( *start == '\'' ) {
            end = start;
            end++;
            state = InSingleQuotedString;
            break;
          }
          end = start;
          end++;
          state = InToken;
          break;
        }

          //!
        case InToken : {
          if ( *end == 0x00 ) {
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            state = InDone;
            break;
          }
          
          if ( isspace(*end) ) {
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            start = end;
            state = InSpaces;
            break;
          }
          end++;
          break;
        }

          //!
        case InSingleQuotedString : {
          if (*end == '\\' ) {
            end++;
            if ( *end ) {
              end++;
              break;
            }
            state = InDone;
            break;
          }
          if ( *end == 0x00 ) {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            state = InDone;
            break;
          }
          if ( *end == '\'' ) {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            end++;
            start = end;
            if ( *end == 0x00 ) {
              state = InDone;
              break;
            }
            if ( isspace(*start) ) {
              state = InSpaces;
              end++;
              break;
            }
            if ( *start == '"' ) {
              state = InDoubleQuotedString;
              end++;
              break;
            }
            if ( *start == '\'' ) {
              state = InSingleQuotedString;
              end++;
              break;
            }
            end++;
            state = InToken;
            break;
          }
          end++;
          break;
        }
          
          //!
        case InDoubleQuotedString : {
          if (*end == '\\' ) {
            end++;
            if ( *end ) {
              end++;
              break;
            }
            state = InDone;
            break;
          }
          if ( *end == 0x00 ) {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            state = InDone;
            break;
          }            
          if ( *end == '"' ) {
            start++;
            n = end - start;
            s = StringNCopy(start, n);
            StringListAppend(commands, s);
            end++;
            start = end;
            if ( *end == 0x00 ) {
              state = InDone;
              break;
            }
            if ( isspace(*start) ) {
              state = InSpaces;
              end++;
              break;
            }
            if ( *start == '"' ) {
              state = InDoubleQuotedString;
              end++;
              break;
            }
            if ( *start == '\'' ) {
              state = InSingleQuotedString;
              end++;
              break;
            }
            end++;
            state = InToken;
            break;
          }
          end++;
          break;
        }
      }
    }
  } else {
    commands = StringSplit(InCommandLine, " \t", true);
  }
  return commands;
}

/*****************************************************************************!
 * Function : UserInputProcessCommand
 *****************************************************************************/
void
UserInputProcessCommand
(StringList* InCommand)
{
  string                                command;
  if ( NULL == InCommand ) {
    return;
  }
  if ( InCommand->stringCount == 0 ) {
    return;
  }

  command = InCommand->strings[0];
  if ( StringEqualNoCase(command, "quit") ) {
    UserInputProcessCommandQuit(InCommand);
    return;
  }
  if ( StringEqualNoCase(command, "file") ) {
    UserInputProcessCommandFile(InCommand);
    return;
  }
  if ( StringEqualNoCase(command, "disk") ) {
    UserInputProcessCommandDisk(InCommand);
    return;
  }

  if ( StringEqualNoCase(command, "map") ) {
	UserInputProcessCommandMap(InCommand);
	return;
  }

  if ( StringEqualNoCase(command, "help") ) {
    UserInputProcessCommandHelp(InCommand);
    return;
  }
}

/*****************************************************************************!
 * Function : UserInputProcessCommandQuit
 *****************************************************************************/
void
UserInputProcessCommandQuit
(StringList* InCommand)
{
  exit(EXIT_SUCCESS);
}

/*****************************************************************************!
 * Function : UserInputProcessCommandFile
 *****************************************************************************/
void
UserInputProcessCommandFile
(StringList* InCommand)
{
  string                                subCommand;
  int                                   n;
  if ( NULL == InCommand ) {
    return;
  }
  if ( InCommand->stringCount == 1 ) {
    n = DiskStressGetFileCount();
    printf("%sFiles Created : %s%s%d%s\n", ColorYellow, ColorReset, ColorGreen, n, ColorReset);
    return;
  }
  subCommand = InCommand->strings[1];
  printf("%sFILES                                                               %s\n", ColorBoldYellowReverse, ColorReset);
  printf("%s                               NAME        SIZE  DATE/TIME          %s\n", ColorBoldYellowReverse, ColorReset);
  if ( StringEqualNoCase(subCommand, "list") ) {
    DiskStressFileList();
    return;
  }
}

/*****************************************************************************!
 * Function : UserInputProcessCommandDisk
 *****************************************************************************/
void
UserInputProcessCommandDisk
(StringList* InCommand)
{
  DiskInformationDisplay();
}

/*****************************************************************************!
 * Function : UserInputProcessCommandHelp
 *****************************************************************************/
void
UserInputProcessCommandHelp
(StringList* InCommand)
{
  
}


/*****************************************************************************!
 * Function : UserInputProcessCommandMap
 *****************************************************************************/
void
UserInputProcessCommandMap
(StringList* InCommand)
{
  int                                   mapSize;
  uint64_t*                             map;
  int                                   i, n, j, k, m;
  bool                                  t;
  int									fileSetSize;

  FileInfoBlockSetGetMap(&mapSize, &map);
  fileSetSize = FileInfoBlockSetGetSize();

  printf("Map Size : %d\n", fileSetSize);
  printf("      0 : ");
  fflush(stdout);
  k = 0;
  m = 0;
  for ( i = 0 ; i < mapSize ; i++ ) {
	for ( j = 0 ; j < 64 ; j++ ) {
	  n = 1 << j;
	  t = map[i] & n ? true : false;
	  printf("%c", t ? '@' : '.');
	  fflush(stdout);
	  k++;
	  if ( k == fileSetSize ) {
		printf("\n");
		break;
	  }
	}
	if ( k == fileSetSize ) {
	  break;
	}
	m += 64;
	printf("\n");
	if ( i + 1 < mapSize ) {
	  printf("%7d : ", m);
	}
  }
}

