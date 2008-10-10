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

#include <iostream>
#include <iomanip>

#include "logger.h"
#include "interface_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"

// ----------------------------------------------------------------------

string KeyFile, CertificateFile, CACertificateFile, ServerURL, Response,
       Project, State, Application, User, Groups, Job_Id, ConfigDir, BinData;
int OutputInXML = 0;
int ListServers = 0;

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
 cout << endl << ExeName << " [options] [job_id]" << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                         show this help." << endl;
 cout << "-a application             specify application to query about. default is any." << endl;
 cout << "-s state                   specify job state to query about. default is any." << endl;
 cout << "-x                         output in XML format." << endl;
 cout << "-l                         report project server list." << endl;
 cout << "-L                         report project resource list." << endl;
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
  } else if( !strcmp( argv[ i ], "-x" ) ) {
     OutputInXML = 1;
  } else if( !strcmp( argv[ i ], "-l" ) ) {
     ListServers |= 1;
  } else if( !strcmp( argv[ i ], "-L" ) ) {
     ListServers |= 2;
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

 if( KeyFile.empty() ) Flag = 1;                  // check combo's of all options...
 if( CertificateFile.empty() ) Flag = 1;
 if( CACertificateFile.empty() ) Flag = 1;
 if( ServerURL.empty() ) Flag = 1;
 if( User.empty() ) Flag = 1;
 if( Groups.empty() ) Flag = 1;
 if( !Job_Id.empty() && ListServers ) Flag = 1;
 if( !Application.empty() && ListServers ) Flag = 1;
 if( !State.empty() && ListServers ) Flag = 1;
 if( !Job_Id.empty() && !Application.empty() ) Flag = 1;
 if( !Job_Id.empty() && !State.empty() ) Flag = 1;

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // now start qstating the server...
 Interface_Server_API ServerAPI( KeyFile, CertificateFile, CACertificateFile );

 if( ListServers )            // should we list servers...
 {

  if( ListServers & 1 )
  {
   if( ( Flag = ServerAPI.Interface_Project_Server_List( Response, ServerURL, Project, User, Groups ) ) != CURLE_OK )
   {
    cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
    return( 1 );
   }

   Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

   if( Response.empty() ) return( 1 );

   if( OutputInXML )
   {
    cout << Response << endl;                // just dump the XML if requested...

    if( !Parse_XML( Response, "error" ).empty() ) return( 1 );
   }
   else
   {
    if( !Parse_XML( Response, "error" ).empty() ) 
    {
     cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
     return( 1 );
    }

    cout << endl << "This project          : " << NormalizeString( Parse_XML( Response, "project" ) ) << endl;
    cout << "This project server   : " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
    cout << "Project master server : " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << endl;
   
    int Count = atoi( NormalizeString( Parse_XML( Response, "number_of_slave_servers" ) ).c_str() );
    int StartPos = 0;
    string Attributes;
 
    for( int i = 1; i <= Count; ++i )
     cout << "Project slave server  : " << NormalizeString( Parse_XML( Response, "project_server", Attributes, StartPos ) ) << endl;

    if( !(ListServers & 2) ) cout << endl;
   }
  }

  if( ListServers & 2 )
  {
   if( ( Flag = ServerAPI.Interface_Project_Resource_List( Response, ServerURL, Project, User, Groups ) ) != CURLE_OK )
   {
    cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
    return( 1 );
   }

   Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

   if( Response.empty() ) return( 1 );

   if( OutputInXML )
   {
    cout << Response << endl;                // just dump the XML if requested...

    if( !Parse_XML( Response, "error" ).empty() ) return( 1 );
   }
   else
   {
    if( !Parse_XML( Response, "error" ).empty() )
    {
     cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
     return( 1 );
    }

    if( !( ListServers & 1 ) )
    {
     cout << endl << "This project          : " << NormalizeString( Parse_XML( Response, "project" ) ) << endl;
     cout << "This project server   : " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
     cout << "Project master server : " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << endl;
    }

    int Count = atoi( NormalizeString( Parse_XML( Response, "number_of_resources" ) ).c_str() );
    int StartPos = 0;
    string Attributes;

    for( int i = 1; i <= Count; ++i )
    {
     string Resource = Parse_XML( Response, "resource", Attributes, StartPos );
     time_t TimeStamp = atoi( NormalizeString( Parse_XML( Resource, "last_call_time" ) ).c_str() );
     char *TimeStampStr = ctime( &TimeStamp );
     TimeStampStr[ 24 ] = '\0';

     cout << endl << "Resource name         : " << NormalizeString( Parse_XML( Resource, "resource_name" ) ) << endl;
     cout << "Resource capabilities : " << NormalizeString( Parse_XML( Resource, "resource_capabilities" ) ) << endl;
     cout << "Resource time stamp   : " <<  TimeStampStr << " [" << TimeStamp << "]" << endl;
    }

    cout << endl;
   }
  }

  return( 0 );
 }



 if( Job_Id.empty() )      // did we ask for general job list...
 {
  int TotalNrOfJobs = 0;
  int NrOfJobs = 0;
  int Limit = 250;
  int Offset = 0;
  char OffsetStr[ 64 ];
  char LimitStr[ 64 ];

  if( !OutputInXML )                       // output header of list...
  {
   cout << endl << "-----------------------------------------------------------------------------------------------------------------------------------" << endl;
   cout << " " << setw( 4 ) << "#" << " | " << setw( 6 ) << "job_id" << " | ";
   cout << setw( 12 ) << "state" << " | " << setw( 36 ) << "target_resources" << " | ";
   cout << setw( 16 ) << "application" << " | " << setw( 24 ) << "time_stamp" << " | ";
   cout << setw( 12 ) << "owners" << endl;
   cout << "-----------------------------------------------------------------------------------------------------------------------------------" << endl;
  }

  do
  {
   sprintf( OffsetStr, "%d", Offset );
   sprintf( LimitStr, "%d", Limit );

   if( ( Flag = ServerAPI.Interface_Job_State( Response, ServerURL, Project, User, Groups, Job_Id, State, Application, OffsetStr, LimitStr ) ) != CURLE_OK )
   {
    cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
    return( 1 );
   }

   Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

   if( Response.empty() ) return( 1 );

   if( !Parse_XML( Response, "error" ).empty() )
   {
    if( OutputInXML )
     cout << Response << endl;
    else
     cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
    return( 1 );
   }

   NrOfJobs = atoi( NormalizeString( Parse_XML( Response, "number_of_jobs" ) ).c_str() );

   if( NrOfJobs )
   {
    string Attribute, Job;
    int StartPos = 0;

    for( int i = 1; i <= NrOfJobs; ++i ) 
    {
     Job = Parse_XML( Response, "job", Attribute, StartPos );

     if( OutputInXML )                                        // output only the jobs tags and adjust the numbering...
      cout << "<job number='" << ( i + Offset ) << "'> " << Job << " </job> ";
     else
     {
      time_t TimeStamp = atoi( NormalizeString( Parse_XML( Job, "state_time_stamp" ) ).c_str() );
      char *TimeStampStr = ctime( &TimeStamp );
      TimeStampStr[ 24 ] = '\0';
 
      cout << " " << setw( 4 ) << ( i + Offset ) << " | ";
      cout << setw( 6 ) << NormalizeString( Parse_XML( Job, "job_id" ) ) << " | ";
      cout << setw( 12 ) << NormalizeString( Parse_XML( Job, "state" ) ) << " | "; 
      cout << setw( 36 ) << NormalizeString( Parse_XML( Job, "target_resources" ) ) << " | ";
      cout << setw( 16 ) << NormalizeString( Parse_XML( Job, "application" ) ) << " | ";
      cout << setw( 24 ) << TimeStampStr << " | ";
      cout << setw( 12 ) << NormalizeString( Parse_XML( Job, "owners" ) ) << endl;
     }

    }
   }
   else
   {
    if( OutputInXML )
    {
     // output the other fields of the response now...

     cout << "<project> " << NormalizeString( Parse_XML( Response, "project" ) ) << " </project> ";
     cout << "<project_master_server> " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << " </project_master_server> ";
     cout << "<this_project_server> " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << " </this_project_server> ";
     cout << "<user> " << NormalizeString( Parse_XML( Response, "user" ) ) << " </user> ";
     cout << "<groups> " << NormalizeString( Parse_XML( Response, "groups" ) ) << " </groups> ";
     cout << "<state> " << NormalizeString( Parse_XML( Response, "state" ) ) << " </state> ";
     cout << "<application> " << NormalizeString( Parse_XML( Response, "application" ) ) << " </application> ";
     cout << "<number_of_jobs> " << TotalNrOfJobs << " </number_of_jobs> " << endl;

    }
    else
    {
     cout << "-----------------------------------------------------------------------------------------------------------------------------------" << endl << endl;
     cout << "Number of jobs listed : " << TotalNrOfJobs << endl;
     cout << "This project          : " << NormalizeString( Parse_XML( Response, "project" ) ) << endl;
     cout << "This project server   : " << NormalizeString( Parse_XML( Response, "this_project_server" ) ) << endl;
     cout << "Project master server : " << NormalizeString( Parse_XML( Response, "project_master_server" ) ) << endl << endl;
    }
   }

   TotalNrOfJobs += NrOfJobs;
   Offset += Limit;
  } while( NrOfJobs );

 }
 else                    // when details were requested...
 {

  // request details of specific job...
  if( ( Flag = ServerAPI.Interface_Job_State( Response, ServerURL, Project, User, Groups, Job_Id, "", "", "", "" ) ) != CURLE_OK )
  {
   cout << endl << "Error posting to server " << ServerURL << ". The cURL return code was " << Flag << endl << endl;
   return( 1 );
  }
  
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

  if( Response.empty() ) return( 1 );

  if( OutputInXML )
  {
   cout << Response << endl;

   if( !Parse_XML( Response, "error" ).empty() ) return( 1 );
  }
  else
  {
   if( !Parse_XML( Response, "error" ).empty() ) 
   {
    cout << endl << "Error message returned by server " << ServerURL << " : " << NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "message" ) ) << endl << endl;
    return( 1 );
   }

   // show the details of the job...

   time_t TimeStamp = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ).c_str() );
   char *TimeStampStr = ctime( &TimeStamp );
   TimeStampStr[ 24 ] = '\0';

   cout << endl << "This project          : " << NormalizeString( Parse_XML( Response, "project" ) ) << endl;
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
   cout << "Time stamp            : " << TimeStampStr << " [" << TimeStamp << "]" << endl;

   HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), BinData );
   cout << "Input                 : " << BinData << endl;
   HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "output" ) ), BinData );
   cout << "Output                : " << BinData << endl << endl;

   // here we output the repository content too...


  }
 }

 return( 0 );
}
