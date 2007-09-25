/* []--------------------------------------------------------[]
    |                     qdel_main.cpp                      |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 24 August 2007.                      |
    | USE:        Implements main for qdel tool...           |
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
#include "interface_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"

// ----------------------------------------------------------------------

string KeyFile, CertificateFile, CACertificateFile, ServerURL, 
       Project, State, Application, User, Groups, Job_Id, ConfigDir;

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
 cout << endl << ExeName << " [options] job_id" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                         show this help." << endl;
 cout << "-c directory               specify configuration directory to read. default is ~/.LGI. use the options below to overrule those settings." << endl; 
 cout << "-P project                 specify project name. if not specified, the default project of the server is assumed." << endl; 
 cout << "-S serverurl               specify project server to query." << endl;
 cout << "-U user                    specify username." << endl;
 cout << "-G groups                  specify groups." << endl;
 cout << "-K keyfile                 specify key file." << endl;
 cout << "-C certificatefile         specify certificate file." << endl;
 cout << "-CA cacertificatefile      specify ca certificate file." << endl << endl;
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

 // turn logging facilities off...
 InitializeLogger( 0 );

 // setup default values from default configuration directory...
 ConfigDir = string( getenv( "HOME" ) ) + "/.LGI";

 User = ReadLineFromFile( ConfigDir + "/user" );
 Groups = ReadLineFromFile( ConfigDir + "/groups" );
 ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
 Project = ReadLineFromFile( ConfigDir + "/defaultproject" );
 if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
 if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
 if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] )
    {
     ConfigDir = string( argv[ i ] );

     User = ReadLineFromFile( ConfigDir + "/user" );
     Groups = ReadLineFromFile( ConfigDir + "/groups" );
     ServerURL = ReadLineFromFile( ConfigDir + "/defaultserver" );
     Project = ReadLineFromFile( ConfigDir + "/defaultproject" );
     if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
     if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
     if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";
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
  } else if( !strcmp( argv[ i ], "-P" ) ) {
    if( argv[ ++i ] )
     Project = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else {
    int Dummy;

    if( sscanf( argv[ i ], "%d", &Dummy ) == 1 )
     Job_Id = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }

  };
 }

 int Flag = 0;

 if( KeyFile.empty() ) Flag = 1;
 if( CertificateFile.empty() ) Flag = 1;
 if( CACertificateFile.empty() ) Flag = 1;
 if( ServerURL.empty() ) Flag = 1;
 if( User.empty() ) Flag = 1;
 if( Groups.empty() ) Flag = 1;
 if( Job_Id.empty() ) Flag = 1;

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
