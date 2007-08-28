<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Resources.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ] );      

// check if this call is valid...
if( !$ResourceData->active )
  return( LGI_Error_Response( 42, $ErrorMsgs[ 42 ], "" ) );

// now mark the current resource as active...
Resource_Mark_As_InActive( $ResourceData );

// remove any stale locks this resource might have...
Resource_Remove_Stale_Locks( $ResourceData );

// start building the response of this api-call...
$Response = " <resource> ".$ResourceData->resource_name." </resource>";
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

$Response .= " <resource_active> ".$ResourceData->active." </resource_active>";

// and return the response...
LGI_Response( $Response, "" );
?>
