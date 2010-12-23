/* []--------------------------------------------------------[]
    |                        xml.h                           |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 10 July 2007.                        |
    | USE:        Implements a basic xml parser...           |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__XML_INCLUDED__)
#define __XML_INCLUDED__

#include <string>
#include <vector>
#include <cctype>

using namespace std;

// -----------------------------------------------------------------------------

string Parse_XML( string XML, string Tag, string &Attributes, int &StartStop );
string Parse_XML( string XML, string Tag, string &Attributes );
string Parse_XML( string XML, string Tag );
void   Parse_XML_ListAllTags( string XML, vector<string> &List );

// -----------------------------------------------------------------------------

#endif
