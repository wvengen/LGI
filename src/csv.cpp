/* []--------------------------------------------------------[]
    |                      csv.cpp                           |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
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

#include "csv.h"

// -----------------------------------------------------------------------------

string NormalizeString( string S )
{
 string Empty;
 int Start = 0,
     End = S.size();

 Empty.clear();
 
 while( ( Start < End ) && isspace( S[ Start ] ) ) Start++;   // ignore spaces in front of the string
 if( Start >= End ) return( Empty );

 while( ( End > Start ) && isspace( S[ End - 1 ] ) ) End--;   // and ignore spaces at the end of the string
 if( End <= Start ) return( Empty );

 return( S.substr( Start, End - Start ) );
}

// -----------------------------------------------------------------------------

vector<string> CommaSeparatedValues2Array( string Values, char Separator )
{
 vector<string> Array;

 Array.push_back( "" );
 
 for( int i = 0; i < Values.length(); ++i )
  if( Values[ i ] != Separator ) 
   Array[ Array.size() - 1 ].push_back( Values[ i ] );
  else
   Array.push_back( "" );

 for( int i = 0; i < Array.size(); ++i )
 {
  Array[ i ] = NormalizeString( Array[ i ] ); 

  if( Array[ i ].length() == 0 )
  {
   if( Array.size() > 1 ) Array[ i ] = Array[ Array.size() - 1 ];
   Array.pop_back();
   --i;
  }
 } 

 return( Array );
}

// -----------------------------------------------------------------------------

string NormalizeCommaSeparatedValues( string Values, char Separator )
{
 vector<string> Array = CommaSeparatedValues2Array( Values, Separator );

 for( int i = 1; i < Array.size(); ++i )
 {
  Array[ 0 ].append( 1, Separator );
  Array[ 0 ].append( 1, ' ' );
  Array[ 0 ].append( Array[ i ] );
 }

 if( Array.size() > 1 )
  return( Array[ 0 ] );
 else
  return( "" );
}

// -----------------------------------------------------------------------------
