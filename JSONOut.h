/*****************************************************************************
 * FILE NAME    : JSONOut.h
 * DATE         : December 02 2020
 * COPYRIGHT    : Copyright (C) 2020 by Gregory R Saltis
 *****************************************************************************/
#ifndef _jsonout_h_
#define _jsonout_h_

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
#include "GeneralUtilities/String.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Type : JSONOutType
 *****************************************************************************/
typedef enum JSONOutType
{
 JSONOutTypeNone                = 0,
 JSONOutTypeInt,
 JSONOutTypeLongLong,
 JSONOutTypeFloat,
 JSONOutTypeString,
 JSONOutTypeBool,
 JSONOutTypeArray,
 JSONOutTypeObject
} JSONOutType;

/*****************************************************************************!
 * Exported Type : JSONOut
 *****************************************************************************/
struct _JSONOut
{
  JSONOutType                           type;
  string                                tag;
  struct {
    string                              valueString;
    bool                                valueBool;
    uint32_t                            valueInt;
    uint64_t                            valueLongLong;
    double                              valueFloat;
    struct _JSONOutArray*               valueArray;
    struct _JSONOutObject*              valueObject;
  };
};
typedef struct _JSONOut JSONOut;

/*****************************************************************************!
 * Exported Type : JSONOutArray
 *****************************************************************************/
struct _JSONOutArray
{
  JSONOut**                             objects;
  uint32_t                              count;
};
typedef struct _JSONOutArray JSONOutArray;

/*****************************************************************************!
 * Exported Type : JSONOutObject
 *****************************************************************************/
struct _JSONOutObject
{
  JSONOut**                             objects;
  uint32_t                              count;
};
typedef struct _JSONOutObject JSONOutObject;

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
void
JSONOutSetName
(JSONOut* InJSON, string InName);

string
JSONOutToString
(JSONOut* InObject, uint32_t InIndent);

//!
void
JSONOutObjectDestroy
(JSONOutObject* InObject);

void
JSONOutObjectAppend
(JSONOutObject* InObject, JSONOut* InNewObject);

JSONOutObject*
JSONOutObjectCreate
();

//!
void
JSONOutArrayDestroy
(JSONOutArray* InArray);

void
JSONOutArrayAppend
(JSONOutArray* InArray, JSONOut* InNewArray);

JSONOutArray*
JSONOutArrayCreate
();

//!
JSONOut*
JSONOutCreate
(string InTag, JSONOutType InType);

void
JSONOutDestroy
(JSONOut* InObject);

void
JSONOutObjectAddObject
(JSONOut* InObject, JSONOut* InNewObject);

void
JSONOutObjectAddObjects
(JSONOut* InObject, ...);

void
JSONOutArrayAddObject
(JSONOut* InObject, JSONOut* InNewObject);

JSONOut*
JSONOutCreateObject
(string InTag);

JSONOut*
JSONOutCreateArray
(string InTag);

JSONOut*
JSONOutCreateBool
(string InTag, bool InBool);

JSONOut*
JSONOutCreateString
(string InTag, string InString);

JSONOut*
JSONOutCreateInt
(string InTag, uint32_t InInt);

JSONOut*
JSONOutCreateLongLong
(string InTag, uint64_t InLongLong);

#endif /* _jsonout_h_*/
