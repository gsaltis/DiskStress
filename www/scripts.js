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

var
GetDiskInfoID;

var
GetDiskBlockID;

var
GetFileInfoID;

var
GetServerInfoID = 0;

var
GetRuntimeInfoID = 0;

var
FileBlockSize = 12;

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
    if ( InPacket.type == "diskinfo" ) {
      WebSocketIFHandleDiskInfoPacket(InPacket.body.diskinfo);
      return;
    }
    if ( InPacket.type == "fileinfo" ) {
      WebSocketIFHandleFileInfoPacket(InPacket.body.fileinfo);
      return;
    }
		 
    if ( InPacket.type == "blockinfo" ) {
      WebSocketIFHandleBlockInfoPacket(InPacket.body.blockinfo);
      return;
    }
    if ( InPacket.type == "runtimeinfo" ) {
      WebSocketIFHandleRuntimeInfoPacket(InPacket.body.runtimeinfo);
      return;
    }
    if ( InPacket.type == "serverinfo" ) {
      WebSocketIFHandleServerInfoPacket(InPacket.body.serverinfo);
      return;
    }
  }
}

/*****************************************************************************!
 * Function : WebSocketIFHandleServerInfoPacket
 *****************************************************************************/
function 
WebSocketIFHandleServerInfoPacket
(InPacket)
{
  var s;
  var elements = [
    { "name" : "ServerStartTime", "field" : "starttime" },
    { "name" : "ServerCurrentTime", "field" : "currenttime" }
  ];
  
  for (i = 0; i < elements.length; i++) {
    document.getElementById(elements[i].name).innerHTML = InPacket[elements[i].field];
  }

  s = "";
  if ( InPacket.updays > 0 ) {
    s = s + InPacket.updays + " Days ";
  }
  if ( InPacket.uphours > 0 || s != "" ) {
    s = s + InPacket.uphours + " Hours ";
  }
  if ( InPacket.upminutes > 0 || s != "" ) {
    s = s + InPacket.upminutes + " Minutes ";
  }
  if ( InPacket.uphours > 0 || s != "" ) {
    s = s + InPacket.upseconds + " Seconds ";
  }

  document.getElementById("ServerElapsedTime").innerHTML = s;
  clearTimeout(GetServerInfoID);
  GetServerInfoID = setTimeout(CBWebSocketIFGetServerInfo, 10000);
}

/*****************************************************************************!
 * Function : WebSocketIFHandleResponseInit
 *****************************************************************************/
function
WebSocketIFHandleResponseInit
(InPacket)
{
  WebSocketIFHandleDiskInfoPacket(InPacket.diskinfo);
  WebSocketIFHandleFileInfoPacket(InPacket.fileinfo);
  WebSocketIFHandleFileSizeInfoPacket(InPacket.filesizeinfo);
  WebSocketIFHandleServerInfoPacket(InPacket.serverinfo);
  clearTimeout(GetRuntimeInfoID);
  GetRuntimeInfoID = setTimeout(CBWebSocketIFGetRuntimeInfo, 10000);
  CreateBlockGrid(InPacket.filesizeinfo.maxfilesint);
}

/*****************************************************************************!
 * Function : WebSocketIFHandleFileSizeInfoPacket
 *****************************************************************************/
function
WebSocketIFHandleFileSizeInfoPacket
(InInfoPacket)
{
  var elements = [
    { "name" : "FileMaxFiles", "field" : "maxfiles" },
    { "name" : "FileMaxFileSize", "field" : "maxfilesize" }
  ];
  
  for (i = 0; i < elements.length; i++) {
    document.getElementById(elements[i].name).innerHTML = InInfoPacket[elements[i].field];
  }

  clearTimeout(GetFileInfoID);
  GetFileInfoID = setTimeout(CBWebSocketIFGetFileInfo, 10000);
}

/*****************************************************************************!
 * Function : WebSocketIFHandleRuntimeInfoPacket
 *****************************************************************************/
function
WebSocketIFHandleRuntimeInfoPacket
(InInfoPacket)
{
  clearTimeout(GetRuntimeInfoID);
  GetRuntimeInfoID = setTimeout(CBWebSocketIFGetRuntimeInfo, 10000);
}

/*****************************************************************************!
 * Function : WebSocketIFHandleFileInfoPacket
 *****************************************************************************/
function
WebSocketIFHandleFileInfoPacket
(InInfoPacket)
{
  var elements = [
    { "name" : "FileInfoTotalFiles", "field" : "count" },
    { "name" : "FileInfoTotalSize", "field" : "size" },
	{ "name" : "FileInfoCreateFiles", "field" : "created" },
	{ "name" : "FileInfoRemovedFiles", "field" : "destroyed" }
  ];
  
  for (i = 0; i < elements.length; i++) {
    document.getElementById(elements[i].name).innerHTML = InInfoPacket[elements[i].field];
  }

  clearTimeout(GetFileInfoID);
  GetFileInfoID = setTimeout(CBWebSocketIFGetFileInfo, 10000);
}

/*****************************************************************************!
 * Function : WebSocketIFHandleDiskInfoPacket
 *****************************************************************************/
function
WebSocketIFHandleDiskInfoPacket
(InInfoPacket)
{
  var                                   w, i, s;
  var                                   avail, used, total, freeWidth, usedWidth;
  var                                   availPercent, usedPercent, breakpoint;
  
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
  w = document.getElementById("DiskInfoTextArea").clientWidth - 20;

  //
  total = InInfoPacket.totalblocks;
  avail  = InInfoPacket.freeblocks;
  used  = InInfoPacket.totalblocks - InInfoPacket.freeblocks;

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

  total = InInfoPacket.totalbytes;
  avail  = InInfoPacket.freebytes;
  used  = InInfoPacket.totalbytes - InInfoPacket.freebytes;

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

  total = InInfoPacket.totalinodes;
  avail  = InInfoPacket.freeinodes;
  used  = InInfoPacket.totalinodes - InInfoPacket.freeinodes;

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
  
  for (i = 0; i < elements.length; i++) {
    document.getElementById(elements[i].name).innerHTML = InInfoPacket[elements[i].field];
  }
  GetDiskInfoID = setTimeout(CBWebSocketIFGetDiskInfo, 10000);
}

/*****************************************************************************!
 * Function : CBWebSocketIFGetServerInfo
 *****************************************************************************/
function
CBWebSocketIFGetServerInfo
()
{
  WebSocketIFSendSimpleRequest("getserverinfo");
}

/*****************************************************************************!
 * Function : CBWebSocketIFGetDiskInfo
 *****************************************************************************/
function
CBWebSocketIFGetDiskInfo
()
{
  WebSocketIFSendSimpleRequest("getdiskinfo");
}

/*****************************************************************************!
 * Function : CBWebSocketIFGetRuntimeInfo
 *****************************************************************************/
function
CBWebSocketIFGetRuntimeInfo
()
{
  WebSocketIFSendSimpleRequest("getruntimeinfo");
}

/*****************************************************************************!
 * Function : CBWebSocketIFGetFileInfo
 *****************************************************************************/
function
CBWebSocketIFGetFileInfo
()
{
  WebSocketIFSendSimpleRequest("getfileinfo");
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
  SetMessageTimeoutID = setTimeout(SetMessageClear, 10000);
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

/*****************************************************************************!
 * Function : CreateBlockGrid
 *****************************************************************************/
function
CreateBlockGrid
(InInfoPacket)
{
  var                                   section;
  var                                   width;
  var                                   height; 
  var                                   d, i, k, top, left, x, y;
  var                                   p, pwidth, pheight;

  section = document.getElementById("FileMapSectionBlocks");
  p = section.parentElement;
  pwidth = p.clientWidth; 
  pheight = p.clientHeight;
  x = section.offsetLeft;
  y = section.offsetTop;

  sectionWidth = pwidth - (x * 2 + 20);
	section.style.width = sectionWidth;
	console.log(x, y, pwidth, pheight);
 
	k = Math.floor(sectionWidth / FileBlockSize);

  while (section.lastChild ) {
	  section.removeChild(section.lastChild);
  }

  width = section.clientWidth;
  height = section.clientHeight;
  top = 2;
	i = 0;
  left = 2;
  for (j = 0; j < InInfoPacket ; j++  ) {
    if ( i == k ) {
      left = 2;
      top += FileBlockSize;
      i = 0;
    }
    d = document.createElement("div");
    d.style.left = left + "px";
    d.style.top =  top + "px";
	  d.className = "FileBlockEmpty";
	  d.id = "Block" + j;
    section.appendChild(d);
	  left += FileBlockSize;
		i++;
  }
}

/*****************************************************************************!
 * Function : WebSocketIFHandleBlockInfoPacket
 *****************************************************************************/
function
WebSocketIFHandleBlockInfoPacket
(InInfoPacket)
{
  var                                   blocks, block, j;

  blocks = InInfoPacket.filemapinfo.map.split('');
  for ( j = 0 ; j < InInfoPacket.filemapinfo.mapsize ; j++ ) {
	block = document.getElementById("Block" + j);
	if ( block ) {
	  if ( blocks[j] == "0") {
	    block.className = "FileBlock FileBlockUnUsed";
	  } else {
        block.className = "FileBlock FileBlockUsed";
  	  }
    }
  }
  GetBlockInfoID = setTimeout(CBWebSocketIFGetBlockInfo, 3000);
}

/*****************************************************************************!
 *  Function : CBWebSocketIFGetBlockInfo 
 *****************************************************************************/
function
CBWebSocketIFGetBlockInfo
()
{
  WebSocketIFSendSimpleRequest("getblockinfo");
}

/*****************************************************************************!
 * Function : CBFileMapSectionButtonPushed
 *****************************************************************************/
function
CBFileMapSectionButtonPushed
(InEvent)
{
  WebSocketIFSendSimpleRequest("getblockinfo");
}

/*****************************************************************************!
 * Function : CBChangeColorTest
 *****************************************************************************/
function
CBChangeTestColor
()
{
  var s = document.getElementById("TestColorSection");
  if ( s.className == "TestColorSectionActive" ) {
    s.className = "";
  } else {
    s.className = "TestColorSectionActive";
  }
}

