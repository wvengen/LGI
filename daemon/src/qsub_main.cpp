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

int ResourceMode = 0;                       // default to user mode...

string KeyFile, CertificateFile, CACertificateFile, 
       ServerURL, Project, Response, ConfigDir,
       Application, Target_Resources( "any" ), 
       Job_Specifics, Owners, Read_Access, Input, 
       Output, User, Groups;

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
 cout << endl << ExeName << " -a application [options] " << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                                  show this help." << endl;
 cout << "-a application                      specify the application." << endl;
 cout << "-t targetresources                  specify the target resources." << endl;
 cout << "-s jobspecs                         specify the job specifics." << endl;
 cout << "-o owners                           specify the job owners." << endl;
 cout << "-i inputfile                        specify the job input file." << endl;
 cout << "-r readaccesslist                   specify the job readaccess list." << endl;
 cout << "-c directory                        specify the configuration directory to read. default is ~/.LGI. specify options below to overrule." << endl;
 cout << "-j jobdirectory                     specify job directory to use. if not specified try current directory or specify the following options." << endl;
 cout << "-K keyfile                          specify key file." << endl;
 cout << "-C certificatefile                  specify certificate file." << endl;
 cout << "-CA cacertificatefile               specify ca certificate file." << endl;
 cout << "-S serverurl                        specify project server url." << endl;
 cout << "-U user                             specify username." << endl;
 cout << "-G groups                           specify groups." << endl;
 cout << "-P project                          specify project name." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 DaemonJob Job;

 // turn logging facilities off...
 InitializeLogger( 0 );

 // check number of arguments first...
 if( argc < 2 )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

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
  Owners = Job.GetOwners();
  Read_Access = Job.GetReadAccess();

  ResourceMode = 1;
 }

 // if that didn't work, try and read default config from ~/.LGI...
 if( !ResourceMode )
 {
  ConfigDir = string( getenv( "HOME" ) ) + "/.LGI";

  User = ReadLineFromFile( ConfigDir + "/user" );
  Groups = ReadLineFromFile( ConfigDir + "/groups" );
  ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
  Project = ReadLineFromFile( ConfigDir + "/defaultproject" );

  if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
  if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
  if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

  ResourceMode = 0;
 }

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-c" ) ) {
     ConfigDir = string( argv[ i ] );

     if( !ReadLineFromFile( ConfigDir + "/user" ).empty() ) User = ReadLineFromFile( ConfigDir + "/user" );
     if( !ReadLineFromFile( ConfigDir + "/groups" ).empty() ) Groups = ReadLineFromFile( ConfigDir + "/groups" );
     if( !ReadLineFromFile( ConfigDir + "/defaultserver" ).empty() ) ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
     if( !ReadLineFromFile( ConfigDir + "/defaultproject" ).empty() ) Project = ReadLineFromFile( ConfigDir + "/defaultproject" );
     if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
     if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
     if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

     ResourceMode = 0;
  } else if( !strcmp( argv[ i ], "-j" ) ) {
    if( argv[ ++i ] ) 
    {
     DaemonJob TempJob( ( string( argv[ i ] ) ) );

     if( TempJob.GetJobDirectory().empty() ) {
      cout << endl << "Directory " << argv[ i ] << " is not a valid job directory" << endl;
      return( 1 );
     }

     Job = TempJob;
     KeyFile = Job.GetKeyFile();
     CertificateFile = Job.GetCertificateFile();
     CACertificateFile = Job.GetCACertificateFile();
     ServerURL = Job.GetThisProjectServer();
     Project = Job.GetProject();
     Owners = Job.GetOwners();
     Read_Access = Job.GetReadAccess();

     ResourceMode = 1;
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
  } else if( !strcmp( argv[ i ], "-a" ) ) {
    if( argv[ ++i ] )
     Application = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-o" ) ) {
    if( argv[ ++i ] )
     Owners = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-r" ) ) {
    if( argv[ ++i ] )
     Read_Access = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-t" ) ) {
    if( argv[ ++i ] )
     Target_Resources = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-s" ) ) {
    if( argv[ ++i ] )
     Job_Specifics = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-U" ) ) {
    if( argv[ ++i ] )
     User = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-G" ) ) {
    if( argv[ ++i ] )
     Groups = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-i" ) ) {
    if( argv[ ++i ] )
    {
     Input = string( argv[ i ] );

     // ...
     // ...
     // ...

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
 if( Application.empty() ) Flag = 1;
 if( User.empty() && ( !ResourceMode ) ) Flag = 1;
 if( Groups.empty() && ( !ResourceMode ) ) Flag = 1;

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // now act accordingly... first see if we are in user mode or not...

 if( ResourceMode )
 {
  Resource_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile );         

  // ...
  // ...
  // ...

 }
 else
 {
  Interface_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile );         

  

 }

 return( 0 );
}
