<?php

// []--------------------------------------------------------[]
//  |                 basic_interface_list.php               |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, October 2007.                        |
//  | USE:        Show list of servers or resources...       |
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
require_once( '../inc/Html.inc' );

// check session...
session_start();
$SID = $_SESSION[ "sid" ];
if( !isset( $_GET[ "sid" ] ) || ( $SID != $_GET[ "sid" ] ) || ( $_GET[ "sid" ] == "" ) ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 66 ] );

Page_Head();

// check if user is set in request... or use value from certificate...
$CommonNameArray = CommaSeparatedField2Array( SSL_Get_Common_Name(), ";" );
$User = $CommonNameArray[ 1 ];
if( isset( $_GET[ "user" ] ) )
 $User = $_GET[ "user" ];
else
 if( isset( $_POST[ "user" ] ) )
  $User = $_POST[ "user" ];
if( strlen( $User ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 57 ] );
$User = mysql_escape_string( $User );

// check if groups is set in request... or default to data from cert...
$Groups = Interface_Get_Groups_From_Common_Name( SSL_Get_Common_Name() );
if( isset( $_GET[ "groups" ] ) )
 $Groups = $_GET[ "groups" ];
else
 if( isset( $_POST[ "groups" ] ) )
  $Groups = $_POST[ "groups" ];
if( strlen( $Groups ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 56 ] );
$Groups = mysql_escape_string( $Groups );

// check if project is set in request... or default to value set in config...
$Project = $Config[ "MYSQL_DEFAULT_DATABASE" ];
if( isset( $_GET[ "project" ] ) )
 $Project = $_GET[ "project" ];
else
 if( isset( $_POST[ "project" ] ) )
  $Project = $_POST[ "project" ];
if( strlen( $Project ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 58 ] );
$Project = mysql_escape_string( $Project );

// check if project_server is set in request... or default to 0...
$ProjectServer = "0";
if( isset( $_GET[ "project_server" ] ) )
 $ProjectServer = $_GET[ "project_server" ];
else
 if( isset( $_POST[ "project_server" ] ) )
  $ProjectServer = $_POST[ "project_server" ];
if( strlen( $ProjectServer ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] ) Exit_With_Text( "ERROR: Project_server field posted too big" );
if( !is_numeric( $ProjectServer ) ) Exit_With_Text( "ERROR: Project_server is not a number" );

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</font></center>" );
Row2( "<b>Project:</b>", htmlentities( $Project ) ); 
Row2( "<b>This project server:</b>", Get_Server_URL() ); 
Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface/index.php?project=$Project&groups=$Groups&sid=$SID'>".Get_Master_Server_URL()."</a>" );
Row2( "<b>User:</b>", htmlentities( $User ) ); 
Row2( "<b>Groups:</b>", htmlentities( $Groups ) ); 

if( $ProjectServer != 0 )
{
 $ServerListQuery = mysql_query( "SELECT resource_name,url,last_call_time FROM active_resources WHERE project_server<>0" );
 $Number = mysql_num_rows( $ServerListQuery );

 Row2( "<b>Number of servers:</b>", $Number );
 Row3( "<center><font color='green' size='4'><b>Server name</b></font></center>", "<center><font color='green' size='4'><b>URL</b></font></center>", "<center><font color='green' size='4'><b>Time stamp</b></font></center>" ); 

 for( $i = 1; $i <= $Number; $i++ )
 {
  $Server = mysql_fetch_object( $ServerListQuery );
  Row3( "<center>$Server->resource_name</center>", "<center><a href='$Server->url/basic_interface/index.php?project=$Project&groups=$Groups&sid=$SID'>$Server->url</a></center>", "<center>".gmdate( "j M Y G:i", $Server->last_call_time )." UTC </center>" );
 }

 if( $Number ) mysql_free_result( $ServerListQuery );

 End_Table();

 echo "<br><a href='basic_interface_list.php?project_server=0&project=$Project&groups=$Groups&sid=$SID'>Show project resource list</a>\n";
}
else
{
 $ResourceListQuery = mysql_query( "SELECT resource_name,last_call_time,resource_capabilities FROM active_resources WHERE project_server=0" );
 $Number = mysql_num_rows( $ResourceListQuery );

 Row2( "<b>Number of resources:</b>", $Number );
 Row3( "<center><font color='green' size='4'><b>Resource name</b></font></center>", "<center><font color='green' size='4'><b>Time stamp</b></font></center>", "<center><font color='green' size='4'><b>Resource capabilities</b></font></center>" ); 

 for( $i = 1; $i <= $Number; $i++ )
 {
  $Resource = mysql_fetch_object( $ResourceListQuery );
  Row3( "<center>$Resource->resource_name</center>", "<center>".gmdate( "j M Y G:i", $Resource ->last_call_time )." UTC </center>", "<center>".nl2br( htmlentities( $Resource->resource_capabilities ) )."</center>" );
 }

 if( $Number ) mysql_free_result( $ResourceListQuery );
 End_Table();

 echo "<br><a href='basic_interface_list.php?project_server=1&project=$Project&groups=$Groups&sid=$SID'>Show project server list</a>\n";
}

echo "<br><a href='basic_interface_job_state.php?project=$Project&groups=$Groups&sid=$SID'>Show job list</a>\n"; 
echo "<br><a href='basic_interface_submit_job_form.php?project=$Project&groups=$Groups&sid=$SID'>Submit a job</a>\n";
echo "<br><a href='index.php?project=$Project&groups=$Groups&sid=$SID'>Go to main menu</a>\n"; 

Page_Tail();
?>

