/* []--------------------------------------------------------[]
    |                      qsub_main.cpp                     |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 24 August 2007.                      |
    | USE:        Implements main for file qsub tool...      |
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
#include "interface_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "daemon_jobclass.h"

// ----------------------------------------------------------------------

string KeyFile, CertificateFile, CACertificateFile, ServerURL, Project;

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] " << endl << endl;
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
 DaemonJob Job;

 // check number of arguments first...
 if( argc < 2 )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // turn logging facilities off...
 InitializeLogger( 0 );

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-j" ) ) {
    if( argv[ ++i ] ) 
    {
     DaemonJob TempJob( ( string( argv[ i ] ) ) );

     if( TempJob.GetJobDirectory().empty() ) {
      cout << "directory " << argv[ i ] << " not a valid job directory" << endl;
      return( 1 );
     }

     Job = TempJob;
     KeyFile = Job.GetKeyFile();
     CertificateFile = Job.GetCertificateFile();
     CACertificateFile = Job.GetCACertificateFile();
     ServerURL = Job.GetThisProjectServer();
     Project = Job.GetProject();
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-k" ) ) {
    if( argv[ ++i ] )
     KeyFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] )
     CertificateFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-ca" ) ) {
    if( argv[ ++i ] )
     CACertificateFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-s" ) ) {
    if( argv[ ++i ] )
     ServerURL = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-p" ) ) {
    if( argv[ ++i ] )
     Project = string( argv[ i ] );
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

 // if no options passed try local directory as jobdirectory...
 if( KeyFile.empty() && CertificateFile.empty() && CACertificateFile.empty() && ServerURL.empty() && Project.empty() )
 {
  DaemonJob TempJob( "." );

  if( !TempJob.GetJobDirectory().empty() )
  {
   Job = TempJob;
   KeyFile = Job.GetKeyFile();
   CertificateFile = Job.GetCertificateFile();
   CACertificateFile = Job.GetCACertificateFile();
   ServerURL = Job.GetThisProjectServer();
   Project = Job.GetProject();
  }
  else
  {
   cout << "current directory not a valid job directory" << endl;
   return( 1 );
  }
 }

 int Flag = 0;

 if( KeyFile.empty() ) Flag = 1;
 if( CertificateFile.empty() ) Flag = 1;
 if( CACertificateFile.empty() ) Flag = 1;
 if( ServerURL.empty() ) Flag = 1;
 if( Project.empty() ) Flag = 1;

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // ...
 // ...
 // ...

 return( 0 );
}
