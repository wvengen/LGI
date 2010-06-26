/* []--------------------------------------------------------[]
    |                        hash.h                          |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 14 August 2007.                      |
    | USE:        Implements a digest for strings...         |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__HASH_INCLUDED__)
#define __HASH_INCLUDED__

#include <string>

using namespace std;

// -----------------------------------------------------------------------------

#define HASH_BLOCK_SIZE 1024

// -----------------------------------------------------------------------------

string Hash( string S, string StartHash = "" );

// -----------------------------------------------------------------------------

#endif
