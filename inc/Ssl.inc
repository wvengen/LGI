<?php

// []--------------------------------------------------------[]
//  |                        Ssl.inc                         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Handy ssl related subroutines...           |
//  |                                                        |
// []--------------------------------------------------------[]
//
//
// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/Errors.inc' );
require_once( '../inc/Response.inc' );

// -------------------------------------------------------------------- 

function SSL_Verify_Client( $ExitOnError = true )
{
 global $Config;
 global $ErrorMsgs;

 // See if a valid CA-signed client certificate was used to connect

 if( $GLOBALS[ "HTTP_SERVER_VARS" ][ "SSL_CLIENT_VERIFY" ] != "SUCCESS" )
  return( LGI_Error_Response( 2, $ErrorMsgs[ 2 ], "", $ExitOnError ) );

 return( 0 );
}

// -------------------------------------------------------------------- 

function SSL_Get_Common_Name( )
{
 return( $GLOBALS[ "HTTP_SERVER_VARS" ][ "SSL_CLIENT_S_DN_CN" ] );
}

// -------------------------------------------------------------------- 

function SSL_Get_Full_Client_Certificate( )
{
 return( $GLOBALS[ "HTTP_SERVER_VARS" ][ "SSL_CLIENT_CERT" ] );
}

// -------------------------------------------------------------------- 

function SSL_Get_Full_Server_Certificate( )
{
 return( $GLOBALS[ "HTTP_SERVER_VARS" ][ "SSL_SERVER_CERT" ] );
}

// --------------------------------------------------------------------

function SSL_Post_To_URL( $URL, $POST )
{
 global $Config;

 $Handle = curl_init( $URL );

 curl_setopt( $Handle, CURLOPT_SSLCERT, $Config[ "SERVER_SSL_CERTIFICATE_FILE" ] );
 curl_setopt( $Handle, CURLOPT_SSLKEY, $Config[ "SERVER_SSL_KEY" ] );
 curl_setopt( $Handle, CURLOPT_CAINFO, $Config[ "SERVER_SSL_CA_CERTIFICATE_FILE" ] );
 curl_setopt( $Handle, CURLOPT_SSL_VERIFYPEER, 1 );
 curl_setopt( $Handle, CURLOPT_SSL_VERIFYHOST, 1 );
 curl_setopt( $Handle, CURLOPT_POSTFIELDS, $POST );
 curl_setopt( $Handle, CURLOPT_RETURNTRANSFER, 1 );

 $Response = curl_exec( $Handle );

 curl_close( $Handle );

 return( $Response );
}

// --------------------------------------------------------------------

?>
