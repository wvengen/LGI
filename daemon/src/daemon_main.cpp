/* []--------------------------------------------------------[]
    |                   daemon_main.cpp                      |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 24 August 2007.                      |
    | USE:        Implements main for scheduler...           |
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
#include "daemon_mainclass.h"

// ----------------------------------------------------------------------

Daemon *TheDaemon = NULL;

// ----------------------------------------------------------------------

void TheSignalHandler( int S )
{
 if( TheDaemon != NULL ) 
 {
  CRITICAL_LOG( "TheSignalHandler; Received signal, stoping with scheduling gracefully" );
  TheDaemon -> StopScheduling();
 }
}

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] configfile" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-q           log only critical messages" << endl;
 cout << "-n           log normal messages (default)" << endl;
 cout << "-v           also log debug messages" << endl;
 cout << "-vv          also log verbose debug messages" << endl;
 cout << "-l file      use specified logfile, default is standard output" << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 int    LogLevel = CRITICAL_LOGGING | NORMAL_LOGGING, ConfigFileSet = 0;
 string LogFile( "/dev/stdout" );
 string ConfigFile( "LGI.cfg" );

 // check arguments here...

 if( argc <= 1 )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }
 
 // read passed arguments here...

 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-q" ) ) {
   LogLevel = CRITICAL_LOGGING;
  } else if( !strcmp( argv[ i ], "-n" ) ) {
   LogLevel = CRITICAL_LOGGING | NORMAL_LOGGING;
  } else if( !strcmp( argv[ i ], "-v" ) ) {
   LogLevel = CRITICAL_LOGGING | NORMAL_LOGGING | DEBUG_LOGGING;
  } else if( !strcmp( argv[ i ], "-vv" ) ) {
   LogLevel = CRITICAL_LOGGING | NORMAL_LOGGING | DEBUG_LOGGING | VERBOSE_DEBUG_LOGGING;
  } else if( !strcmp( argv[ i ], "-l" ) ) {
   LogFile = string( argv[ ++i ] );
  } else {
   ConfigFile = string( argv[ i ] );
   ConfigFileSet = 1;
  };
 }

 if( !ConfigFileSet ) 
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // setup signal handlers...

 signal( SIGINT, TheSignalHandler );
 signal( SIGQUIT, TheSignalHandler );
 signal( SIGABRT, TheSignalHandler );
 signal( SIGTERM, TheSignalHandler );

 InitializeLogger( LogLevel, LogFile.c_str() );
 
 TheDaemon = new Daemon( ConfigFile );
 
 if( TheDaemon != NULL )
 {
  if( TheDaemon -> IsSchedularReady() )
   while( TheDaemon -> RunSchedular() );
  delete TheDaemon;
 }

 NORMAL_LOG( "Main; Daemon finished" );
}

