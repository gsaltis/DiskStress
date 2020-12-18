/*****************************************************************************
 * FILE NAME    : scripts.js
 * DATE         : December 02 2020
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Local Variables
 *****************************************************************************/
var
SetMessageTimeoutID = 0;

var
WebSocketIFConnection = null;

var
WebSocketIFID = 0;

var
WebSocketIFDiskInfoPoll = true;

var
WebSocketIFDiskInfoPollTimeoutID = 0;

/*****************************************************************************!
 * Function : CBSystemInitialize
 *****************************************************************************/
function
CBSystemInitialize
()
{
  HideBlocker();
  WebSocketIFInitialize();
  SetMessageError("Greetings Earthlings");
}

/*****************************************************************************!
 * Function : WebSocketIFInitialize
 *****************************************************************************/
function
WebSocketIFInitialize
()
{
  var                                   hostaddress;

  hostaddress = "ws://" + WebSocketIFAddress + ":" + WebSocketIFPort;

  WebSocketIFConnection = new WebSocket(hostaddress);
  WebSocketIFConnection.onopen = function() {
    SetMessage("Connected to " + WebSocketIFAddress + ":" + WebSocketIFPort);
    WebSocketIFSendSimpleRequest("init");
    HideBlocker();
  }

  WebSocketIFConnection.onerror = function() {

  }

  WebSocketIFConnection.onclose = function() {
    ShowBlocker();
    SetMessage("Disconnected from " + WebSocketIFAddress + ":" + WebSocketIFPort);
  }

  WebSocketIFConnection.onmessage = function(InEvent) {
    WebSocketIFHandlePacket(InEvent.data);
  }
}

/*****************************************************************************!
 * Function : WebSocketIFHandlePacket
 *****************************************************************************/
function
WebSocketIFHandlePacket
(InData)
{
  var                                   packet;
  var                                   packettype;
  
  packet = JSON.parse(InData);
  n = packet.packetid;
  if ( n > 0 ) {
    WebSocketIFID = n;
  }

  packettype = packet.packettype;
  if ( packettype == "request" ) {
    WebSocketIFHandleRequest(packet);
  } else if ( packettype == "response" ) {
    WebSocketIFHandleResponse(packet);
  }
}

/*****************************************************************************!
 * Function : WebSocketIFHandleResponse
 *****************************************************************************/
function
WebSocketIFHandleResponse
(InPacket)
{
  if ( InPacket.status == "Error" ) {
    SetMessageError(InPacket.message);
    return;
  }

  console.log(InPacket);
  if ( InPacket.status == "OK" ) {
    WebSocketIFID = InPacket.packetid + 1;
    if ( InPacket.type == "init" ) {
      WebSocketIFHandleResponseInit(InPacket.body);
      return;
    }
  }
}

/*****************************************************************************!
 * Function : WebSocketIFHandleResponseInit
 *****************************************************************************/
function
WebSocketIFHandleResponseInit
(InPacket)
{

  var                                   w;
  var                                   avail, used, total, freeWidth, usedWidth;
  
  var elements = [
    { "name" : "DiskInfoTotalBlocks", "field" : "totalblocksstring" },
    { "name" : "DiskInfoTotalBytes", "field" : "totalbytesstring" },
    { "name" : "DiskInfoTotalInodes", "field" : "totalinodesstring" },
    { "name" : "DiskInfoAvailBlocks", "field" : "freeblocksstring" },
    { "name" : "DiskInfoAvailBytes", "field" : "freebytesstring" },
    { "name" : "DiskInfoAvailInodes", "field" : "freeinodesstring" },
    { "name" : "DiskInfoUsedBlocks", "field" : "usedblocksstring" },
    { "name" : "DiskInfoUsedBytes", "field" : "usedbytesstring" },
    { "name" : "DiskInfoUsedInodes", "field" : "usedinodesstring" }
  ];

  w1 = document.getElementById("DiskInfoTextArea").clientWidth;
  w = document.getElementById("DiskInfoTextArea").clientWidth - 20;


  //
  total = InPacket.diskinfo.totalblocks;
  avail  = InPacket.diskinfo.freeblocks;
  used  = InPacket.diskinfo.totalblocks - InPacket.diskinfo.freeblocks;

  availPercent = avail * 1000 / total;
  availPercent = availPercent.toFixed(0);
  usedPercent = 1000 - availPercent;
  breakpoint = w * availPercent / 1000;

  availPercent /= 10;
  s = document.getElementById("DiskInfoBlocksFree");
  s.innerHTML = availPercent + "%";
  s.style.left = (w - breakpoint + 20) + "px"

  usedPercent /= 10;
  s = document.getElementById("DiskInfoBlocksUsed");
  s.innerHTML = usedPercent + "%";
  s.style.right = breakpoint + "px";


  

  total = InPacket.diskinfo.totalbytes;
  avail  = InPacket.diskinfo.freebytes;
  used  = InPacket.diskinfo.totalbytes - InPacket.diskinfo.freebytes;

  availPercent = avail * 1000 / total;
  availPercent = availPercent.toFixed(0);
  usedPercent = 1000 - availPercent;
  breakpoint = w * availPercent / 1000;

  s = document.getElementById("DiskInfoBytesFree");
  availPercent /= 10;
  s.innerHTML = availPercent.toFixed(1) + "%";
  s.style.left = (w - breakpoint) + "px"

  usedPercent /= 10;
  s = document.getElementById("DiskInfoBytesUsed");
  s.innerHTML = usedPercent.toFixed(1) + "%";
  s.style.right = breakpoint + "px";



  total = InPacket.diskinfo.totalinodes;
  avail  = InPacket.diskinfo.freeinodes;
  used  = InPacket.diskinfo.totalinodes - InPacket.diskinfo.freeinodes;

  availPercent = avail * 1000 / total;
  availPercent = availPercent.toFixed(0);
  usedPercent = 1000 - availPercent;
  breakpoint = w * availPercent / 1000;

  availPercent /= 10;
  s = document.getElementById("DiskInfoInodesFree");
  s.innerHTML = availPercent.toFixed(1) + "%";
  s.style.left = (w - breakpoint + 20) + "px"
  
  usedPercent /= 10;
  s = document.getElementById("DiskInfoInodesUsed");
  s.innerHTML = usedPercent.toFixed(1) + "%";
  s.style.right = (breakpoint) + "px";

  
  console.log(w1, breakpoint, availPercent, usedPercent);
  for (i = 0; i < elements.length; i++) {
    document.getElementById(elements[i].name).innerHTML = InPacket.diskinfo[elements[i].field];
  }
}

/*****************************************************************************!
 * Function : HideBlocker
 *****************************************************************************/
function
HideBlocker
()
{
  var                                   blocker;

  blocker = document.getElementById("MainBlocker");
  blocker.style.visibility = "hidden";
}

/*****************************************************************************!
 * Function : ShowBlocker
 *****************************************************************************/
function
ShowBlocker
()
{
  var                                   blocker;

  blocker = document.getElementById("MainBlocker");
  blocker.style.visibility = "visible";
}

/*****************************************************************************!
 * Function : WebSocketIFGetNextID
 *****************************************************************************/
function
WebSocketIFGetNextID
()
{
  return ++WebSocketIFID;
}

/*****************************************************************************!
 * Function : WebSocketIFSendSimpleRequest
 *****************************************************************************/
function
WebSocketIFSendSimpleRequest
(InRequest)
{
  var                                   d;
  var                                   request;

  request = {};

  d = new Date();
  
  request.packettype = "request";
  request.packetid = WebSocketIFGetNextID();
  request.time = d.getTime();
  request.type = InRequest;
  request.body = "";

  WebSocketIFSendGeneralRequest(request);
}

/*****************************************************************************!
 * Function : WebSocketIFSendGeneralRequest
 *****************************************************************************/
function
WebSocketIFSendGeneralRequest
(InRequest)
{
  if ( WebSocketIFConnection ) {
    WebSocketIFConnection.send(JSON.stringify(InRequest));
  }
}

/*****************************************************************************!
 * Function : SetMessageNormal
 *****************************************************************************/
function
SetMessageNormal
(InMessage)
{
  SetMessage(InMessage, "#000080");
}

/*****************************************************************************!
 * Function : SetMessageError
 *****************************************************************************/
function
SetMessageError
(InMessage)
{
  SetMessage(InMessage, "#C00000");
}

/*****************************************************************************!
 * Function : SetMessage
 *****************************************************************************/
function
SetMessage
(InMessage, InColor)
{
  var                                   inputArea;

  inputArea = document.getElementById("MessageArea");
  inputArea.innerHTML = InMessage;
  inputArea.style.color = InColor;
  SetMessageTimeoutID = setInterval(SetMessageClear, 10000);
}

/*****************************************************************************!
 * Function : SetMessageClear
 *****************************************************************************/
function
SetMessageClear
()
{
  var                                   inputArea;

  inputArea = document.getElementById("MessageArea");
  inputArea.innerHTML = "";
  SetMessageTimeoutID = 0;
}

