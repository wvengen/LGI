/* []--------------------------------------------------------[]
    |                     csv_main.cpp                       |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 29 August 2007.                      |
    | USE:        Implements main for csv tool...            |
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

#include <vector>

#include "csv.h"

// ----------------------------------------------------------------------

string ReadStringFromFile( fstream &File )
{
 char TempBuffer[ 1024 ];
 string Buffer;

 Buffer.reserve( 4096 );
 Buffer.clear();

 while( File )
 {
  File.read( TempBuffer, 1024 );
  if( File.gcount() )
   Buffer.append( string( TempBuffer, File.gcount() ) );
 }

 return( Buffer );
}

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] [csvlist]" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h           this help." << endl;
 cout << "-i file      specify input file. default is standard input." << endl;
 cout << "-o file      specify output file. default is standard output." << endl;
 cout << "-s char      set seperator character. default is ','." << endl;
 cout << "-n           output number of values." << endl;
 cout << "-l           output list of values. this is the default." << endl;
 cout << "-e number    output item number." << endl;
 cout << "-nl          output number and list of values." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 vector<string> ValueList;
 fstream Input( "/dev/stdin", fstream::in | fstream::binary );
 fstream Output( "/dev/stdout", fstream::out | fstream::binary );
 string  CVSList;
 int     PrintNumberOption = 0;
 int     PrintItemOption = 0;
 int     PrintListOption = 0;
 char    Separator = ',';

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
  } else if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-n" ) ) {
    PrintNumberOption = 1;
  } else if( !strcmp( argv[ i ], "-e" ) ) {
    PrintItemOption = atoi( argv[ ++i ] );
  } else if( !strcmp( argv[ i ], "-l" ) ) {
    PrintListOption = 1;
  } else if( !strcmp( argv[ i ], "-nl" ) ) {
    PrintNumberOption = 1;
    PrintListOption = 1;
  } else if( !strcmp( argv[ i ], "-s" ) ) {
    Separator = argv[ ++i ][ 0 ];
  } else {
    CVSList.append( string( argv[ i ] ) );
  };
 }

 if( !PrintNumberOption && !PrintListOption && !PrintItemOption ) PrintListOption = 1;

 if( CVSList.empty() )
  CVSList = ReadStringFromFile( Input );

 ValueList = CommaSeparatedValues2Array( CVSList, Separator );

 if( PrintNumberOption )
  Output << ValueList.size() << endl;

 if( PrintListOption )
 {
  for( int i = 0; i < ValueList.size(); ++i )
   Output << ValueList[ i ] << endl;
 }

 if( ( PrintItemOption > 0 ) && ( PrintItemOption <= ValueList.size() ) )
  Output << ValueList[ PrintItemOption - 1 ] << endl;

 return( 0 );
}
