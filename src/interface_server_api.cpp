/* []--------------------------------------------------------[]
    |               interface_server_api.cpp                 |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 10 July 2007.                        |
    | USE:        Implements the interface API to the        |
    |             project server using cURL...               |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "interface_server_api.h"

// ------------------------------------------------------------------------------

Interface_Server_API::Interface_Server_API( string KeyFile, string CertificateFile, string CAFile, CURL *cURLHandle )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Server_API; KeyFile=" << KeyFile << ", CertificateFile=" << CertificateFile << ", CAFile=" << CAFile );

 PrivateKeyFile = KeyFile;
 PublicCertificateFile = CertificateFile;
 CAChainFile = CAFile;
 MycURLHandle = cURLHandle;
 CreatedMycURLHandle = 0;
}

// ------------------------------------------------------------------------------

Interface_Server_API::~Interface_Server_API( void )
{
 if( CreatedMycURLHandle && MycURLHandle != NULL ) curl_easy_cleanup( MycURLHandle );
}

// ------------------------------------------------------------------------------

CURL *Interface_Server_API::SetupcURLForPost( string &PostURL )
{
 if( MycURLHandle == NULL )
 {
  MycURLHandle = curl_easy_init();
  CreatedMycURLHandle = 1;
 }

 if( MycURLHandle != NULL )
 {
  curl_easy_reset( MycURLHandle );
  curl_easy_setopt( MycURLHandle, CURLOPT_URL, PostURL.c_str() );
  curl_easy_setopt( MycURLHandle, CURLOPT_SSLCERT, PublicCertificateFile.c_str() );
  curl_easy_setopt( MycURLHandle, CURLOPT_SSLKEY, PrivateKeyFile.c_str() );
  curl_easy_setopt( MycURLHandle, CURLOPT_CAINFO, CAChainFile.c_str() );
  curl_easy_setopt( MycURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( MycURLHandle, CURLOPT_SSL_VERIFYHOST, 0 );
  curl_easy_setopt( MycURLHandle, CURLOPT_NOSIGNAL, 1 );
  curl_easy_setopt( MycURLHandle, CURLOPT_VERBOSE, 0 );
  curl_easy_setopt( MycURLHandle, CURLOPT_NOPROGRESS, 1 );
  curl_easy_setopt( MycURLHandle, CURLOPT_ERRORBUFFER, CURLErrorBuffer );
  DEBUG_LOG_RETURN( MycURLHandle, "Interface_Server_API::SetupcURLForPost; Obtained cURL handle for PostURL=" << PostURL );
 }

 CRITICAL_LOG_RETURN( MycURLHandle, "Interface_Server_API::SetupcURLForPost; Failed to obtain a cURL handle for PostURL=" << PostURL );
}

// ------------------------------------------------------------------------------

size_t Interface_Server_API::WriteToStringCallBack( void *ptr, size_t size, size_t nmemb, void *stream )
{
 string *PointerToString = (string *)( stream );

 (*PointerToString).append( (char *)( ptr ), size * nmemb );

 return( size * nmemb );
}

// ------------------------------------------------------------------------------

CURLcode Interface_Server_API::PerformcURLPost( string &Response, CURL *cURLHandle, curl_httppost *PostList )
{
 if( cURLHandle != NULL )
 {
  Response.reserve( 8192 );    
  Response.clear();

  curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, &Response );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEFUNCTION, WriteToStringCallBack );
  if( PostList != NULL ) curl_easy_setopt( cURLHandle, CURLOPT_HTTPPOST, PostList );

  CURLcode cURLResult = curl_easy_perform( cURLHandle );

  if( PostList != NULL ) curl_formfree( PostList );

  if( cURLResult != CURLE_OK )
   CRITICAL_LOG_RETURN( cURLResult, "Interface_Server_API::PerformcURLPost; Could not perform post, returned " << cURLResult << " with error buffer: " << CURLErrorBuffer )
  else
  {
   DEBUG_LOG( "Interface_Server_API::PerformcURLPost; Performed post" ); 
   VERBOSE_DEBUG_LOG( "Interface_Server_API::PerformcURLPost; Response=" << Response );
   return( cURLResult );
  }

 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::PerformcURLPost; Received an invalid cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Interface_Server_API::Interface_Project_Server_List( string &Response, string ServerURL, string Project, string User, string Groups )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Project_Server_List; ServerURL=" << ServerURL << ", Project=" << Project << ", User=" << User << ", Groups=" << Groups );

 string PostURL = ServerURL + "/interfaces/interface_project_server_list.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "user", CURLFORM_PTRCONTENTS, User.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "groups", CURLFORM_PTRCONTENTS, Groups.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Interface_Server_API::Interface_Project_Server_List; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::Interface_Project_Server_List; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Interface_Server_API::Interface_Project_Resource_List( string &Response, string ServerURL, string Project, string User, string Groups )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Project_Resource_List; ServerURL=" << ServerURL << ", Project=" << Project << ", User=" << User << ", Groups=" << Groups );

 string PostURL = ServerURL + "/interfaces/interface_project_resource_list.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "user", CURLFORM_PTRCONTENTS, User.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "groups", CURLFORM_PTRCONTENTS, Groups.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Interface_Server_API::Interface_Project_Resource_List; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::Interface_Project_Resource_List; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Interface_Server_API::Interface_Job_State( string &Response, string ServerURL, string Project, string User, string Groups, string Job_Id, string State, string Application, string Start, string Limit )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Job_State; ServerURL=" << ServerURL << ", Project=" << Project << ", User=" << User << ", Groups=" << Groups  << ", Job_Id=" << Job_Id << ", State=" << State << ", Application=" << Application << ", Start=" << Start << ", Limit= " << Limit );

 string PostURL = ServerURL + "/interfaces/interface_job_state.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "user", CURLFORM_PTRCONTENTS, User.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "groups", CURLFORM_PTRCONTENTS, Groups.c_str(), CURLFORM_END );
 
  if( !Job_Id.empty() )
   curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );
  else
  {
   if( !State.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "state", CURLFORM_PTRCONTENTS, State.c_str(), CURLFORM_END );
   if( !Application.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "application", CURLFORM_PTRCONTENTS, Application.c_str(), CURLFORM_END );
   if( !Start.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "start", CURLFORM_PTRCONTENTS, Start.c_str(), CURLFORM_END );
   if( !Limit.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "limit", CURLFORM_PTRCONTENTS, Limit.c_str(), CURLFORM_END );
  }

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Interface_Server_API::Interface_Job_State; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::Interface_Job_State; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Interface_Server_API::Interface_Delete_Job( string &Response, string ServerURL, string Project, string User, string Groups, string Job_Id )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Delete_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", User=" << User << ", Groups=" << Groups  << ", Job_Id=" << Job_Id );

 string PostURL = ServerURL + "/interfaces/interface_delete_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "user", CURLFORM_PTRCONTENTS, User.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "groups", CURLFORM_PTRCONTENTS, Groups.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Interface_Server_API::Interface_Delete_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::Interface_Delete_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Interface_Server_API::Interface_Submit_Job( string &Response, string ServerURL, string Project, string User, string Groups, string Application, string Target_Resources, string Job_Specifics, string Input, string Read_Access, string Write_Access, string Output, vector<string> FilesToUpload )
{
 DEBUG_LOG( "Interface_Server_API::Interface_Submit_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", User=" << User << ", Groups=" << Groups  << ", Application=" << Application << ", Target_Resources=" << Target_Resources << ", Job_Specifics=" << Job_Specifics << ", Input=" << Input << ", Read_Access= " << Read_Access << ", Write_Access=" << Write_Access << ", Output=" << Output );

 string PostURL = ServerURL + "/interfaces/interface_submit_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "user", CURLFORM_PTRCONTENTS, User.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "groups", CURLFORM_PTRCONTENTS, Groups.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "application", CURLFORM_PTRCONTENTS, Application.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "target_resources", CURLFORM_PTRCONTENTS, Target_Resources.c_str(), CURLFORM_END );

  if( !Job_Specifics.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_specifics", CURLFORM_PTRCONTENTS, Job_Specifics.c_str(), CURLFORM_END );
  if( !Input.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "input", CURLFORM_PTRCONTENTS, Input.c_str(), CURLFORM_END );
  if( !Read_Access.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "read_access", CURLFORM_PTRCONTENTS, Read_Access.c_str(), CURLFORM_END );
  if( !Write_Access.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "write_access", CURLFORM_PTRCONTENTS, Write_Access.c_str(), CURLFORM_END );
  if( !Output.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "output", CURLFORM_PTRCONTENTS, Output.c_str(), CURLFORM_END );

  if( FilesToUpload.size() )          // there are files to be uploaded too...
  {
   char Tag[ 128 ];
   int NrOfFiles = 0;
   
   for( int i = 0; i < FilesToUpload.size(); ++i )
    if( !FilesToUpload[ i ].empty() )
    {
     ++NrOfFiles;
     sprintf( Tag, "uploaded_file_%d", NrOfFiles );
     curl_formadd( &PostList, &LastItem, CURLFORM_COPYNAME, Tag, CURLFORM_FILE, FilesToUpload[ i ].c_str(), CURLFORM_END );
    }

   if( NrOfFiles >= 1 )
   {
    sprintf( Tag, "%d", NrOfFiles );
    curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "number_of_uploaded_files", CURLFORM_PTRCONTENTS, Tag, CURLFORM_END );
   }
  }

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Interface_Server_API::Interface_Submit_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Interface_Server_API::Interface_Submit_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------
