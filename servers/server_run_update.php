<?php

// []--------------------------------------------------------[]
//  |                server_run_update.php                   |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Apply update to this server...             |
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
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;
 
// check if server is known to the project and certified correctly...
$ServerData = Server_Verify( $_POST[ "project" ] );

// check if update is being pushed to this server...
if( isset( $_POST[ "version" ] ) && isset( $_POST[ "servers" ] ) && isset( $_POST[ "update" ] ) )
{
 $UpdateVersion = $_POST[ "version" ];
 $TargetServers = $_POST[ "servers" ];
 $UpdateQuery = $_POST[ "update" ];

 // check if posted fields are correct and allowed...
 if( !is_numeric( $UpdateVersion ) || ( $TargetServers == "" ) || ( $UpdateQuery == "" ) )
  return( LGI_Error_Response( 41, $ErrorMsgs[ 41 ], "" ) );
 
 // build response header...
 $Response = "<project> ".Get_Selected_MySQL_DataBase()." </project> ";
 $Response .= "<project_master_server> ".Get_Master_Server_URL()." </project_master_server> ";
 $Response .= "<this_project_server> ".$ServerData->resource_name." </this_project_server> ";
 $Response .= "<project_server> ".Get_Server_URL()." </project_server> ";
 $Response .= "<update_version> ".$UpdateVersion." </update_version> ";
 $Response .= "<target_servers> ".$TargetServers." </target_servers> ";
 $Response .= "<update_query> ".$UpdateQuery." </update_query> ";

 // check latest update we have...
 $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
 if( mysql_num_rows( $mysqlresult ) == 1 )
 {
  $MaxVersion = mysql_fetch_object( $mysqlresult );

  mysql_free_result( $mysqlresult );

  // do we need to start an update cycle...
  if( $UpdateVersion > $MaxVersion->max + 1 )
  {
   Server_Check_And_Perform_Updates();
   mysql_free_result( $mysqlresult );
   $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
   $MaxVersion = mysql_fetch_object( $mysqlresult );
   mysql_free_result( $mysqlresult );
   $Response .= "<update> <did_update_cyle> 1 </did_update_cyle> <update_version> ".$MaxVersion->max." </update_version> </update>";
   return( LGI_Response( $Response, "" ) );
  }

  // if we already have this update...
  if( $UpdateVersion <= $MaxVersion->max )
  {
   $Response .= "<update> <did_update_cyle> 0 </did_update_cyle> <update_version> ".$MaxVersion->max." </update_version> </update>";
   return( LGI_Response( $Response, "" ) );
  }
 }
  
 mysql_free_result( $mysqlresult );

 $UpdateQuery = hexbin( $UpdateQuery );

 // perform the update on the DB if needed...
 if( FoundInCommaSeparatedField( $TargetServers, Get_Server_Name(), "," ) )
  $mysqlresult = mysql_query( $UpdateQuery );

 // insert this update into the DB...
 mysql_query( "INSERT INTO updates SET version=".$UpdateVersion.", servers='".mysql_escape_string( $TargetServers )."', update_query='".mysql_escape_string( $UpdateQuery )."'" );

 // get latest update we now have on this project-server...
 $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
 if( mysql_num_rows( $mysqlresult ) == 1 )
 {
  $UpdateData = mysql_fetch_object( $mysqlresult );
  $Response .= "<update> <did_update_cyle> 0 </did_update_cyle> <update_version> ".$UpdateData->max." </update_version> </update>";
 }
 else
  $Response .= "<update> <did_update_cyle> 0 </did_update_cyle> <update_version> -1 </update_version> </update>";

 mysql_free_result( $mysqlresult );
 return( LGI_Response( $Response, "" ) );
}

// this is an update cycle being initiated by another server...
Server_Check_And_Perform_Updates();

$mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
$MaxVersion = mysql_fetch_object( $mysqlresult );
mysql_free_result( $mysqlresult );
$Response = "<project> ".Get_Selected_MySQL_DataBase()." </project> ";
$Response .= "<project_master_server> ".Get_Master_Server_URL()." </project_master_server> ";
$Response .= "<this_project_server> ".$ServerData->resource_name." </this_project_server> ";
$Response .= "<project_server> ".Get_Server_URL()." </project_server> ";
$Response .= "<update> <did_update_cyle> 1 </did_update_cyle> <update_version> ".$MaxVersion->max." </update_version> </update>";

return( LGI_Response( $Response, "" ) );
?>

