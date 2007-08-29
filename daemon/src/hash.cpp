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

string Hash( string S, string StartHash )
{
 string TheHash( "0123456789ABCDEF" );
 int b, r, i, n1, n2, StartIndex, EndIndex, NrBlocks;
 char s;

 if( ( !StartHash.empty() ) && ( StartHash.size() == 16 ) )
  TheHash = StartHash;

 NrBlocks = ( S.length() / HASH_BLOCK_SIZE ) + 1;           

 for( b = 0; b < NrBlocks; ++b )             // run through string in blocks of size HASH_BLOCK_SIZE
 {
  StartIndex = b * HASH_BLOCK_SIZE;
  EndIndex = ( b + 1 ) * HASH_BLOCK_SIZE;

  if( StartIndex >= S.length() ) continue;                 // check the bounds
  if( EndIndex >= S.length() ) EndIndex = S.length();

  for( r = 0; r < 16; ++r )                        // run through block
   for( i = StartIndex; i < EndIndex; ++i )
   {
    n1 = ( int )( S[ i ] >> 4 ) & 0x0F;
    n2 = ( int )( S[ i ] ) & 0x0F;

    s = TheHash[ n1 ];
    TheHash[ n1 ] = TheHash[ n2 ];
    TheHash[ n2 ] = s;

    TheHash[ r ] ^= ( char )( S[ i ] + ( i - StartIndex ) );
   }
 }

 return( TheHash );
}

// -----------------------------------------------------------------------------

