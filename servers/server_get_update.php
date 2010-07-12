<?php

// []--------------------------------------------------------[]
//  |                server_get_update.php                   |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Get latest update of this server...        |
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

require_once( '../inc/Servers.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if server is known to the project and certified correctly...
$ServerData = Server_Verify( $_POST[ "project" ] );

// check if compulsory post variable was set...
if( !isset( $_POST[ "version" ] ) || ( $_POST[ "version" ] == "" ) || !ctype_digit( $_POST[ "version" ] ) )
 return( LGI_Error_Response( 40, $ErrorMsgs[ 40 ] ) );
else
{
 if( strlen( $_POST[ "version" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 59, $ErrorMsgs[ 59 ] ) );
 $ServerVersionNumber = $_POST[ "version" ];
}

// build response header...
$Response = "<project> ".Get_Selected_MySQL_DataBase()." </project> ";
$Response .= "<project_master_server> ".Get_Master_Server_URL()." </project_master_server> ";
$Response .= "<this_project_server> ".Get_Server_URL()." </this_project_server> ";
$Response .= "<requesting_project_server> ".$ServerData->resource_name." </requesting_project_server> ";
$Response .= "<requesting_project_server_version> ".$ServerVersionNumber." </requesting_project_server_version> ";

// query db for possible updates...
$mysqlresult = mysql_query( "SELECT * FROM updates WHERE version>".$ServerVersionNumber." ORDER BY version ASC" );
if( $mysqlresult )
 $NrOfUpdates = mysql_num_rows( $mysqlresult );
else 
 $NrOfUpdates = 0;

// if there are any updates, report the first one to be done...
if( $NrOfUpdates >= 1 )
{
 $UpDateData = mysql_fetch_object( $mysqlresult );
 $Response .= "<update> <updates> ".$NrOfUpdates." </updates> ";
 $Response .= "<update_version> ".$UpDateData->version." </update_version> ";
 $Response .= "<target_servers> ".$UpDateData->servers." </target_servers> ";
 $Response .= "<update_query> ".binhex( $UpDateData->update_query )." </update_query> </update>";
 mysql_free_result( $mysqlresult );
}
else
{
 // there are no updates for the requesting project-server here...
 $Response .= "<update> <updates> 0 </updates> ";

 // get latest update we have on this project-server...
 $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
 if( $mysqlresult )
 {
  $UpDateData = mysql_fetch_object( $mysqlresult );
  $Response .= "<update_version> ".$UpDateData->max." </update_version> </update>";
  mysql_free_result( $mysqlresult );
 }
 else
  $Response .= "<update_version> -1 </update_version> </update>";
}

// return the response...
return( LGI_Response( $Response ) );
?>

