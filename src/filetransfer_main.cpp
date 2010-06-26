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
#include <curl/curl.h>

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>

#include "logger.h"
#include "resource_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "daemon_jobclass.h"

// ----------------------------------------------------------------------

#define CMD_LIST       1
#define CMD_DOWNLOAD   2
#define CMD_UPLOAD     4
#define CMD_DELETE     8

// ----------------------------------------------------------------------

int Command = 0, OutputXML = 0;

string KeyFile, CertificateFile, CACertificateFile, RepositoryURL,
       RepositoryServer, RepositoryDir;

vector<string> FileList;

// ----------------------------------------------------------------------

size_t WriteToStringCallBack( void *ptr, size_t size, size_t nmemb, void *stream )
{
 string *PointerToString = (string *)( stream );

 (*PointerToString).append( (char *)( ptr ), size * nmemb );

 return( size * nmemb );
}

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
 cout << endl << ExeName << " [options] command repository-url [files]" << endl << endl;
 cout << "commands:" << endl << endl;
 cout << "list                           list files present in specified repository." << endl;
 cout << "download                       download files from repository." << endl;
 cout << "upload                         upload files to repository." << endl;
 cout << "delete                         delete files from repository." << endl << endl;
 cout << "options:" << endl << endl;
 cout << "-h                             show this help." << endl;
 cout << "-x                             output lists in XML format." << endl;
 cout << "-c directory                   specify the configuration directory to read. default is ~/.LGI. specify options below to overrule." << endl;
 cout << "-j jobdirectory                specify job directory to use. if not specified try current directory or specify the following options." << endl;
 cout << "-K keyfile                     specify key file." << endl;
 cout << "-C certificatefile             specify certificate file." << endl;
 cout << "-CA cacertificatefile          specify ca certificate file." << endl << endl;
}

// ----------------------------------------------------------------------

int ListRepository( void )
{
 string Response, File, FileData, URL = RepositoryServer + "/repository_content.php?repository=" + RepositoryDir;
 int Pos, FileNr;
 time_t TimeStamp;
 char *TimeStampStr;

 CURL *cURLHandle = curl_easy_init();

 if( cURLHandle != NULL )
 {
  curl_easy_setopt( cURLHandle, CURLOPT_URL, URL.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLCERT, CertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLKEY, KeyFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_CAINFO, CACertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYHOST, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_NOSIGNAL, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, &Response );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEFUNCTION, WriteToStringCallBack );
  CURLcode cURLResult = curl_easy_perform( cURLHandle );
  curl_easy_cleanup( cURLHandle );

  if( cURLResult != CURLE_OK )
  {
   cout << endl << "Error value returned from cURL request: " << cURLResult << endl << endl;
   return( cURLResult );
  }

  if( OutputXML ) cout << Response << endl;  

  Response = NormalizeString( Parse_XML( Response, "repository_content" ) );

  if( !Response.empty() )
  {
   FileNr = 0;
   FileData = Parse_XML( Response, "file", File, Pos = 0 );

   if( !OutputXML ) cout << endl;

   do {
    FileNr++;

    if( !OutputXML )
    {
     TimeStamp = atoi( NormalizeString( Parse_XML( FileData, "date" ) ).c_str() );
     TimeStampStr = ctime( &TimeStamp );
     TimeStampStr[ 24 ] = '\0';

     cout << left << setw( 32 ) << File.substr( 6, File.length() - 7 ) << " " << right << 
             setw( 8 ) << NormalizeString( Parse_XML( FileData, "size" ) ) << " " <<
             TimeStampStr << " [" << TimeStamp << "]" << endl;
    }
    else
     cout << " <file number=\"" << FileNr << "\"> " << FileData << " <file_name> " << 
             File.substr( 6, File.length() - 7 ) << " </file_name> </file>";


    FileData = Parse_XML( Response, "file", File, Pos );

   } while( !FileData.empty() );

   if( !OutputXML ) 
    cout << endl;
   else
    cout << " <number_of_files> " << FileNr << " </number_of_files>" << endl;
  }
  else 
   if( OutputXML ) cout << " <number_of_files> 0 </number_of_files>" << endl;

 }
 else
  return( 1 );

 return( 0 );
}

// ----------------------------------------------------------------------

int DownLoadFilesFromRepository( void )
{
 int Flag = 0;

 CURL *cURLHandle = curl_easy_init();

 if( cURLHandle != NULL )
 {
  curl_easy_setopt( cURLHandle, CURLOPT_SSLCERT, CertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLKEY, KeyFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_CAINFO, CACertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYHOST, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_NOSIGNAL, 1 );

  for( int i = 0; i < FileList.size(); ++i )
   if( !FileList[ i ].empty() )
   {
    FILE *TheFile = fopen( FileList[ i ].c_str(), "wb" );

    if( TheFile == NULL )
    {
     cout << endl << "Error opening file '" <<  FileList[ i ] << "' ..." << endl;
     Flag = 1;
     continue;
    }

    string URL = RepositoryURL + "/" + FileList[ i ];

    curl_easy_setopt( cURLHandle, CURLOPT_URL, URL.c_str() );
    curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, TheFile );

    CURLcode cURLResult = curl_easy_perform( cURLHandle );

    if( cURLResult != CURLE_OK )
    {
     cout << endl << "Error downloading from '" << URL << "' ..." << endl;
     Flag = 1;
    }
    else
     cout << endl << "Downloaded from '" << URL << "' ..." << endl;

    fflush( TheFile );
    fclose( TheFile );
   }

  curl_easy_cleanup( cURLHandle );

  cout << endl;
 }
 else
  return( 1 );

 return( Flag );
}

// ----------------------------------------------------------------------

int UpLoadFilesToRepository( void )
{
 string Response;
 int Flag = 1;

 CURL *cURLHandle = curl_easy_init();

 if( cURLHandle != NULL )
 {
  curl_easy_setopt( cURLHandle, CURLOPT_SSLCERT, CertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLKEY, KeyFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_CAINFO, CACertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYHOST, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_NOSIGNAL, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_UPLOAD, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, &Response );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEFUNCTION, WriteToStringCallBack );

  for( int i = 0; i < FileList.size(); ++i )
   if( !FileList[ i ].empty() )
   {
    FILE *TheFile = fopen( FileList[ i ].c_str(), "rb" );

    if( TheFile == NULL )
    {
     cout << endl << "Error opening file '" << FileList[ i ] << "' ..." << endl;
     Flag = 1;
     continue;
    }

    fseek( TheFile, 0L, SEEK_END );

    string URL = RepositoryURL + "/" + basename( FileList[ i ].c_str() );

    unsigned long FileSize = ftell( TheFile );

    fseek( TheFile, 0L, SEEK_SET );

    curl_easy_setopt( cURLHandle, CURLOPT_URL, URL.c_str() );
    curl_easy_setopt( cURLHandle, CURLOPT_READDATA, TheFile );
    curl_easy_setopt( cURLHandle, CURLOPT_INFILESIZE, FileSize );

    Response.clear();

    CURLcode cURLResult = curl_easy_perform( cURLHandle );

    int Status = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "status" ), "number" ) ).c_str() );

    if( ( cURLResult != CURLE_OK ) || ( Status != 200 ) )
    {
     cout << endl << "Error uploading to '" << URL << "' ..." << endl;
     Flag = 1;
    }
    else
     cout << endl << "Uploaded to '" << URL << "' ..." << endl;

    fclose( TheFile );
   }

  curl_easy_cleanup( cURLHandle );

  cout << endl;
 } 
 else
  return( 1 );
 
 return( Flag ); 
}

// ----------------------------------------------------------------------

int DeleteFilesFromRepository( void )
{
 string Response;
 int Flag = 0;

 CURL *cURLHandle = curl_easy_init();

 if( cURLHandle != NULL )
 {
  curl_easy_setopt( cURLHandle, CURLOPT_SSLCERT, CertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLKEY, KeyFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_CAINFO, CACertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYHOST, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_NOSIGNAL, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_CUSTOMREQUEST, "DELETE" );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, &Response );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEFUNCTION, WriteToStringCallBack );

  for( int i = 0; i < FileList.size(); ++i )
   if( !FileList[ i ].empty() )
   {
    string URL = RepositoryURL + "/" + FileList[ i ];

    curl_easy_setopt( cURLHandle, CURLOPT_URL, URL.c_str() );

    Response.clear();

    CURLcode cURLResult = curl_easy_perform( cURLHandle );

    int Status = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "status" ), "number" ) ).c_str() );

    if( ( cURLResult != CURLE_OK ) || ( Status != 200 ) ) 
    {
     cout << endl << "Error deleting '" << URL << "' ..." << endl;
     Flag = 1;
    }
    else
     cout << endl << "Deleted '" << URL << "' ..." << endl;
   }

  curl_easy_cleanup( cURLHandle );

  cout << endl;
 }
 else
  return( 1 );

 return( Flag );
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
 }

 // if that didn't work, try and read default config from ~/.LGI...
 if( KeyFile.empty() | CertificateFile.empty() | CACertificateFile.empty() )
 {
  string ConfigDir = string( getenv( "HOME" ) ) + "/.LGI";

  if( !ReadLineFromFile( ConfigDir + "/privatekey" ).empty() ) KeyFile = ConfigDir + "/privatekey";
  if( !ReadLineFromFile( ConfigDir + "/certificate" ).empty() ) CertificateFile = ConfigDir + "/certificate";
  if( !ReadLineFromFile( ConfigDir + "/ca_chain" ).empty() ) CACertificateFile = ConfigDir + "/ca_chain";
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
  } else if( !strcmp( argv[ i ], "-x" ) ) {
    OutputXML = 1;
  } else if( !strcmp( argv[ i ], "list" ) ) {
    if( Command )
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    Command = CMD_LIST;
  } else if( !strcmp( argv[ i ], "download" ) ) {
    if( Command )
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    Command = CMD_DOWNLOAD;
  } else if( !strcmp( argv[ i ], "upload" ) ) {
    if( Command )
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    Command = CMD_UPLOAD;
  } else if( !strcmp( argv[ i ], "delete" ) ) {
    if( Command )
    {
     PrintHelp( argv[ 0 ] );
     return( 1 );
    }
    Command = CMD_DELETE;
  } else {
    if( RepositoryURL.empty() )
     RepositoryURL = string( argv[ i ] );
    else
     FileList.push_back( string( argv[ i ] ) ); 
  };
 }

 int DirPos = 0, Flag = 0;

 if( KeyFile.empty() ) Flag = 1;
 if( CertificateFile.empty() ) Flag = 1;
 if( CACertificateFile.empty() ) Flag = 1;
 if( RepositoryURL.empty() ) Flag = 1;
 if( !Command ) Flag = 1;
 if( ( Command == CMD_LIST ) && FileList.size() ) Flag = 1;
 if( ( Command != CMD_LIST ) && OutputXML ) Flag = 1;
 if( ( Command != CMD_LIST ) && !FileList.size() ) Flag = 1;

 if( RepositoryURL.find( "https://" ) != 0 ) 
  Flag = 1;
 else
 {
  DirPos = RepositoryURL.rfind( "/" );

  if( DirPos <= 9 )
   Flag = 1;
  else
  {
   RepositoryServer = RepositoryURL.substr( 0, DirPos );
   RepositoryDir = RepositoryURL.substr( DirPos + 1, RepositoryURL.length() - DirPos - 1);
  }

  if( RepositoryServer.empty() ) Flag = 1;
  if( RepositoryDir.empty() ) Flag = 1;
 }

 if( Flag )
 {
  PrintHelp( argv[ 0 ] );
  return( 1 );
 }

 // Now check issued command...
 switch( Command )
 {
  case CMD_LIST:     return( ListRepository() );
  case CMD_DOWNLOAD: return( DownLoadFilesFromRepository() );
  case CMD_UPLOAD:   return( UpLoadFilesToRepository() );
  case CMD_DELETE:   return( DeleteFilesFromRepository() );
 }

 return( 1 );
}
