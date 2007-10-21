/* []--------------------------------------------------------[]
    |                    hexbin_main.cpp                     |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 29 August 2007.                      |
    | USE:        Implements main for hexbin tool...         |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include <iostream>
#include <fstream>
#include <string>

#include "binhex.h"

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options]" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h           this help." << endl;
 cout << "-i file      specify input file. default is standard input." << endl;
 cout << "-o file      specify output file. default is standard output." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 char Buffer[ 1024 ];
 fstream Input( "/dev/stdin", fstream::in | fstream::binary );
 fstream Output( "/dev/stdout", fstream::out | fstream::binary );
 string  Bin;

 // read passed arguments here...

 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-i" ) ) {
    if( argv[ ++i ] )
    {
     if( !Input.good() ) Input.clear();
     if( Input.is_open() ) Input.close();
     Input.open( argv[ i ], fstream::in | fstream::binary );
     if( !Input ) { Output << "input error on file " << argv[ i ] << endl; return( 1 ); }
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-o" ) ) {
    if( argv[ ++i ] )
    {
     if( !Output.good() ) Output.clear();
     if( Output.is_open() ) Output.close();
     Output.open( argv[ i ], fstream::out | fstream::binary );
     if( !Output ) return( 1 );
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else {
    PrintHelp( argv[ 0 ] );
    return( 1 );
  };
 }

 Bin.reserve( 512 );

 while( Input )
 {
  Input.read( Buffer, 1024 );
  if( Input.gcount() )
  {
   HexBin( string( Buffer, Input.gcount() ), Bin );
   Output << Bin;
  }
 }

 return( 0 );
}
