/*****************************************************************************
 * FILE NAME    : JSONOut.c
 * DATE         : December 02 2020
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "JSONOut.h"
#include "GeneralUtilities/MemoryManager.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/

/*****************************************************************************!
 * Local Data
 *****************************************************************************/

/*****************************************************************************!
 * Function : JSONOutCreate
 *****************************************************************************/
JSONOut*
JSONOutCreate
(string InTag, JSONOutType InType)
{
  JSONOut*                              jsonout;

  jsonout = (JSONOut*)GetMemory(sizeof(JSONOut));
  memset(jsonout, 0x00, sizeof(JSONOut));
  if ( InTag ) {
    jsonout->tag = StringCopy(InTag);
  }
  jsonout->type = InType;

  return jsonout;
}

/*****************************************************************************!
 * Function : JSONOutCreateInt
 *****************************************************************************/
JSONOut*
JSONOutCreateInt
(string InTag, uint32_t InInt)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeInt);
  jsonout->valueInt = InInt;
  return jsonout;
}

/*****************************************************************************!
 * Function : JSONOutCreateLongLong
 *****************************************************************************/
JSONOut*
JSONOutCreateLongLong
(string InTag, uint64_t InLongLong)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeLongLong);
  jsonout->valueLongLong = InLongLong;
  return jsonout;
}

/*****************************************************************************!
 * Function : JSONOutCreateString
 *****************************************************************************/
JSONOut*
JSONOutCreateString
(string InTag, string InString)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeString);
  if ( InString ) {
    jsonout->valueString = StringCopy(InString);
  } else {
    jsonout->valueString = StringCopy("");
  }
  return jsonout;
  
}

/*****************************************************************************!
 * Function : JSONOutCreateBool
 *****************************************************************************/
JSONOut*
JSONOutCreateBool
(string InTag, bool InBool)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeBool);
  jsonout->valueBool = InBool;
  return jsonout;
  
}

/*****************************************************************************!
 * Function : JSONOutCreateObject
 *****************************************************************************/
JSONOut*
JSONOutCreateObject
(string InTag)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeObject);

  jsonout->valueObject = JSONOutObjectCreate();
  return jsonout;
}

/*****************************************************************************!
 * Function : JSONOutCreateArray
 *****************************************************************************/
JSONOut*
JSONOutCreateArray
(string InTag)
{
  JSONOut*                              jsonout;

  jsonout = JSONOutCreate(InTag, JSONOutTypeArray);

  jsonout->valueArray = JSONOutArrayCreate();
  return jsonout;
}

/*****************************************************************************!
 * Function : JSONOutObjectAddObject
 *****************************************************************************/
void
JSONOutObjectAddObject
(JSONOut* InObject, JSONOut* InNewObject)
{
  if ( NULL == InObject || NULL == InNewObject ) {
    return;
  }
  if ( InObject->type != JSONOutTypeObject ) {
    return;
  }
  JSONOutObjectAppend(InObject->valueObject, InNewObject);
}

/*****************************************************************************!
 * Function : JSONOutObjectAddObjects
 *****************************************************************************/
void
JSONOutObjectAddObjects
(JSONOut* InObject, ...)
{
  JSONOut*                              object;
  va_list                               ap;
  
  if ( NULL == InObject ) {
    return;
  }
  if ( InObject->type != JSONOutTypeObject ) {
    return;
  }
  va_start(ap, InObject);
  object = va_arg(ap, JSONOut*);
  while (object) {
    JSONOutObjectAddObject(InObject, object);
    object = va_arg(ap, JSONOut*);
  }

  va_end(ap);
}

/*****************************************************************************!
 * Function : JSONOutArrayAddObject
 *****************************************************************************/
void
JSONOutArrayAddObject
(JSONOut* InArray, JSONOut* InNewObject)
{
  if ( NULL == InArray || NULL == InNewObject ) {
    return;
  }
  if ( InArray->type != JSONOutTypeArray ) {
    return;
  }
  JSONOutArrayAppend(InArray->valueArray, InNewObject);
}

/*****************************************************************************!
 * Function : JSONOutDestroy
 *****************************************************************************/
void
JSONOutDestroy
(JSONOut* InObject)
{
  if ( NULL == InObject ) {
    return;
  }

  if ( InObject->tag ) {
    FreeMemory(InObject->tag);
  }

  switch (InObject->type) {
    case JSONOutTypeNone : {
      break;
    }
      
    case JSONOutTypeInt : {
      break;
    }
      
    case JSONOutTypeLongLong : {
      break;
    }
      
    case JSONOutTypeFloat : {
      break;
    }
      
    case JSONOutTypeString : {
      FreeMemory(InObject->valueString);
      break;
    }
      
    case JSONOutTypeBool : {
      break;
    }
      
    case JSONOutTypeArray : {
      JSONOutArrayDestroy(InObject->valueArray);
      break;
    }
      
    case JSONOutTypeObject : {
      JSONOutObjectDestroy(InObject->valueObject);
      break;
    }      
  }
  FreeMemory(InObject);
}

/*****************************************************************************!
 * Function : JSONOutObjectCreate
 *****************************************************************************/
JSONOutObject*
JSONOutObjectCreate
()
{
  JSONOutObject*                        object;

  object = (JSONOutObject*)GetMemory(sizeof(JSONOutObject));
  memset(object, 0x00, sizeof(JSONOutObject));
  return object;
}

/*****************************************************************************!
 * Function : JSONOutObjectAppend
 *****************************************************************************/
void
JSONOutObjectAppend
(JSONOutObject* InObject, JSONOut* InNewObject)
{
  uint32_t                              i;
  JSONOut**                             objects;
  uint32_t                              n;
  if ( NULL == InObject || NULL == InNewObject ) {
    return;
  }

  n = InObject->count + 1;
  objects = (JSONOut**)GetMemory(sizeof(JSONOut*) * n);
  for ( i = 0; i < InObject->count ; i++ ) {
    objects[i] = InObject->objects[i];
  }
  if ( InObject->count > 0 ) {
    FreeMemory(InObject->objects);
  }
  objects[InObject->count] = InNewObject;
  
  InObject->objects = objects;
  InObject->count = n;
}

/*****************************************************************************!
 * Function : JSONOutObjectDestroy
 *****************************************************************************/
void
JSONOutObjectDestroy
(JSONOutObject* InObject)
{
  int                                   i;
  if ( NULL == InObject ) {
    return;
  }
  if ( InObject->count ) {
    for (i = 0; i < InObject->count; i++) {
      JSONOutDestroy(InObject->objects[i]);
    }
    FreeMemory(InObject->objects);
  }
  FreeMemory(InObject);
}

/*****************************************************************************!
 * Function : JSONOutToString
 *****************************************************************************/
string
JSONOutToString
(JSONOut* InObject, uint32_t InIndent)
{
  string                                s2;
  int                                   i;
  char                                  intString[32];
  char                                  floatString[16];
  string                                indentString;
  string                                s;
  if ( NULL == InObject ) {
    return NULL;
  }
  if ( InIndent > 0 ) {
    indentString = StringFill(' ', InIndent);
  } else {
    indentString = StringCopy("");
  }

  s = StringCopy(indentString);
  if ( InObject->tag ) {
    s = StringConcatTo(s, "\"");
    s = StringConcatTo(s, InObject->tag);
    s = StringConcatTo(s, "\" : ");
  }

  switch (InObject->type) {
    case JSONOutTypeNone : {
      break;
    }
      
    case JSONOutTypeInt : {
      sprintf(intString, "%d", InObject->valueInt);
      s = StringConcatTo(s, intString);
      break;
    }

    case JSONOutTypeLongLong : {
      sprintf(intString, "%lld", InObject->valueLongLong);
      s = StringConcatTo(s, intString);
      break;
    }
      
    case JSONOutTypeFloat : {
      sprintf(floatString, "%f", InObject->valueFloat);
      s = StringConcatTo(s, floatString);
      break;
    }
      
    case JSONOutTypeString : {
      s = StringConcatTo(s, "\"");
      s = StringConcatTo(s, InObject->valueString);
      s = StringConcatTo(s, "\"");
      break;
    }
      
    case JSONOutTypeBool : {
      s = StringConcatTo(s, InObject->valueBool ? "true" : "false");
      break;
    }
      
    case JSONOutTypeArray : {
      s = StringConcatTo(s, "[\n");
      for ( i = 0 ; i < InObject->valueArray->count ; i++ ) {
        s2 = JSONOutToString(InObject->valueArray->objects[i], InIndent + 2);
        // s = StringConcatTo(s, "{ ");
        s = StringConcatTo(s, s2);
        FreeMemory(s2);
        // s = StringConcatTo(s, " }");
        if ( i + 1 < InObject->valueArray->count ) {
          s = StringConcatTo(s, ",");
        }
        s = StringConcatTo(s,"\n");
      }
      s = StringConcatTo(s, indentString);
      s = StringConcatTo(s, "]");
      break;
    }
      
    case JSONOutTypeObject : {
      s = StringConcatTo(s, "{\n");
      for ( i = 0 ; i < InObject->valueObject->count ; i++ ) {
        s2 = JSONOutToString(InObject->valueObject->objects[i], InIndent + 2);
        s = StringConcatTo(s, s2);
        FreeMemory(s2);
        if ( i + 1 < InObject->valueObject->count ) {
          s = StringConcatTo(s, ",");
        }
        s = StringConcatTo(s,"\n");
      }
      s = StringConcatTo(s, indentString);
      s = StringConcatTo(s, "}");
      break;
    }
  }
  FreeMemory(indentString);
  return s;
}

/*****************************************************************************!
 * Function : JSONOutArrayCreate
 *****************************************************************************/
JSONOutArray*
JSONOutArrayCreate
()
{
  JSONOutArray*                        object;

  object = (JSONOutArray*)GetMemory(sizeof(JSONOutArray));
  memset(object, 0x00, sizeof(JSONOutArray));
  return object;
}

/*****************************************************************************!
 * Function : JSONOutArrayAppend
 *****************************************************************************/
void
JSONOutArrayAppend
(JSONOutArray* InArray, JSONOut* InNewArray)
{
  uint32_t                              i;
  JSONOut**                             objects;
  uint32_t                              n;
  if ( NULL == InArray || NULL == InNewArray ) {
    return;
  }

  n = InArray->count + 1;
  objects = (JSONOut**)GetMemory(sizeof(JSONOut*) * n);
  for ( i = 0; i < InArray->count ; i++ ) {
    objects[i] = InArray->objects[i];
  }
  if ( InArray->count > 0 ) {
    FreeMemory(InArray->objects);
  }
  objects[InArray->count] = InNewArray;
  
  InArray->objects = objects;
  InArray->count = n;
}

/*****************************************************************************!
 * Function : JSONOutArrayDestroy
 *****************************************************************************/
void
JSONOutArrayDestroy
(JSONOutArray* InArray)
{
  int                                   i;
  if ( NULL == InArray ) {
    return;
  }
  if ( InArray->count ) {
    for (i = 0; i < InArray->count; i++) {
      JSONOutDestroy(InArray->objects[i]);
    }
    FreeMemory(InArray->objects);
  }
  FreeMemory(InArray);
}

/*****************************************************************************!
 * Function : JSONOutSetName
 *****************************************************************************/
void
JSONOutSetName
(JSONOut* InJSON, string InName)
{
  if ( NULL == InJSON || NULL == InName ) {
    return;
  }

  if ( InJSON->tag ) {
    FreeMemory(InJSON->tag);
  }

  InJSON->tag = StringCopy(InName);
}
