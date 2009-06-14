/* []--------------------------------------------------------[]
    |               resource_server_api.cpp                  |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 10 July 2007.                        |
    | USE:        Implements the resource daemon API to the  |
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

#include "resource_server_api.h"

// ------------------------------------------------------------------------------

Resource_Server_API::Resource_Server_API( string KeyFile, string CertificateFile, string CAFile )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Server_API; KeyFile=" << KeyFile << ", CertificateFile=" << CertificateFile << ", CAFile=" << CAFile );

 PrivateKeyFile = KeyFile;
 PublicCertificateFile = CertificateFile;
 CAChainFile = CAFile;
}

// ------------------------------------------------------------------------------

CURL *Resource_Server_API::SetupcURLForPost( string &PostURL )
{
 CURL *cURLHandle = curl_easy_init(); 

 if( cURLHandle != NULL )
 {
  curl_easy_setopt( cURLHandle, CURLOPT_URL, PostURL.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLCERT, PublicCertificateFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSLKEY, PrivateKeyFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_CAINFO, CAChainFile.c_str() );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYPEER, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_SSL_VERIFYHOST, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_NOSIGNAL, 1 );
  //  curl_easy_setopt( cURLHandle, CURLOPT_VERBOSE, 1 );
  curl_easy_setopt( cURLHandle, CURLOPT_ERRORBUFFER, CURLErrorBuffer );
  DEBUG_LOG_RETURN( cURLHandle, "Resource_Server_API::SetupcURLForPost; Obtained cURL handle for PostURL=" << PostURL );
 }

 CRITICAL_LOG_RETURN( cURLHandle, "Resource_Server_API::SetupcURLForPost; Failed to obtain a cURL handle for PostURL=" << PostURL );
}

// ------------------------------------------------------------------------------

size_t Resource_Server_API::WriteToStringCallBack( void *ptr, size_t size, size_t nmemb, void *stream )
{
 string *PointerToString = (string *)( stream );

 (*PointerToString).append( (char *)( ptr ), size * nmemb );

 return( size * nmemb );
}

// ------------------------------------------------------------------------------

CURLcode Resource_Server_API::PerformcURLPost( string &Response, CURL *cURLHandle, curl_httppost *PostList )
{

 if( cURLHandle != NULL )
 {
  Response.reserve( 8192 );    
  Response.clear();

  curl_easy_setopt( cURLHandle, CURLOPT_WRITEDATA, &Response );
  curl_easy_setopt( cURLHandle, CURLOPT_WRITEFUNCTION, WriteToStringCallBack );
  if( PostList != NULL ) curl_easy_setopt( cURLHandle, CURLOPT_HTTPPOST, PostList );

  CURLcode cURLResult = curl_easy_perform( cURLHandle );

  curl_easy_cleanup( cURLHandle );
  if( PostList != NULL ) curl_formfree( PostList );

  if( cURLResult != CURLE_OK )
   CRITICAL_LOG_RETURN( cURLResult, "Resource_Server_API::PerformcURLPost; Could not perform post, returned " << cURLResult << " with error buffer: " << CURLErrorBuffer )
  else
  {
   DEBUG_LOG( "Resource_Server_API::PerformcURLPost; Performed post" ); 
   VERBOSE_DEBUG_LOG( "Resource_Server_API::PerformcURLPost; Response=" << Response );
   return( cURLResult );
  }

 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::PerformcURLPost; Received an invalid cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_SignUp_Resource( string &Response, string ServerURL, string Project, string Capabilities )
{
 DEBUG_LOG( "Resource_Server_API::Resource_SignUp_Resource; ServerURL=" << ServerURL << ", Project=" << Project );

 string PostURL = ServerURL + "/resources/resource_signup_resource.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  if( !Capabilities.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "capabilities", CURLFORM_PTRCONTENTS, Capabilities.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_SignUp_Resource; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_SignUp_Resource; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_SignOff_Resource( string &Response, string ServerURL, string Project, string SessionID )
{
 DEBUG_LOG( "Resource_Server_API::Resource_SignOff_Resource; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID );

 string PostURL = ServerURL + "/resources/resource_signoff_resource.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_SignOff_Resource; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_SignOff_Resource; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Request_Work( string &Response, string ServerURL, string Project, string SessionID, string Application, string Start, string Limit )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Request_Work; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Application=" << Application << ", Start=" << Start << ", Limit=" << Limit );

 string PostURL = ServerURL + "/resources/resource_request_work.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "application", CURLFORM_PTRCONTENTS, Application.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "start", CURLFORM_PTRCONTENTS, Start.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "limit", CURLFORM_PTRCONTENTS, Limit.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Request_Work; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Request_Work; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Request_Job_Details( string &Response, string ServerURL, string Project, string SessionID, string Job_Id )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Request_Job_Details; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Job_Id=" << Job_Id );

 string PostURL = ServerURL + "/resources/resource_request_job_details.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Request_Job_Details; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Request_Job_Details; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Lock_Job( string &Response, string ServerURL, string Project, string SessionID, string Job_Id )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Lock_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Job_Id=" << Job_Id );

 string PostURL = ServerURL + "/resources/resource_lock_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Lock_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Lock_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_UnLock_Job( string &Response, string ServerURL, string Project, string SessionID, string Job_Id )
{
 DEBUG_LOG( "Resource_Server_API::Resource_UnLock_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Job_Id=" << Job_Id );

 string PostURL = ServerURL + "/resources/resource_unlock_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_UnLock_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_UnLock_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Update_Job( string &Response, string ServerURL, string Project, string SessionID, string Job_Id, string State, string Target_Resources, string Input, string Output, string Job_Specifics )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Update_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Job_Id=" << Job_Id << ", State=" << State << ", Target_Resources=" << Target_Resources << ", Input=" << Input << ", Output=" << Output << ", Job_Specifics=" << Job_Specifics );

 string PostURL = ServerURL + "/resources/resource_update_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );
  if( !State.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "state", CURLFORM_PTRCONTENTS, State.c_str(), CURLFORM_END );
  if( !Target_Resources.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "target_resources", CURLFORM_PTRCONTENTS, Target_Resources.c_str(), CURLFORM_END );
  if( !Input.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "input", CURLFORM_PTRCONTENTS, Input.c_str(), CURLFORM_END );
  if( !Output.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "output", CURLFORM_PTRCONTENTS, Output.c_str(), CURLFORM_END );
  if( !Job_Specifics.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_specifics", CURLFORM_PTRCONTENTS, Job_Specifics.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Update_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Update_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );
}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Submit_Job( string &Response, string ServerURL, string Project, string SessionID, string Application, string State, string Owners, string Target_Resources, string Read_Access, string Job_Specifics, string Input, string Output, vector<string> FilesToUpload )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Submit_Job; ServerURL=" << ServerURL << ", Project=" << Project << ", SessionID=" << SessionID << ", Application=" << Application << ", State=" << State << ", Owners=" << Owners << ", Target_Resources=" << Target_Resources << ", Read_Access=" << Read_Access << ", Input=" << Input << ", Output=" << Output << ", Job_Specifics=" << Job_Specifics );

 string PostURL = ServerURL + "/resources/resource_submit_job.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "session_id", CURLFORM_PTRCONTENTS, SessionID.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "application", CURLFORM_PTRCONTENTS, Application.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "state", CURLFORM_PTRCONTENTS, State.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "owners", CURLFORM_PTRCONTENTS, Owners.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "target_resources", CURLFORM_PTRCONTENTS, Target_Resources.c_str(), CURLFORM_END );
  if( !Read_Access.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "read_access", CURLFORM_PTRCONTENTS, Read_Access.c_str(), CURLFORM_END );
  if( !Job_Specifics.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_specifics", CURLFORM_PTRCONTENTS, Job_Specifics.c_str(), CURLFORM_END );
  if( !Input.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "input", CURLFORM_PTRCONTENTS, Input.c_str(), CURLFORM_END );
  if( !Output.empty() ) curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "output", CURLFORM_PTRCONTENTS, Output.c_str(), CURLFORM_END );

  if( FilesToUpload.size() )     // there are files to be uploaded too...
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

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Submit_Job; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Submit_Job; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Job_State( string &Response, string ServerURL, string Project, string Job_Id )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Job_State; ServerURL=" << ServerURL << ", Project=" << Project << ", Job_Id=" << Job_Id );

 string PostURL = ServerURL + "/resources/resource_job_state.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "project", CURLFORM_PTRCONTENTS, Project.c_str(), CURLFORM_END );
  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "job_id", CURLFORM_PTRCONTENTS, Job_Id.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Job_state; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Job_State; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------

int Resource_Server_API::Resource_Request_Resource_Data( string &Response, string ServerURL, string Project, string Resource_Name )
{
 DEBUG_LOG( "Resource_Server_API::Resource_Request_Resource_Data; ServerURL=" << ServerURL << ", Project=" << Project << ", Resource_Name=" << Resource_Name );

 string PostURL = ServerURL + "/resources/resource_request_resource_data.php";
 CURL *cURLHandle = SetupcURLForPost( PostURL );

 if( cURLHandle != NULL )
 {
  struct curl_httppost *PostList = NULL;
  struct curl_httppost *LastItem = NULL;

  curl_formadd( &PostList, &LastItem, CURLFORM_PTRNAME, "resource_name", CURLFORM_PTRCONTENTS, Resource_Name.c_str(), CURLFORM_END );

  CURLcode cURLResult = PerformcURLPost( Response, cURLHandle, PostList );

  VERBOSE_DEBUG_LOG_RETURN( cURLResult, "Resource_Server_API::Resource_Request_Resource_Data; returned " << cURLResult );
 }
 else
  CRITICAL_LOG_RETURN( CURLE_FAILED_INIT, "Resource_Server_API::Resource_Request_Resource_Data; Couldn't obtain cURL handle, returned " << CURLE_FAILED_INIT );

}

// ------------------------------------------------------------------------------
