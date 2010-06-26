/* []--------------------------------------------------------[]
    |                     hash.cpp                           |
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

#include "hash.h"

// -----------------------------------------------------------------------------

string Hash( string S, string StartHash )
{
 string TheHash( "0123456789ABCDEF" );
 int i;
 unsigned char s, h, n0, n1, n2;

 if( ( !StartHash.empty() ) && ( StartHash.size() == 16 ) )
  TheHash = StartHash;

 for( i = n0 = 0; i < S.length(); n0 = ( ++i & 0x0F ) )
 {
  s = S[ i ];
  h = TheHash[ n0 ];

  n1 = ( ( s + h ) >> 4 ) & 0x0F;
  n2 = ( ( s - h ) ) & 0x0F;

  TheHash[ n0 ] = h ^ ( s + n0 );
  
  s = TheHash[ n1 ];
  TheHash[ n1 ] = TheHash[ n2 ];
  TheHash[ n2 ] = s;
 }

 return( TheHash );
}

// -----------------------------------------------------------------------------

