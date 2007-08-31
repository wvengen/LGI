/* []--------------------------------------------------------[]
    |                  filetransfer_main.cpp                 |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 24 August 2007.                      |
    | USE:        Implements main for file transfer tool...  |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include <signal.h>
#include <sys/wait.h>

#include "logger.h"
#include "resource_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "daemon_jobclass.h"

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] command" << endl << endl;
 cout << "commands:" << endl << endl;
 cout << "serve                               run program as server serving file transfer request jobs from a job directory." << endl;
 cout << "remove file                         remove specified file." << endl;
 cout << "move sourcefile destinationfile     move specified file to specified destination." << endl;
 cout << "copy sourcefile destinationfile     copy specified file to specified destination." << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                                  show this help." << endl;
 cout << "-j jobdirectory                     specify job directory to use. if not specified try current directory or specify the following options." << endl;
 cout << "-k keyfile                          specify key file." << endl;
 cout << "-c certificatefile                  specify certificate file." << endl;
 cout << "-ca cacertificatefile               specify ca certificate file." << endl;
 cout << "-s serverurl                        specify project server url." << endl;
 cout << "-p project                          specify project name." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{

 // check number of arguments first...
 if( argc < 2 )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-j" ) ) {
  } else if( !strcmp( argv[ i ], "-k" ) ) {
  } else if( !strcmp( argv[ i ], "-c" ) ) {
  } else if( !strcmp( argv[ i ], "-ca" ) ) {
  } else if( !strcmp( argv[ i ], "-s" ) ) {
  } else if( !strcmp( argv[ i ], "-p" ) ) {
  } else if( !strcmp( argv[ i ], "serve" ) ) {
  } else if( !strcmp( argv[ i ], "remove" ) ) {
  } else if( !strcmp( argv[ i ], "move" ) ) {
  } else if( !strcmp( argv[ i ], "copy" ) ) {
  } else {
  };
 }

 return( 0 );
}
