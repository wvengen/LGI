/* []--------------------------------------------------------[]
    |                     hash.cpp                           |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
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

string Hash( string S )
{
 string TheHash( "0123456789ABCDEF" );
 int r, i, n1, n2;
 char s;

 for( r = 0; r < 16; ++r )
  for( i = 0; i < S.length(); ++i )
  {
   n1 = ( int )( S[ i ] >> 4 ) & 0x0F;
   n2 = ( int )( S[ i ] ) & 0x0F;

   s = TheHash[ n1 ];
   TheHash[ n1 ] = TheHash[ n2 ];
   TheHash[ n2 ] = s;

   TheHash[ r ] ^= ( char )( S[ i ] + i );
  }

 return( TheHash );
}

// -----------------------------------------------------------------------------

