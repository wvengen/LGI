<?php

// []--------------------------------------------------------[]
//  |            resource_request_resource_data.php          |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Return resource data from project DB...    |
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

require_once( '../inc/Resources.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], $_POST[ "session_id" ] );

// check if compulsory post variable was set...
if( !isset( $_POST[ "resource_name" ] ) || ( $_POST[ "resource_name" ] == "" ) )
 return( LGI_Error_Response( 43, $ErrorMsgs[ 43 ] ) );
else
{
 if( strlen( $_POST[ "resource_name" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 63, $ErrorMsgs[ 63 ] ) );
 $ResourceName = mysql_escape_string( $_POST[ "resource_name" ] );
}

// query for the job specs...
$ResourceQuery = mysql_query( "SELECT * FROM active_resources WHERE resource_name='".$ResourceName."'" );
if( $ResourceQuery )
 $NrRows = mysql_num_rows( $ResourceQuery );
else
 $NrRows = 0;
if( $NrRows != 1 )
{
 if( $NrRows ) mysql_free_result( $ResourceQuery );
 return( LGI_Error_Response( 5, $ErrorMsgs[ 5 ] ) );
}

$ResourceSpecs = mysql_fetch_object( $ResourceQuery );
mysql_free_result( $ResourceQuery );

// build response for this job...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <requested_resource_data> <resource_name> ".$ResourceSpecs->resource_name." </resource_name>";
$Response .= " <client_certificate> ".binhex( $ResourceSpecs->client_certificate )." </client_certificate>";
$Response .= " <resource_url> ".$ResourceSpecs->url." </url>";
$Response .= " <last_call_time> ".$ResourceSpecs->last_call_time." </last_call_time> </requested_resource_data>";

// return the response...
return( LGI_Response( $Response ) );
?>

