/* []--------------------------------------------------------[]
    |                   daemon_main.cpp                      |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
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

string ReadLineFromFile( string FileName )
{
 fstream File( FileName.c_str(), fstream::in );
 string Line;

 if( !File ) return( Line );
 getline( File, Line );
 return( Line );
}

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] configfile" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h           this help." << endl;
 cout << "-d           daemonize and run in background." << endl;
 cout << "-q           log only critical messages." << endl;
 cout << "-n           log normal messages. this is the default." << endl;
 cout << "-v           also log debug messages." << endl;
 cout << "-vv          also log verbose debug messages." << endl;
 cout << "-l file      use specified logfile. default is to log to standard output." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 int    LogLevel = CRITICAL_LOGGING | NORMAL_LOGGING;
 int    ConfigFileSet = 0;
 int    Daemonize = 0;
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
   if( argv[ ++i ] )
    LogFile = string( argv[ i ] );
   else
   {
    PrintHelp( argv[ 0 ] );
    return( 1 );
   }
  } else if( !strcmp( argv[ i ], "-d" ) ) {
   Daemonize = 1;
  } else if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else {
   ConfigFile = string( argv[ i ] );
   string Data = ReadStringFromFile( ConfigFile );
   if( Data.empty() )
   {
    cout << endl << "Could not read from configuration file " << ConfigFile << endl << endl;
    return( 1 );
   }
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
  {
   if( Daemonize )
   {
    if( daemon( 1, 0 ) == 0 )
    {
     setsid();
     setpgid( 0, 0 );
     while( TheDaemon -> RunSchedular() );
     delete TheDaemon;
     NORMAL_LOG( "Main; Daemon finished normally" );
     _exit( 0 );
    }
   }
   else
   {
    while( TheDaemon -> RunSchedular() );
    delete TheDaemon;
    NORMAL_LOG( "Main; Daemon finished normally" );
   }
  }
  else
   CRITICAL_LOG_RETURN( 1, "Main; Daemon stopped" );
 }
 
 return( 0 );
}
