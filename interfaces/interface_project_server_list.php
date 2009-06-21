<?php

// []--------------------------------------------------------[]
//  |            interface_project_server_list.php           |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        List project servers from project DB...    |
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

require_once( '../inc/Interfaces.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// start building the response of this api-call...
$Response = "<user> ".$JobUser." </user> <groups> ".$JobGroups." </groups>";
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

if( $NumberOfServers ) mysql_free_result( $queryresult );

// return the response...
return( LGI_Response( $Response ) );
?>

