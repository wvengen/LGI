/* []--------------------------------------------------------[]
    |                interface_server_api.h                  |
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

#if !defined(__INTERFACE_SERVER_API_INCLUDED__)
#define __INTERFACE_SERVER_API_INCLUDED__

#include <cstdio>
#include <string>
#include <vector>
#include <curl/curl.h>

#include "logger.h"

using namespace std;

// ------------------------------------------------------------------------------

#define SERVER_BACK_OF_ERROR_NR 1

// ------------------------------------------------------------------------------

class Interface_Server_API
      {
       public:

              Interface_Server_API( string KeyFile, string CertificateFile, string CAFile );

              int Interface_Project_Server_List( string &Response, string ServerURL, string Project, string User, string Groups );
              int Interface_Project_Resource_List( string &Response, string ServerURL, string Project, string User, string Groups );
              int Interface_Job_State( string &Response, string ServerURL, string Project, string User, string Groups, string Job_Id = "", string State = "", string Application = "", string Start = "", string Limit = "" );
              int Interface_Delete_Job( string &Response, string ServerURL, string Project, string User, string Groups, string Job_Id );
              int Interface_Submit_Job( string &Response, string ServerURL, string Project, string User, string Groups, string Application, string Target_Resources, string Job_Specifics = "", string Input = "", string Read_Access = "", string Write_Access = "", string Output = "", vector<string> FilesToUpload = vector<string>() );

       private:

              CURL           *SetupcURLForPost( string &PostURL );
              CURLcode       PerformcURLPost( string &Response, CURL *cURLHandle, curl_httppost *PostList = NULL );
              static size_t  WriteToStringCallBack( void *ptr, size_t size, size_t nmemb, void *stream );

       string PrivateKeyFile;
       string PublicCertificateFile;
       string CAChainFile;
       char   CURLErrorBuffer[ CURL_ERROR_SIZE ];
      }; 
  
// ------------------------------------------------------------------------------

#endif

