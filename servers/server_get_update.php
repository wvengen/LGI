<?php

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

// check if compulsory post variable was set...
if( !isset( $_POST[ "version" ] ) || ( $_POST[ "version" ] == "" ) || !is_numeric( $_POST[ "version" ] ) )
 return( LGI_Error_Response( 40, $ErrorMsgs[ 40 ], "" ) );
else
 $ServerVersionNumber = $_POST[ "version" ];

// build response header...
$Response = "<project> ".Get_Selected_MySQL_DataBase()." </project> ";
$Response .= "<project_master_server> ".Get_Master_Server_URL()." </project_master_server> ";
$Response .= "<this_project_server> ".Get_Server_URL()." </this_project_server> ";
$Response .= "<requesting_project_server> ".$ServerData->resource_name." </requesting_project_server> ";
$Response .= "<requesting_project_server_version> ".$ServerVersionNumber." </requesting_project_server_version> ";

// query db for possible updates...
$mysqlresult = mysql_query( "SELECT * FROM updates WHERE version>".$ServerVersionNumber." ORDER BY version ASC" );

// if there are any updates, report the first one to be done...
if( mysql_num_rows( $mysqlresult ) >= 1 )
{
 $UpDateData = mysql_fetch_object( $mysqlresult );
 $Response .= "<update> <updates> ".mysql_num_rows( $mysqlresult )." </updates> ";
 $Response .= "<update_version> ".$UpDateData->version." </update_version> ";
 $Response .= "<target_servers> ".$UpDateData->servers." </target_servers> ";
 $Response .= "<update_query> ".binhex( $UpDateData->update_query )." </update_query> </update>";
}
else
{
 // there are no updates for the requesting project-server here...
 mysql_free_result( $mysqlresult );

 $Response .= "<update> <updates> 0 </updates> ";

 // get latest update we have on this project-server...
 $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
 if( mysql_num_rows( $mysqlresult ) == 1 )
 {
  $UpDateData = mysql_fetch_object( $mysqlresult );
  $Response .= "<update_version> ".$UpDateData->max." </update_version> </update>";
 }
 else
  $Response .= "<update_version> -1 </update_version> </update>";

}
mysql_free_result( $mysqlresult );

// return the response...
return( LGI_Response( $Response, "" ) );
?>

