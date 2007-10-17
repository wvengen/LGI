/* []--------------------------------------------------------[]
    |                  filetransfer_main.cpp                 |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
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

#define CMD_REMOVE 1
#define CMD_MOVE   2
#define CMD_COPY   4
#define CMD_SERVE  8

// ----------------------------------------------------------------------

int Command = CMD_SERVE, ResourceMode = 0;                   

string KeyFile, CertificateFile, CACertificateFile, 
       ServerURL, Project, User, Groups, SourceFile,
       DestinationFile;

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
 cout << endl << ExeName << " [options] command" << endl << endl;
 cout << "commands:" << endl << endl;
 cout << "serve                               run program as server serving file transfer request jobs from a job directory." << endl;
 cout << "remove file                         remove specified file." << endl;
 cout << "move sourcefile destinationfile     move specified file to specified destination. either source or destination needs to be local." << endl;
 cout << "copy sourcefile destinationfile     copy specified file to specified destination. either source or destination needs to be local." << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                                  show this help." << endl;
 cout << "-c directory                        specify the configuration directory to read. default is ~/.LGI. specify options below to overrule." << endl;
 cout << "-j jobdirectory                     specify job directory to use. if not specified try current directory or specify the following options." << endl;
 cout << "-K keyfile                          specify key file." << endl;
 cout << "-C certificatefile                  specify certificate file." << endl;
 cout << "-CA cacertificatefile               specify ca certificate file." << endl;
 cout << "-U user                             specify username." << endl;
 cout << "-G groups                           specify groups." << endl;
 cout << "-S serverurl                        specify project server url." << endl;
 cout << "-P project                          specify project name." << endl;
 cout << "-m                                  switch user or resource mode." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 DaemonJob Job;

 // turn logging facilities off...
 InitializeLogger( 0 );

 // try and read current job directory first as a default...
 DaemonJob TempJob( "." );
 if( !TempJob.GetJobDirectory().empty() )
 {
  Job = TempJob;
  KeyFile = Job.GetKeyFile();
  CertificateFile = Job.GetCertificateFile();
  CACertificateFile = Job.GetCACertificateFile();
  ServerURL = Job.GetThisProjectServer();
  Project = Job.GetProject();

  ResourceMode = 1;
  Command = CMD_SERVE;
 }

 // if that didn't work, try and read default config from ~/.LGI...
 if( !ResourceMode )
 {
  string ConfigDir = string( getenv( "HOME" ) ) + "/.LGI";

  User = ReadLineFromFile( ConfigDir + "/user" );
  Groups = ReadLineFromFile( ConfigDir + "/groups" );
  ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
  Project = ReadLineFromFile( ConfigDir + "/defaultproject" );

  if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
  if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
  if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

  ResourceMode = 0;
  Command = 0;
 }

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

     ResourceMode = 1;
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] )
    {
     string ConfigDir = string( argv[ i ] );

     if( !ReadLineFromFile( ConfigDir + "/user" ).empty() ) User = ReadLineFromFile( ConfigDir + "/user" );
     if( !ReadLineFromFile( ConfigDir + "/groups" ).empty() ) Groups = ReadLineFromFile( ConfigDir + "/groups" );
     if( !ReadLineFromFile( ConfigDir + "/defaultserver" ).empty() ) ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
     if( !ReadLineFromFile( ConfigDir + "/defaultproject" ).empty() ) Project = ReadLineFromFile( ConfigDir + "/defaultproject" );
     if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
     if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
     if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

     ResourceMode = 0;
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-K" ) ) {
    if( argv[ ++i ] )
     KeyFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-C" ) ) {
    if( argv[ ++i ] )
     CertificateFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-CA" ) ) {
    if( argv[ ++i ] )
     CACertificateFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-S" ) ) {
    if( argv[ ++i ] )
     ServerURL = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-P" ) ) {
    if( argv[ ++i ] )
     Project = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-U" ) ) {
    if( argv[ ++i ] )
    {
     User = string( argv[ i ] );
     ResourceMode = 0;
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-G" ) ) {
    if( argv[ ++i ] )
    {
     Groups = string( argv[ i ] );
     ResourceMode = 0;
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-m" ) ) {
    ResourceMode = !ResourceMode;
  } else if( !strcmp( argv[ i ], "serve" ) ) {
    if( ResourceMode )
     Command = CMD_SERVE;
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "remove" ) ) {
    if( argv[ ++i ] )
    {
     Command = CMD_REMOVE;
     SourceFile = DestinationFile = string( argv[ i ] );
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "move" ) ) {
    if( argv[ ++i ] )
     SourceFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    if( argv[ ++i ] )
    {
     Command = CMD_MOVE;
     DestinationFile = string( argv[ i ] );
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "copy" ) ) {
    if( argv[ ++i ] )
     SourceFile = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    if( argv[ ++i ] )
    {
     Command = CMD_COPY;
     DestinationFile = string( argv[ i ] );
    }
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

 int Flag = 0;

 if( KeyFile.empty() ) Flag = 1;
 if( CertificateFile.empty() ) Flag = 1;
 if( CACertificateFile.empty() ) Flag = 1;
 if( ServerURL.empty() ) Flag = 1;
 if( User.empty() && ( !ResourceMode ) ) Flag = 1;
 if( Groups.empty() && ( !ResourceMode ) ) Flag = 1;
 if( !Command ) Flag = 1;

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
