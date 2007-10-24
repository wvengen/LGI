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

require_once( '../inc/Config.inc' );
require_once( '../inc/Resources.inc' );

global $Config;
global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ] );

// now create a new session for this resource...
Resource_Setup_Session( $ResourceData );

// start building the response of this api-call...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server>";
$Response .= " <this_project_server> ".Get_Server_URL()." </this_project_server>";

$queryresult = mysql_query( "SELECT url FROM active_resources WHERE project_server=2" );

$NumberOfServers = mysql_num_rows( $queryresult );

$Response .= " <number_of_slave_servers> ".$NumberOfServers." </number_of_slave_servers>";

for( $i = 1; $i <= $NumberOfServers; $i++ )
{
 $Server = mysql_fetch_object( $queryresult );
 $Response .= " <project_server number='".$i."'> ".$Server->url." </project_server>";
}

mysql_free_result( $queryresult );

$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";

// and return the response...
return( LGI_Response( $Response ) );
?>
