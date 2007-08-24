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

int main( int *argc, char *argv[] )
{

 // read passed arguments here...

 signal( SIGINT, TheSignalHandler );
 signal( SIGQUIT, TheSignalHandler );
 signal( SIGABRT, TheSignalHandler );
 signal( SIGTERM, TheSignalHandler );

 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING);
 
 TheDaemon = new Daemon( "LGI.cfg" );
 
 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING|VERBOSE_DEBUG_LOGGING);
 
 if( TheDaemon != NULL )
 {
  if( TheDaemon -> IsSchedularReady() )
   while( TheDaemon -> RunSchedular() );
  delete TheDaemon;
 }

 NORMAL_LOG( "Main; Daemon finished" );
}

