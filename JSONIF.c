/*****************************************************************************
 * FILE NAME    : JSONIF.c
 * DATE         : March 30 2020
 * PROJECT      : Bay Simulator
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

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "RPiBaseModules/json.h"
#include "GeneralUtilities/String.h"
#include "GeneralUtilities/MemoryManager.h"
#include "JSONIF.h"

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
 * Function : JSONIFGetString
 *****************************************************************************/
string
JSONIFGetString
(json_value* InValue, string InName)
{
  string                                s;
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return NULL;
  }
  if ( InValue->type != json_object ) {
    return NULL;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_string ) {
      continue;
    }
    s = (string)StringNCopy(v->u.string.ptr, v->u.string.length);
    return s;
  }
  return NULL;
}

/*****************************************************************************!
 * Function : JSONIFGetBool
 *****************************************************************************/
bool
JSONIFGetBool
(json_value* InValue, string InName)
{
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return false;
  }
  if ( InValue->type != json_object ) {
    return false;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_boolean ) {
      continue;
    }
    return v->u.boolean ? true : false;
  }
  return false;
}

/*****************************************************************************!
 * Function : JSONIFGetFloat
 *****************************************************************************/
float
JSONIFGetFloat
(json_value* InValue, string InName)
{
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return 0;
  }
  if ( InValue->type != json_object ) {
    return 0;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_double ) {
      continue;
    }
    return (float)v->u.dbl;
  }
  return 0;
}

/*****************************************************************************!
 * Function : JSONIFGetInt
 *****************************************************************************/
int
JSONIFGetInt
(json_value* InValue, string InName)
{
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return 0;
  }
  if ( InValue->type != json_object ) {
    return 0;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_integer ) {
      continue;
    }
    return v->u.integer;
  }
  return 0;
}

/*****************************************************************************!
 * Function : JSONIFGetValue
 *****************************************************************************/
json_value*
JSONIFGetValue
(json_value* InValue, string InName)
{
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return NULL;
  }
  if ( InValue->type != json_object ) {
    return NULL;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    return entry->value;
  }
  return NULL;
}


/*****************************************************************************!
 * Function : JSONIFGetObject
 *****************************************************************************/
json_value*
JSONIFGetObject
(json_value* InValue, string InName)
{
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return NULL;
  }
  if ( InValue->type != json_object ) {
    return NULL;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_object ) {
      continue;
    }
    return v;
  }
  return NULL;
}

/*****************************************************************************!
 * Function : JSONIFGetArray
 *****************************************************************************/
json_value*
JSONIFGetArray
(json_value* InValue, string InName)
{
  json_value*                           v;
  int                                   i;
  if ( NULL == InValue || NULL == InName ) {
    return NULL;
  }
  if ( InValue->type != json_object ) {
    return NULL;
  }

  for ( i = 0 ; i < InValue->u.object.length ; i++ ) {
    json_object_entry*                  entry;

    entry = &(InValue->u.object.values[i]);
    if ( ! StringEqual(entry->name, InName) ) {
      continue;
    }
    v = entry->value;
    if ( v->type != json_array ) {
      continue;
    }
    return v;
  }
  return NULL;
}

/*****************************************************************************!
 * Function : JSONIFGetStringList
 *****************************************************************************/
StringList*
JSONIFGetStringList
(json_value* InValue, string InName)
{
  string                                s;
  json_value*                           v;
  int                                   i;
  json_value*                           value;
  StringList*                           strings;
  
  if ( NULL == InValue || NULL == InName ) {
    return NULL;
  }

  value = JSONIFGetArray(InValue, InName);
  if ( NULL == value ) {
    return NULL;
  }

  strings = StringListCreate();
  for ( i = 0; i < value->u.array.length; i++ ) {
    v = value->u.array.values[i];
    if ( v->type != json_string ) {
      continue;
    }
    s = StringNCopy(v->u.string.ptr, v->u.string.length);
    StringListAppend(strings, s);
  }
  return strings;
}
