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
#include <cstring>

#include <curl/curl.h>

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

int XMLOutput = 0,
    ResourceMode = 0;                       // default to user mode...

string KeyFile, CertificateFile, CACertificateFile, 
       ServerURL, Project, Response, ConfigDir,
       Application, Target_Resources( "any" ), 
       Job_Specifics, Owners, Read_Access, Input, 
       Write_Access, Output, User, Groups;

bool Strict = true;

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
 cout << endl << ExeName << " -a application [options] [files to upload to repository]" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                      show this help." << endl;
 cout << "-x                      output in XML format." << endl;
 cout << "-a application          specify the application." << endl;
 cout << "-t targetresources      specify the target resources." << endl;
 cout << "-s jobspecs             specify the job specifics." << endl;
 cout << "-i inputfile            specify the job input file." << endl;
 cout << "-r readaccesslist       specify the job readaccess list." << endl;
 cout << "-w writeaccesslist      specify the job writeaccess list." << endl;
 cout << "-c directory            specify the configuration directory to read. default is ~/.LGI. specify options below to overrule." << endl;
 cout << "-j jobdirectory         specify job directory to use. if not specified try current directory or specify the following options." << endl;
 cout << "-K keyfile              specify key file." << endl;
 cout << "-C certificatefile      specify certificate file." << endl;
 cout << "-CA cacertificatefile   specify ca certificate file." << endl;
 cout << "-S serverurl            specify project server url." << endl;
 cout << "-U user                 specify username." << endl;
 cout << "-G groups               specify groups." << endl;
 cout << "-P project              specify project name." << endl;
 cout << "-W                      be less strict in hostname checks of project server certificates." << endl;
 cout << "-m                      switch user or resource mode." << endl << endl;
}

// ----------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 DaemonJob Job;
 vector<string> FilesToUpload;

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
  Write_Access = Job.GetWriteAccess();

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
  } else if( !strcmp( argv[ i ], "-W" ) ) {
    Strict = false;
  } else if( !strcmp( argv[ i ], "-x" ) ) {
    XMLOutput = 1;
  } else if( !strcmp( argv[ i ], "-c" ) ) {
    if( argv[ ++i ] ) 
    {
     ConfigDir = string( argv[ i ] );

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
     Write_Access = Job.GetWriteAccess();

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
  } else if( !strcmp( argv[ i ], "-w" ) ) {
    if( argv[ ++i ] )
     Write_Access = string( argv[ i ] );
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
  } else if( !strcmp( argv[ i ], "-i" ) ) {
    if( argv[ ++i ] )
    {
     Input = string( argv[ i ] );
     Response = ReadStringFromFile( Input );
     BinHex( Response, Input );
    }
    else
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
  } else {
    if( !ReadStringFromFile( string( argv[ i ] ) ).empty() )
     FilesToUpload.push_back( string( argv[ i ] ) ); 
    else
    {
     cout << "Cannot read from file '" << argv[ i ] << "'." << endl;
     return( 1 );
    }
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

 curl_global_init( CURL_GLOBAL_ALL );

 // now act accordingly... first see if we are in user mode or not...
 if( ResourceMode )
 {
  Resource_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile, NULL, Strict );         

  // first try to sign up...
  Flag = ServerAPI.Resource_SignUp_Resource( Response, ServerURL, Project );
  if( Flag != CURLE_OK )
  {
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
   return( 1 );
  }

  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  if( Response.empty() ) return( 1 );
  Flag = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
  if( Flag ) 
  {
   cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
   return( 1 );
  }

  // we are signed on and have a session running now...
  string SessionID = NormalizeString( Parse_XML( Response, "session_id" ) );

  Flag = ServerAPI.Resource_Submit_Job( Response, ServerURL, Project, SessionID, Application, "queued", Owners, Target_Resources, Read_Access, Write_Access, Job_Specifics, Input, "", FilesToUpload );
  if( Flag != CURLE_OK )
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
  else
  {
   Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
   if( Response.empty() ) return( 1 );
   Flag = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
   if( Flag ) 
    cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
   if( XMLOutput ) cout << Response << endl;
  }

  // if we did submit the job, this signoff will unlock it automatically...
  Flag = ServerAPI.Resource_SignOff_Resource( Response, ServerURL, Project, SessionID );
  if( Flag != CURLE_OK )
  {
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
   return( 1 );
  }

  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  if( Response.empty() ) return( 1 );
  Flag = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
  if( Flag ) 
  {
   cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
   return( 1 );
  }

  return( 0 );           // all went well....
 }
 else    // when the submit is in user mode...
 {
  Interface_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile, NULL, Strict );         

  Flag = ServerAPI.Interface_Submit_Job( Response, ServerURL, Project, User, Groups, Application, Target_Resources, Job_Specifics, Input, Read_Access, Write_Access, "", FilesToUpload );
  if( Flag != CURLE_OK )
  {
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
   return( 1 );
  }

  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  if( Response.empty() ) return( 1 );
  Flag = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
  if( Flag )
  {
   cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
   return( 1 );
  }

  if( XMLOutput )
  {
   cout << Response << endl;
   return( 0 );
  }

  // dump job details we received back from response...
  time_t TimeStamp = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ).c_str() );
  char *TimeStampStr = ctime( &TimeStamp );
  TimeStampStr[ 24 ] = '\0';
 
  cout << endl << "Job has been submitted. Some details: " << endl << endl;
  cout << "This project          : " << NormalizeString( Parse_XML( Response, "project" ) ) << endl;
  cout << "This project server   : " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
  cout << "Project master server : " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << endl;
  cout << "User                  : " << NormalizeString( Parse_XML( Response, "user" ) ) << endl;
  cout << "Groups                : " << NormalizeString( Parse_XML( Response, "groups" ) ) << endl;

  cout << "Job id                : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_id" ) ) << endl;
  cout << "Job state             : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ) << endl;
  cout << "Job application       : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "application" ) ) << endl;
  cout << "Job specifics         : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_specifics" ) ) << endl;
  cout << "Target resources      : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "target_resources" ) ) << endl;
  cout << "Job owners            : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "owners" ) ) << endl;
  cout << "Read access on job    : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "read_access" ) ) << endl;
  cout << "Write access on job   : " << NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "write_access" ) ) << endl;
  cout << "Time stamp            : " << TimeStampStr << " [" << TimeStamp << "]" << endl;
  HexBin(  NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), Input );
  cout << "Input                 : " << Input << endl;

  // here we output the repository content too...
  Response = NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "repository_content" ) );
  if( !Response.empty() )
  {
   cout << "Repository content    : ";

   Output = Parse_XML( Response, "file", Input, Flag = 0 );
   do {
    TimeStamp = atoi( NormalizeString( Parse_XML( Output, "date" ) ).c_str() );
    TimeStampStr = ctime( &TimeStamp );
    TimeStampStr[ 24 ] = '\0';

    cout << Input.substr( 6, Input.length() - 7 ); 

    Output = Parse_XML( Response, "file", Input, Flag );

    if( !Output.empty() ) cout << ", ";
   } while( !Output.empty() );

   cout << endl;
  }

  cout << endl;

  return( 0 );
 }

 return( 1 );
}
