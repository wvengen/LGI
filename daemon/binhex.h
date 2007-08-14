/* []--------------------------------------------------------[]
    |                       binhex.h                         |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 2 August 2007.                       |
    | USE:        Implements binhex and hebin...             |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__BINHEX_INCLUDED__)
#define __BINHEX_INCLUDED__

#include <string>
#include <cctype>

using namespace std;

// -----------------------------------------------------------------------------

void BinHex( string& Bin, string &Hex );
void HexBin( string& Hex, string &Bin );

// -----------------------------------------------------------------------------

#endif
