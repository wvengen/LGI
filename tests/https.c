/*****************************************************************************
*                                  _   _ ____  _
*  Project                     ___| | | |  _ \| |
*                             / __| | | | |_) | |
*                            | (__| |_| |  _ <| |___
*                             \___|\___/|_| \_\_____|
*
* $Id: https.c,v 1.2 2004/11/24 16:11:35 bagder Exp $
*/

#include <stdio.h>
#include <curl/curl.h>

const char *pClientCertFile = "../certificates/CLIENT_1.crt";
const char *pClientPrivateKeyFile = "../certificates/CLIENT.key";
const char *pCACertFile = "../certificates/LGI+CA.crt";
const char *pRequestedData = "project=LGI&job_id=2";

int main( int argc, char *argv[] )
{
  CURL *curl;
  CURLcode result;
 
  if( argc < 3 )
  {
   printf( "USE https.x URL [POSTSTRING]\n\n" ); 
   return( 1 );
  }

  curl = curl_easy_init();
  if( curl ) {
   
    curl_easy_setopt( curl, CURLOPT_URL, argv[ 1 ] );

    // for use with client certificates ...
    curl_easy_setopt( curl, CURLOPT_SSLCERT, pClientCertFile );
    curl_easy_setopt( curl, CURLOPT_SSLKEY, pClientPrivateKeyFile );

    // verify server certidicate ...
    curl_easy_setopt( curl, CURLOPT_CAINFO, pCACertFile );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 1 );
    curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 1 );

    // post the request for the common name
    curl_easy_setopt( curl, CURLOPT_POSTFIELDS, argv[ 2 ] );

    result = curl_easy_perform( curl );

    curl_easy_cleanup( curl );

  }
  return 0;
}

