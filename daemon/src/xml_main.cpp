/* []--------------------------------------------------------[]
    |                     xml_main.cpp                       |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 29 August 2007.                      |
    | USE:        Implements main for xml tool...            |
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
#include <cstring>

#include "xml.h"
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
 cout << endl << ExeName << " [options] tag [attribute]" << endl << endl;
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
 string  Data, Tag, Attribute, DataFound, AttributesFound;
 int StartPos = 0;

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
  } else if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else {
    if( Tag.empty() )
     Tag = string( argv[ i ] );
    else
    {
     if( !Attribute.empty() ) Attribute = Attribute + ",";
     Attribute = Attribute + string( argv[ i ] );
    }
  };
 }

 if( Tag.empty() )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 Data = ReadStringFromFile( Input );
 do
 {
  DataFound = NormalizeString( Parse_XML( Data, Tag, AttributesFound, StartPos ) );
  if( Attribute.empty() ) break;
 } while( ( !DataFound.empty() ) && ( AttributesFound != Attribute ) );

 if( !DataFound.empty() )
 {
  Output << DataFound << endl;
  return( 0 );
 }

 return( 1 );
}
