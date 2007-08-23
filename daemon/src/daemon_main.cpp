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

int main( int *argc, char *argv[] )
{

 // read passed arguments here and perhaps setup a signal handler...

 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING,"testmain.log");
 
 TheDaemon = new Daemon( "LGI.cfg" );

 if( TheDaemon != NULL )
 {
  if( TheDaemon -> IsSchedularReady() )
   while( TheDaemon -> RunSchedular() );
  delete TheDaemon;
 }

}

