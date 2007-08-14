/* []--------------------------------------------------------[]
    |                        csv.h                           |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 14 August 2007.                      |
    | USE:        Implements a csv parser...                 |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__CSV_INCLUDED__)
#define __CSV_INCLUDED__

#include <string>
#include <vector>
#include <cctype>

using namespace std;

// -----------------------------------------------------------------------------

vector<string> CommaSeparatedValues2Array( string Values, char Separator = ',' );
string         NormalizeCommaSeparatedValues( string Values, char Separator = ',' );

string         NormalizeString( string S );

// -----------------------------------------------------------------------------

#endif
