<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// start building the response of this api-call...
$Response = "<user> ".$JobUser." </user> <groups> ".$JobGroups." </groups> ";
$Response .= "<project> ".Get_Selected_MySQL_DataBase()." </project> ";
$Response .= "<project_master_server> ".Get_Master_Server_URL()." </project_master_server> ";

$queryresult = mysql_query( "SELECT url FROM active_resources WHERE project_server=2" );

$NumberOfServers = mysql_num_rows( $queryresult );

$Response .= "<number_of_slave_servers> ".$NumberOfServers." </number_of_slave_servers> ";

for( $i = 1; $i <= $NumberOfServers; $i++ )
{
 $Server = mysql_fetch_object( $queryresult );
 $Response .= " <project_server number='".$i."'> ".$Server->url." </project_server> ";
}

mysql_free_result( $queryresult );

// return the response...
return( LGI_Response( $Response, "" ) );
?>

