/* []--------------------------------------------------------[]
    |                    hexbin_main.cpp                     |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
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
 fstream Input( "/dev/stdin", fstream::in | fstream::binary );
 fstream Output( "/dev/stdout", fstream::out | fstream::binary );
 string  Bin, Hex, Line;

 // read passed arguments here...

 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-i" ) ) {
    Input.close();
    Input.open( argv[ ++i ], fstream::in | fstream::binary );
    if( !Input ) { Output << "input error" << endl; return( 1 ); }
  } else if( !strcmp( argv[ i ], "-o" ) ) {
    Output.close();
    Output.open( argv[ ++i ], fstream::out | fstream::binary );
    if( !Output ) return( 1 );
  } else {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  };
 }

 Hex.reserve( 1024 );
 Hex.clear();

 getline( Input, Line );
 while( Input )
 {
  Hex.append( Line );
  Hex.push_back( '\n' );
  getline( Input, Line );
 }

 if( Hex.length() >= 1 )
  Hex = Hex.substr( 0, Hex.length() - 1 );
 else
  Hex.clear();

 HexBin( Hex, Bin );

 Output << Bin;
  
 return( 0 );
}
