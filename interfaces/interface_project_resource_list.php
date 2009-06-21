<?php

// []--------------------------------------------------------[]
//  |          interface_project_resource_list.php           |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        List resources from project DB...          |
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

$queryresult = mysql_query( "SELECT resource_name,last_call_time,resource_capabilities FROM active_resources WHERE project_server=0" );

$NumberOfResources = mysql_num_rows( $queryresult );

$Response .= " <number_of_resources> ".$NumberOfResources." </number_of_resources>";

for( $i = 1; $i <= $NumberOfResources; $i++ )
{
 $Resource = mysql_fetch_object( $queryresult );
 $Response .= " <resource number='".$i."'> <resource_name> $Resource->resource_name </resource_name>";
 $Response .= " <resource_capabilities> $Resource->resource_capabilities </resource_capabilities>";
 $Response .= " <last_call_time> $Resource->last_call_time </last_call_time> </resource>";
}

if( $NumberOfResources ) mysql_free_result( $queryresult );

// return the response...
return( LGI_Response( $Response ) );
?>

