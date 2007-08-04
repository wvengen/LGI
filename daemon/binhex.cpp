/* []--------------------------------------------------------[]
    |                      binhex.cpp                        |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 2 August 2007.                       |
    | USE:        Implements binhex and hexbin...            |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "binhex.h"

// -----------------------------------------------------------------------------

void BinHex( const string& Bin, string &Hex )
{
 const char HEXDIGITS[] = "0123456789ABCDEF";
 int i, j, Length = Bin.length();

 Hex.reserve( Length << 1 + 1 );
 Hex.clear();
 
 for( j = i = 0; i < Length; i++ )
 {
  Hex.push_back( HEXDIGITS[ Bin[ i ] >> 4 ] );
  Hex.push_back( HEXDIGITS[ Bin[ i ] & 0x0F ] );
 }

}

// -----------------------------------------------------------------------------

void HexBin( const string& Hex, string &Bin )
{
 const string HEXDIGITS = "0123456789ABCDEF";
 int i, Length = Hex.length();
 int HighNible, LowNible;

 Bin.reserve( Length >> 1 + 1 );
 Bin.clear();

 i = 0;
 while( isspace( Hex[ i ] ) ) i++;

 for( ; i < Length; i += 2 )
 {
  HighNible = HEXDIGITS.find( Hex[ i ], 0 );
  LowNible = HEXDIGITS.find( Hex[ i + 1 ], 0 );
  Bin.push_back( ( ( HighNible << 4 ) | LowNible ) & 0xFF );
 }

}

// -----------------------------------------------------------------------------

