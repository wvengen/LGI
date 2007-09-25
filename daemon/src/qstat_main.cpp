/* []--------------------------------------------------------[]
    |                     qstat_main.cpp                     |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 24 August 2007.                      |
    | USE:        Implements main for qstat tool...          |
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
#include "daemon_configclass.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"

// ----------------------------------------------------------------------

string KeyFile, CertificateFile, CACertificateFile, ServerURL, Response,
       Project, State, Application, User, Groups, Job_Id, ConfigDir;
int OutputInXML = 0;
int ListServers = 0;

// ----------------------------------------------------------------------

void PrintHelp( char *ExeName )
{
 cout << endl << ExeName << " [options] [job_id]" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                         show this help." << endl;
 cout << "-a application             specify application to query about. default is any." << endl;
 cout << "-s state                   specify job state to query about. default is any." << endl;
 cout << "-x                         output in XML format." << endl;
 cout << "-l                         report porject server list in output." << endl;
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

 User = ReadStringFromFile( ConfigDir + "/user" );
 Groups = ReadStringFromFile( ConfigDir + "/groups" );
 ServerURL = ReadStringFromFile( ConfigDir + "/defaultserver" );
 Project = ReadStringFromFile( ConfigDir + "/defaultproject" );

 if( !ReadStringFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
 if( !ReadStringFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
 if( !ReadStringFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-x" ) ) {
     OutputInXML = 1;
  } else if( !strcmp( argv[ i ], "-l" ) ) {
     ListServers = 1;
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] )
    {
     ConfigDir = string( argv[ i ] );

     User = ReadStringFromFile( ConfigDir + "/user" );
     Groups = ReadStringFromFile( ConfigDir + "/groups" );
     ServerURL = ReadStringFromFile( ConfigDir + "/defaultserver" );
     Project = ReadStringFromFile( ConfigDir + "/defaultproject" );
     if( !ReadStringFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
     if( !ReadStringFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
     if( !ReadStringFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";
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
  } else if( !strcmp( argv[ i ], "-a" ) ) {
    if( argv[ ++i ] )
     Application = string( argv[ i ] );
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else if( !strcmp( argv[ i ], "-s" ) ) {
    if( argv[ ++i ] )
     State = string( argv[ i ] );
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

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // remove newlines from possible file reading...
 while( User[ User.size() - 1 ] == '\n' ) User = User.substr( 0, User.size() - 1 );
 while( Groups[ Groups.size() - 1 ] == '\n' ) Groups = Groups.substr( 0, Groups.size() - 1 );
 while( Project[ Project.size() - 1 ] == '\n' ) Project = Project.substr( 0, Project.size() - 1 );
 while( ServerURL[ ServerURL.size() - 1 ] == '\n' ) ServerURL = ServerURL.substr( 0, ServerURL.size() - 1 );

 // now start qstating the server...
 Interface_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile );

 if( ListServers )            // should we list servers...
 {

  if( ( Flag = ServerAPI.Interface_Project_Server_List( Response, ServerURL, Project, User, Groups ) ) != CURLE_OK )
  {
   cout << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl;
   return( 1 );
  }

  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

  if( OutputInXML )
  {
   cout << Response << endl;                // just dump the XML if requested...

   if( !Parse_XML( Response, "error" ).empty() ) return( 1 );
  }
  else
  {
   if( !Parse_XML( Response, "error" ).empty() ) 
   {
    cout << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl;
    return( 1 );
   }

   cout << "--- Project server list requested ---" << endl << endl;
   cout << "This project server   :         " << NormalizeString( ServerURL ) << endl << endl;
   cout << "Project master server :         " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << endl << endl;
   
   int Count = atoi( NormalizeString( Parse_XML( Response, "number_of_slave_servers" ) ).c_str() );
   int StartPos = 0;
   string Attributes;
 
   for( int i = 1; i <= Count; ++i )
    cout << "Project slave server :         " << NormalizeString( Parse_XML( Response, "project_server", Attributes, StartPos ) ) << endl;
   if( Count ) cout << endl;

   cout << "--- End of project server list ---" << endl << endl;
  }
 }

 if( Job_Id.empty() )      // did we ask for details...
 {
  if( OutputInXML )
  {
  }
  else
  {
  }
 }
 else                    // when details were requested...
 {
  if( OutputInXML )
  {
  }
  else
  {
  }
 }

 return( 0 );
}
