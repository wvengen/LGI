/* []--------------------------------------------------------[]
    |                resource_server_api.h                   |
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

#if !defined(__RESOURCE_SERVER_API_INCLUDED__)
#define __RESOURCE_SERVER_API_INCLUDED__

#include <cstdio>
#include <string>
#include <vector>
#include <curl/curl.h>

#include "logger.h"

using namespace std;

// ------------------------------------------------------------------------------

#define LGI_SERVER_BACKOFF_ERROR_NR     1

// ------------------------------------------------------------------------------

class Resource_Server_API
      {
       public:

              Resource_Server_API( string KeyFile, string CertificateFile, string CAFile, CURL *cURLHandle = NULL, bool Strict = true );
              ~Resource_Server_API( void );

              int Resource_SignUp_Resource( string &Response, string ServerURL, string Project, string Capabilities = "" );
              int Resource_SignOff_Resource( string &Response, string ServerURL, string Project, string SessionID );
              int Resource_Request_Work( string &Response, string ServerURL, string Project, string SessionID, string Application, string Start = "", string Limit = "", string Owners = "" );
              int Resource_Request_Job_Details( string &Response, string ServerURL, string Project, string SessionID, string Job_Id );
              int Resource_Lock_Job( string &Response, string ServerURL, string Project, string SessionID, string Job_Id );
              int Resource_UnLock_Job( string &Response, string ServerURL, string Project, string SessionID, string Job_Id );
              int Resource_Update_Job( string &Response, string ServerURL, string Project, string Session_ID, string Job_Id, string State = "", string Target_Resources = "", string Input = "", string Output = "" , string Job_Specifics = "" );
              int Resource_Submit_Job( string &Response, string ServerURL, string Project, string Session_ID, string Application, string State, string Owners, string Target_Resources, string Read_Access = "", string Write_Access = "", string Job_Specifics = "", string Input = "", string Output = "", vector<string> FilesToUpload = vector<string>() );
              int Resource_Job_State( string &Response, string ServerURL, string Project, string Job_Id );
              int Resource_Request_Resource_Data( string &Response, string ServerURL, string Project, string Resource_Name );

       private:

              CURL           *SetupcURLForPost( string &PostURL );
              CURLcode       PerformcURLPost( string &Response, CURL *cURLHandle, curl_httppost *PostList = NULL );
              static size_t  WriteToStringCallBack( void *ptr, size_t size, size_t nmemb, void *stream );

       string PrivateKeyFile;
       string PublicCertificateFile;
       string CAChainFile;
       char   CURLErrorBuffer[ CURL_ERROR_SIZE ];
       CURL   *MycURLHandle;
       int    CreatedMycURLHandle;
       bool   CheckHostname;
      }; 
  
// ------------------------------------------------------------------------------

#endif

