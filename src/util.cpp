/* []--------------------------------------------------------[]
    |                       util.cpp                         |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     W. van Engen                               |
    | VERSION:    1.00, 31 January 2012.                     |
    | USE:        General utility functions...               |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2012 W. van Engen, Nikhef / Stichting FOM
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "util.h"

// ----------------------------------------------------------------------

string ReadLineFromFile( string FileName )
{
 fstream File( FileName.c_str(), fstream::in );
 string Line;

 if( !File ) return( Line );
 getline( File, Line );
 return( Line );
}

// -----------------------------------------------------------------------------

string ReadStringFromFile( string FileName, int MaxSize )
{
 char TempBuffer[ 1024 ];
 fstream File( FileName.c_str(), fstream::in | fstream::binary );
 string Buffer;

 Buffer.reserve( 4096 );
 Buffer.clear();

 while( File )
 {
  File.read( TempBuffer, 1024 );
  if( File.gcount() ) Buffer.append( string( TempBuffer, File.gcount() ) );
  if( ( MaxSize > 0 ) && ( Buffer.size() >= MaxSize ) ) break;
 }
 if( ( MaxSize > 0 ) && ( Buffer.size() > MaxSize ) ) 
 {
  Buffer.erase( MaxSize );
  CRITICAL_LOG_RETURN( Buffer, "ReadStringFromFile; Data of file " << FileName << " truncated to " << MaxSize << " bytes" );
 }

 VERBOSE_DEBUG_LOG_RETURN( Buffer, "ReadStringFromFile; Data returned from file " << FileName << ": '" << Buffer << "'" );
}

// -----------------------------------------------------------------------------

void WriteStringToFile( string String, string FileName )
{
 fstream File( FileName.c_str(), fstream::out | fstream::binary );

 File << String;

 VERBOSE_DEBUG_LOG( "WriteStringToFile; Wrote file " << FileName << " with String=" << String );
}

// -----------------------------------------------------------------------------

string AbsolutePath( string FileName )
{
 char TempBuffer[ 1024 ];
 
 if( FileName[ 0 ] == '/' ) return( FileName );

 if( !getcwd( TempBuffer, sizeof( TempBuffer ) ) )
  CRITICAL_LOG_RETURN( FileName, "AbsolutePath; could not obtain current directory: " << strerror( errno ) );
 
 string Result( TempBuffer ); 

 Result = Result + "/" + FileName;

 VERBOSE_DEBUG_LOG_RETURN( Result, "AbsolutePath; returned " << Result );
}

// -----------------------------------------------------------------------------

string BaseName( string& Path )
{
  int idx = Path.rfind( '/' );
  if( idx ) return( Path.substr( idx + 1 ) );
  return( Path );
}

// -----------------------------------------------------------------------------

bool FileExists( string Path )
{
 struct stat FileStat;
 return stat( Path.c_str(), &FileStat ) == 0;
}

// -----------------------------------------------------------------------------

bool DirectoryExists( string Path )
{
 struct stat FileStat;
 return stat( Path.c_str(), &FileStat ) == 0 && S_ISDIR(FileStat.st_mode);
}

// -----------------------------------------------------------------------------
