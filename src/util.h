/* []--------------------------------------------------------[]
    |                        util.h                          |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     W. van Engen                               |
    | VERSION:    1.00, 31 January 2012.                     |
    | USE:        General utility functions...               |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2012 W. van Engen, Nikhef / Stichting FOM
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__UTIL_INCLUDED__)
#define __UTIL_INCLUDED__

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include "logger.h"

using namespace std;

// -----------------------------------------------------------------------------

string ReadLineFromFile( string FileName );
string ReadStringFromFile( string FileName, int MaxSize = -1 );
void   WriteStringToFile( string String, string FileName );
string AbsolutePath( string FileName );
string BaseName( string& Path );
bool   FileExists( string Path );
bool   DirectoryExists( string Path );

// -----------------------------------------------------------------------------

#endif /* __UTIL_INCLUDED__ */
