/*****************************************************************************
 * FILE NAME    : JSONIF.h
 * DATE         : March 30 2020
 * PROJECT      : Bay Simulator
 * COPYRIGHT    : Copyright (C) 2020 by Vertiv Company
 *****************************************************************************/
#ifndef _jsonif_h_
#define _jsonif_h_

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

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Data
 *****************************************************************************/

/*****************************************************************************!
 * Exported Functions
 *****************************************************************************/
StringList*
JSONIFGetStringList
(json_value* InValue, string InName);

int
JSONIFGetInt
(json_value* InValue, string InName);

bool
JSONIFGetBool
(json_value* InValue, string InName);

string
JSONIFGetString
(json_value* InValue, string InName);

json_value*
JSONIFGetObject
(json_value* InValue, string InName);

json_value*
JSONIFGetValue
(json_value* InValue, string InName);

json_value*
JSONIFGetArray
(json_value* InValue, string InName);

float
JSONIFGetFloat
(json_value* InValue, string InName);

#endif /* _jsonif_h_*/

