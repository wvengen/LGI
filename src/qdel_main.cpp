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
#include <cstring>
#include <cstdlib>
#include <vector>

#include <curl/curl.h>

#include "logger.h"
#include "user_configclass.h"
#include "interface_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"

// ----------------------------------------------------------------------

string KeyFile, CertificateFile, CACertificateFile, ServerURL, Response,
       Project, State, Application, User, Groups, ConfigDir;

vector<string> Job_Ids;

bool Strict = true;

// ----------------------------------------------------------------------

int IsNumber( char *String )
{
 for( int i = 0; String[ i ]; ++i )
  if( !isdigit( String[ i ] ) ) return( 0 );
 return( 1 );
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
 cout << "-W                         be less strict in hostname checks of project server certificates." << endl;
 cout << "-K keyfile                 specify key file." << endl;
 cout << "-C certificatefile         specify certificate file." << endl;
 cout << "-CA cacertificatefile      specify ca certificate file." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 UserConfig Config;

 // check number of arguments first...
 if( argc < 2 )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // turn logging facilities off...
 InitializeLogger( 0 );

 // setup default values from default configuration directory...
 User = Config.User();
 Groups = Config.Groups();
 ServerURL = Config.DefaultServer();
 Project = Config.DefaultProject();
 CACertificateFile = Config.CA_ChainFile();
 CertificateFile = Config.CertificateFile();
 KeyFile = Config.PrivateKeyFile();

 // read passed arguments here...
 for( int i = 1; i < argc; ++i )
 {
  if( !strcmp( argv[ i ], "-h" ) ) {
    PrintHelp( argv[ 0 ] );
    return( 0 );
  } else if( !strcmp( argv[ i ], "-W" ) ) {
    Strict = false;
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] )
    {
     Config.Read( argv [ i ] );
     if( !Config.User().empty() ) User = Config.User();
     if( !Config.Groups().empty() ) Groups = Config.Groups();
     if( !Config.DefaultServer().empty() ) ServerURL = Config.DefaultServer();
     if( !Config.DefaultProject().empty() ) Project = Config.DefaultProject();
     if( !Config.CA_ChainFile().empty() ) CACertificateFile = Config.CA_ChainFile();
     if( !Config.CertificateFile().empty() ) CertificateFile = Config.CertificateFile();
     if( !Config.PrivateKeyFile().empty() ) KeyFile = Config.PrivateKeyFile();
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
    if( IsNumber( argv[ i ] ) )
     Job_Ids.push_back( string( argv[ i ] ) );
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
 if( Job_Ids.size() <= 0 ) Flag = 1;

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 curl_global_init( CURL_GLOBAL_ALL );

 // now start contacting the server...
 Interface_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile, NULL, Strict );


 for( int j = 0; j < Job_Ids.size(); ++j )
 {
 
  // try and delete job from server...
  if( ( Flag = ServerAPI.Interface_Delete_Job( Response, ServerURL, Project, User, Groups, Job_Ids[ j ] ) ) != CURLE_OK )
  {
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
   return( 1 );
  }

  // now show response obtained...
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

  if( Response.empty() ) continue;

  if( !Parse_XML( Response, "error" ).empty() )
  {
   cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) )<< endl;
   continue;
  }

  // check if deleted okay ....
  if( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ) == "deleted" )
  {
   cout << endl << "Job " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_id" ) ) << " deleted from project ";
   cout << NormalizeString( Parse_XML( Response, "project" ) ) << " on server " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
   continue;
  }

  if( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ) == "aborting" )
  {
   cout << endl << "Aborting job " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_id" ) ) << " in project ";
   cout << NormalizeString( Parse_XML( Response, "project" ) ) << " on server " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
   continue;
  }

  cout << endl << "Could not abort or delete job " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_id" ) ) << " from project ";
  cout << NormalizeString( Parse_XML( Response, "project" ) ) << " on server " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
 }

 cout << endl;

 return( 0 );
}
