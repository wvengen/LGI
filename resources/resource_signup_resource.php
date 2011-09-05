<?php

// []--------------------------------------------------------[]
//  |                resource_signup_resource.php            |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Signup resource to project DB...           |
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
if( isset( $_POST[ "capabilities" ] ) )
 $ResourceData = Resource_Verify( $_POST[ "project" ], "", $_POST[ "capabilities" ] );
else
 $ResourceData = Resource_Verify( $_POST[ "project" ] );

// now create a new session for this resource...
Resource_Setup_Session( $ResourceData );

// start building the response of this api-call...
$Response = " <resource> ".XML_Escape( $ResourceData->resource_name )." </resource>";
$Response .= " <resource_url> ".XML_Escape( $ResourceData->url )." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server>";
$Response .= " <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";

$queryresult = mysql_query( "SELECT url FROM active_resources WHERE project_server=2" );
if( $queryresult )
 $NumberOfServers = mysql_num_rows( $queryresult );
else 
 $NumberOfServers = 0;

$Response .= " <number_of_slave_servers> ".$NumberOfServers." </number_of_slave_servers>";

for( $i = 1; $i <= $NumberOfServers; $i++ )
{
 $Server = mysql_fetch_object( $queryresult );
 $Response .= " <project_server number='".$i."'> ".XML_Escape( $Server->url )." </project_server>";
}

if( $NumberOfServers ) mysql_free_result( $queryresult );

$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";

// and return the response...
return( LGI_Response( $Response ) );
?>
