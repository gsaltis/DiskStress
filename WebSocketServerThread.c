/*****************************************************************************
 * FILE NAME    : WebSocketServerThread.c
 * DATE         : December 14 2020
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
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "WebSocketServerThread.h"
#include "GeneralUtilities/ANSIColors.h"
#include "DiskStressThread.h"
#include "RPiBaseModules/mongoose.h"
#include "WebConnection.h"
#include "GeneralUtilities/String.h"
#include "JSONIF.h"
#include "JSONOut.h"
#include "RPiBaseModules/json.h"
#include "GeneralUtilities/MemoryManager.h"
#include "DiskInformation.h"
#include "Log.h"
#include "FileInfoBlock.h"
#include "GeneralUtilities/NumericTypes.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/
#define WEBSOCKET_SERVER_MAX_ADDRESS_TRIES      30
#define WEBSOCKET_SERVER_ADDRESS_WAIT_PERIOD    2

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
static pthread_t
WebSocketServerThreadID;

static uint32_t
WebSocketID;

static WebConnectionList*
WebSocketConnections;

static struct mg_serve_http_opts
WebSocketServerOptions;

static struct mg_connection*
WebSocketConnection;

static struct mg_mgr
WebSocketManager;

static string
WebSocketWWWDirectoryDefault = "www";

static string
WebSocketWWWDirectory = NULL;

static string
WebSocketPortAddressDefault = "8002";

static string
WebSocketPortAddress = NULL;

static int
WebSocketServerPollPeriod = 20;

time_t
WebSocketServerStartTime = 0;

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void*
WebSocketServerThread
(void* InParameters);

void
WebSocketServerEventHandler
(struct mg_connection* InConnection, int InEvent, void* InParameter);

void
WebSocketHandlePacket
(struct mg_connection* InConnection, string InData, int InDataSize);

void
WebSocketHandlePacket
(struct mg_connection* InConnection, string InData, int InDataSize);

void
WebSocketHandleRequest
(struct mg_connection* InConnection, json_value* InValue);

void
WebSocketFrameSend
(struct mg_connection* InConnection, string InBuffer, uint16_t InBufferLen);

void
WebSocketHandleInit
(struct mg_connection* InConnection, json_value* InJSONDoc);

void
WebSocketHandleGetDiskInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

void
WebSocketHandleGetFileInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

void
WebSocketHandleGetRuntimeInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

void
WebSocketHandleGetBlockInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

JSONOut*
WebSocketCreateFileInfoSection
();

void
WebSocketHandleGetServerInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

JSONOut*
WebSocketCreateServerInfoSection
();

void
WebSocketHandleGetStressInfo
(struct mg_connection* InConnection, json_value* InJSONDoc);

void
WebSocketServerSendResponse
(struct mg_connection * InConnection, JSONOut* InBody, json_value* InJSONDoc, string InResponseType);

/*****************************************************************************!
 * Function : WebSocketServerThreadInit
 *****************************************************************************/
void
WebSocketServerThreadInit
()
{
  WebSocketConnections = WebConnectionListCreate();
  WebSocketPortAddress = WebSocketPortAddressDefault;
  WebSocketWWWDirectory = StringCopy(WebSocketWWWDirectoryDefault);
  WebSocketID           = 0;
}

/*****************************************************************************!
 * Function : WebSocketServerThreadStart
 *****************************************************************************/
void
WebSocketServerThreadStart
()
{
  WebSocketServerCreateInfoScript();
  if ( pthread_create(&WebSocketServerThreadID, NULL, WebSocketServerThread, NULL) ) {
    LogAppend("Could not start WebSocket Server Thread");
    fprintf(stdout, "%sCould not start \"Web Socket Server Thread\"%s\n", ColorRed, ColorReset);
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : WebSocketServerThread
 *****************************************************************************/
void*
WebSocketServerThread
(void* InParameters)
{
  mg_mgr_init(&WebSocketManager, NULL);
  WebSocketConnection = mg_bind(&WebSocketManager, WebSocketPortAddress, WebSocketServerEventHandler);
  if ( NULL == WebSocketConnection ) {
    LogAppend("Failed to create WebSocket Server");
    fprintf(stdout, "%sFailed to create WebSocket server%s\n", ColorBrightRed, ColorReset);
    exit(EXIT_FAILURE);
  }
  mg_set_protocol_http_websocket(WebSocketConnection);
  WebSocketServerOptions.document_root = WebSocketWWWDirectory;
  WebSocketServerOptions.enable_directory_listing = "yes";
  
  printf("%sWeb Socket Server Thread : %sstarted%s\n"
         "%s  Port                   : %s%s%s\n"
         "%s  Directory              : %s%s%s\n", 
         ColorGreen, ColorYellow, ColorReset,
         ColorCyan, ColorYellow, WebSocketPortAddress, ColorReset, 
         ColorCyan, ColorYellow, WebSocketWWWDirectory, ColorReset);
  LogAppend("WebSocket Server Thread : started");
  LogAppend("  Port                  : %s", WebSocketPortAddress);
  LogAppend("  Directory             : %s", WebSocketWWWDirectory);
  DiskStressThreadStart();
  WebSocketServerStartTime = time(NULL);
  while ( true ) {
    mg_mgr_poll(&WebSocketManager, WebSocketServerPollPeriod);
  }
}

/*****************************************************************************!
 * Function : WebSocketServerGetThreadID
 *****************************************************************************/
pthread_t
WebSocketServerGetThreadID
()
{
  return WebSocketServerThreadID;
}

/*****************************************************************************!
 * Function : WebSocketServerEventHandler
 *****************************************************************************/
void
WebSocketServerEventHandler
(struct mg_connection* InConnection, int InEvent, void* InParameter)
{
  WebConnection*                        con;
  struct websocket_message*             message;
  
  if ( InEvent == 0 ) {
    return;
  }

  switch (InEvent) {
    case MG_EV_CLOSE : {
      con = WebConnectionListFind(WebSocketConnections, InConnection);
      if ( con ) {
        WebConnectionListRemove(WebSocketConnections, con);
      }
      break;
    }

    case MG_EV_WEBSOCKET_FRAME : {
      if ( NULL == WebConnectionListFind(WebSocketConnections, InConnection) ) {
        WebConnectionListAppend(WebSocketConnections, WebConnectionCreate(InConnection));
      }
      message = (struct websocket_message*)InParameter;
      WebSocketHandlePacket(InConnection, (string)message->data, message->size);
      break;
    }
  }
}

/*****************************************************************************!
 * Function : WebSocketServerCreateInfoScript
 *****************************************************************************/
void
WebSocketServerCreateInfoScript
()
{
  struct ifaddrs*                       addresses;
  struct ifaddrs*                       tempAddress;
  
  string                                address;
  int                                   fd;
  struct                                ifreq ifr;
  FILE*                                 file;
  struct sockaddr_in*                   sa;
  char                                  b;
  bool                                  found;
  string                                interface;

  found = false;

  getifaddrs(&addresses);
  
  printf("%sWebSocket Script         : ", ColorGreen);
  fflush(stdout);
  //! We only want to do this when we have a value 192. address
  //  So we loop until we do or we eventually give up
  for (int i = 0; i < WEBSOCKET_SERVER_MAX_ADDRESS_TRIES && !found; i++ ) { 
    for ( tempAddress = addresses ; tempAddress ; tempAddress = tempAddress->ifa_next ) {
      if ( ! (tempAddress->ifa_addr && tempAddress->ifa_addr->sa_family == AF_PACKET) ) {
        continue;
      }
      fd = socket(AF_INET, SOCK_DGRAM, 0);
      /* I want to get an IPv4 IP address */
      ifr.ifr_addr.sa_family = AF_INET;

      interface = tempAddress->ifa_name;    
      strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    
      ioctl(fd, SIOCGIFADDR, &ifr);
      close(fd);
      sa = (struct sockaddr_in*)&ifr.ifr_addr;
      b = (char)(sa->sin_addr.s_addr & 0xFF);
  
      //! Check that we have a at least a C Address
      if ( b & 0xC0 ) {
        address = StringCopy(inet_ntoa(sa->sin_addr));
        file = fopen("www/websocketinfo.js", "wb");
        if ( NULL == file ) {
          return;
        }
        fprintf(file, "var WebSocketIFAddress = \"%s\";\n", address);
        fprintf(file, "var WebSocketIFPort = \"%s\";\n", WebSocketPortAddress);
        fclose(file);
        FreeMemory(address);
        found = true;
      }
      if ( !found ) {
        //! We don't so sleep and try again
        sleep(WEBSOCKET_SERVER_ADDRESS_WAIT_PERIOD);
      }
    }
  }
  freeifaddrs(addresses);
  printf("%s%screated%s\n", ColorYellow, found ? "" : "not ", ColorReset);
}

/*****************************************************************************!
 * Function : WebSocketHandlePacket
 *****************************************************************************/
void
WebSocketHandlePacket
(struct mg_connection* InConnection, string InData, int InDataSize)
{
  string                                packetType;
  string                                s;
  json_value*                           jsonDoc;

  
  s = StringNCopy(InData, InDataSize);
  jsonDoc = json_parse((const json_char*)InData, (size_t)InDataSize);

  packetType = JSONIFGetString(jsonDoc, "packettype");

  if ( StringEqual(packetType, "request") ) {
    WebSocketHandleRequest(InConnection, jsonDoc);
  }
  json_value_free(jsonDoc);
  FreeMemory(packetType);
  FreeMemory(s);
}

/*****************************************************************************!
 * Function : WebSocketHandleRequest
 *****************************************************************************/
void
WebSocketHandleRequest
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  string                                type;

  type = JSONIFGetString(InJSONDoc, "type");
  WebSocketID = JSONIFGetInt(InJSONDoc, "packetid");
  if ( StringEqual(type, "init") ) {
    WebSocketHandleInit(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getdiskinfo") ) {
    WebSocketHandleGetDiskInfo(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getfileinfo") ) {
    WebSocketHandleGetFileInfo(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getruntimeinfo") ) {
    WebSocketHandleGetRuntimeInfo(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getblockinfo") ) {
    WebSocketHandleGetBlockInfo(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getserverinfo") ) {
    WebSocketHandleGetServerInfo(InConnection, InJSONDoc);
  } else if ( StringEqual(type, "getstressinfo") ) {
    WebSocketHandleGetStressInfo(InConnection, InJSONDoc);
  }
  FreeMemory(type);
}

/*****************************************************************************!
 * Function : WebSocketHandleGetStressInfo
 *****************************************************************************/
void
WebSocketHandleGetStressInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              body;
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, DiskStressThreadStressInfoToJSON());
  WebSocketServerSendResponse(InConnection, body, InJSONDoc, "stressinfo");
}

/*****************************************************************************!
 * Function : WebSocketHandleGetServerInfo
 *****************************************************************************/
void
WebSocketHandleGetServerInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              body;
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, WebSocketCreateServerInfoSection());
  WebSocketServerSendResponse(InConnection, body, InJSONDoc, "serverinfo");
}

/*****************************************************************************!
 * Function : WebSocketCreateServerInfoSection
 *****************************************************************************/
JSONOut*
WebSocketCreateServerInfoSection
()
{
  JSONOut*                              jsonout;
  time_t                                currentTime;
  time_t                                elapsedSeconds;
  char                                  startTimeString[64];
  char                                  currentTimeString[64];
  int                                   n, days, hours, minutes, seconds;
  struct tm*                            t;

  currentTime = time(NULL);
  elapsedSeconds = currentTime - WebSocketServerStartTime;

  t = localtime(&WebSocketServerStartTime);
  sprintf(startTimeString, "%02d/%02d/%04d %02d:%02d:%02d", t->tm_mon + 1,
          t->tm_mday, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);

  t = localtime(&currentTime);
  sprintf(currentTimeString, "%02d/%02d/%04d %02d:%02d:%02d", t->tm_mon + 1,
          t->tm_mday, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);

  days = elapsedSeconds / 86400;
  n = elapsedSeconds % 86400;
  hours = n / 3600;
  n = n % 3600;
  minutes = n / 60; 
  seconds = n % 60;

  jsonout = JSONOutCreateObject("serverinfo");
  
  JSONOutObjectAddObjects(jsonout,
                          JSONOutCreateString("starttime", startTimeString),
                          JSONOutCreateString("currenttime", currentTimeString),
                          JSONOutCreateInt("updays", days),
                          JSONOutCreateInt("uphours", hours),
                          JSONOutCreateInt("upminutes", minutes),
                          JSONOutCreateInt("upseconds", seconds),
                          JSONOutCreateInt("elapsedtime", elapsedSeconds),
                          NULL);
  return jsonout;

}

/*****************************************************************************!
 * Function : WebSocketHandleGetRuntimeInfo
 *****************************************************************************/
void
WebSocketHandleGetBlockInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              body;
  JSONOut*                              blockInfo;

  blockInfo = JSONOutCreateObject("blockinfo");
  JSONOutObjectAddObjects(blockInfo,
                          FileInfoBlockSetToJSON(),
                          NULL);
  
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, blockInfo);
  WebSocketServerSendResponse(InConnection, body, InJSONDoc, "blockinfo");
}

/*****************************************************************************!
 * Function : WebSocketServerSendResponse
 *****************************************************************************/
void
WebSocketServerSendResponse
(struct mg_connection * InConnection, JSONOut* InBody, json_value* InJSONDoc, string InResponseType)
{
  string                                s;
  JSONOut*                              object;

  object = JSONOutCreateObject(NULL);
  JSONOutObjectAddObjects(object,
                          JSONOutCreateString("packettype", "response"),
                          JSONOutCreateInt("packetid", JSONIFGetInt(InJSONDoc, "packetid")),
                          JSONOutCreateInt("time", (int)time(NULL)),
                          JSONOutCreateString("type", InResponseType),
                          JSONOutCreateString("status", "OK"),
                          InBody,
                          NULL);
  
  s = JSONOutToString(object, 0);
  WebSocketFrameSend(InConnection, s, strlen(s));
  FreeMemory(s);
  JSONOutDestroy(object);
}

/*****************************************************************************!
 * Function : WebSocketHandleGetRuntimeInfo
 *****************************************************************************/
void
WebSocketHandleGetRuntimeInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              body;
  string                                s;
  JSONOut*                              object;
  JSONOut*                              fileInfo;


  fileInfo = JSONOutCreateObject("runtimeinfo");
  JSONOutObjectAddObjects(fileInfo,
                          JSONOutCreateInt("starttime", DiskStressThreadGetStartTime()),
                          JSONOutCreateInt("currenttime", (int)time(NULL)),
                          NULL);
  
  object = JSONOutCreateObject(NULL);
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, fileInfo);
  JSONOutObjectAddObjects(object,
                          JSONOutCreateString("packettype", "response"),
                          JSONOutCreateInt("packetid", JSONIFGetInt(InJSONDoc, "packetid")),
                          JSONOutCreateInt("time", (int)time(NULL)),
                          JSONOutCreateString("type", "runtimeinfo"),
                          JSONOutCreateString("status", "OK"),
                          body,
                          NULL);
  
  s = JSONOutToString(object, 0);
  WebSocketFrameSend(InConnection, s, strlen(s));
  FreeMemory(s);
  JSONOutDestroy(object);
}

/*****************************************************************************!
 * Function : WebSocketHandleGetFileInfo
 *****************************************************************************/
void
WebSocketHandleGetFileInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              body;
  string                                s;
  JSONOut*                              object;
  JSONOut*                              fileInfo;

  fileInfo = WebSocketCreateFileInfoSection();  
 
  object = JSONOutCreateObject(NULL);
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, fileInfo);
  JSONOutObjectAddObjects(object,
                          JSONOutCreateString("packettype", "response"),
                          JSONOutCreateInt("packetid", JSONIFGetInt(InJSONDoc, "packetid")),
                          JSONOutCreateInt("time", (int)time(NULL)),
                          JSONOutCreateString("type", "fileinfo"),
                          JSONOutCreateString("status", "OK"),
                          body,
                          NULL);
  
  s = JSONOutToString(object, 0);
  WebSocketFrameSend(InConnection, s, strlen(s));
  FreeMemory(s);
  JSONOutDestroy(object);

}

/*****************************************************************************!
 * Function : WebSocketCreateFileInfoSection 
 *****************************************************************************/
JSONOut*
WebSocketCreateFileInfoSection
()
{
  JSONOut*                              fileInfo;
  char                                  s1[32], s2[32], s3[32], s4[32];

  fileInfo = JSONOutCreateObject("fileinfo");
  JSONOutObjectAddObjects(fileInfo,
                          JSONOutCreateString("size",      ConvertLongLongToCommaString(DiskStressGetFileSize(), s1)),
                          JSONOutCreateString("count",     ConvertIntToCommaString(DiskStressGetFileCount(), s2)),
                          JSONOutCreateString("created",   ConvertIntToCommaString(DiskStressThreadGetFilesCreatedCount(), s3)),
                          JSONOutCreateString("destroyed", ConvertIntToCommaString(DiskStressThreadGetFilesRemovedCount(), s4)),
                          NULL);
  return fileInfo;
}

/*****************************************************************************!
 * Function : WebSocketHandleInit
 *****************************************************************************/
void
WebSocketHandleInit
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              diskInfo;
  JSONOut*                              body;
  string                                s;
  JSONOut*                              object;
  JSONOut*                              fileInfo;
  JSONOut*                              sizeInfo;
  JSONOut*                              serverInfo;
  JSONOut*                              stressInfo;
  char                                  s1[32], s2[32];

  sizeInfo = JSONOutCreateObject("filesizeinfo");
  JSONOutObjectAddObjects(sizeInfo,
                          JSONOutCreateString("maxfiles",    ConvertLongLongToCommaString(DiskStressThreadGetMaxFiles(),s1)),
                          JSONOutCreateLongLong("maxfilesint", DiskStressThreadGetMaxFiles()),
                          JSONOutCreateString("maxfilesize", ConvertLongLongToCommaString(DiskStressThreadGetMaxFileSize(), s2)),
                          NULL);

  object = JSONOutCreateObject(NULL);
  fileInfo = WebSocketCreateFileInfoSection();
  serverInfo = WebSocketCreateServerInfoSection();
  diskInfo = DiskInformationToJSON();
  stressInfo = DiskStressThreadStressInfoToJSON();
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObjects(body, serverInfo, diskInfo, fileInfo, sizeInfo, stressInfo, NULL);
  JSONOutObjectAddObjects(object,
                          JSONOutCreateString("packettype", "response"),
                          JSONOutCreateInt("packetid", JSONIFGetInt(InJSONDoc, "packetid")),
                          JSONOutCreateInt("time", (int)time(NULL)),
                          JSONOutCreateString("type", "init"),
                          JSONOutCreateString("status", "OK"),
                          body,
                          NULL);
  
  s = JSONOutToString(object, 0);
  WebSocketFrameSend(InConnection, s, strlen(s));
  FreeMemory(s);
  JSONOutDestroy(object);
}

/*****************************************************************************!
 * Function : WebSocketHandleGetDiskInfo
 *****************************************************************************/
void
WebSocketHandleGetDiskInfo
(struct mg_connection* InConnection, json_value* InJSONDoc)
{
  JSONOut*                              diskInfo;
  JSONOut*                              body;
  string                                s;
  JSONOut*                              object;

  object = JSONOutCreateObject(NULL);
  diskInfo = DiskInformationToJSON();
  body = JSONOutCreateObject("body");
  JSONOutObjectAddObject(body, diskInfo);
  JSONOutObjectAddObjects(object,
                          JSONOutCreateString("packettype", "response"),
                          JSONOutCreateInt("packetid", JSONIFGetInt(InJSONDoc, "packetid")),
                          JSONOutCreateInt("time", (int)time(NULL)),
                          JSONOutCreateString("type", "diskinfo"),
                          JSONOutCreateString("status", "OK"),
                          body,
                          NULL);
  
  s = JSONOutToString(object, 0);
  WebSocketFrameSend(InConnection, s, strlen(s));
  FreeMemory(s);
  JSONOutDestroy(object);
}

/*****************************************************************************!
 * Function : WebSocketFrameSend
 *****************************************************************************/
void
WebSocketFrameSend
(struct mg_connection* InConnection, string InBuffer, uint16_t InBufferLen)
{
  mg_send_websocket_frame(InConnection, WEBSOCKET_OP_TEXT,
                          InBuffer, InBufferLen);
}

/*****************************************************************************!
 * Function : WebSocketJSONSendAll
 *****************************************************************************/
void
WebSocketJSONSendAll
(JSONOut* InJSON)
{
  uint16_t                              sl;
  WebConnection*                        connection;
  string                                s;
  if ( NULL == InJSON ) {
    return;
  }
  
  if ( WebSocketConnections == NULL || WebSocketConnections->first == NULL ) {
    return;
  }
  s = JSONOutToString(InJSON, 0);
  if ( NULL == s ) {
    return;
  }
  sl = (uint16_t)strlen(s);
  for ( connection = WebSocketConnections->first; connection; connection = connection->next ) {
    WebSocketFrameSend(connection->connection, s, sl);
  }
  FreeMemory(s);
}

/*****************************************************************************!
 * Function : WebSocketServerSetDirectory
 *****************************************************************************/
void
WebSocketServerSetDirectory
(string InDirectoryName)
{
  if ( InDirectoryName == NULL ) {
    return;
  }

  if ( WebSocketWWWDirectory ) {
    FreeMemory(WebSocketWWWDirectory);
  }

  WebSocketWWWDirectory = StringCopy(InDirectoryName);
}
