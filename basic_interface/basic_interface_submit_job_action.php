<?php

// []--------------------------------------------------------[]
//  |          basic_interface_submit_job_action.php         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Submit job to project DB...                |
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
require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );
require_once( '../inc/Html.inc' );

global $ErrorMsgs;
global $Config;

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

// check if groups is set in request... or default to user's group...
$Groups = $User;
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

// check if application was given...
$Application = "";
if( isset( $_GET[ "application" ] ) )
 $Application = $_GET[ "application" ];
else
 if( isset( $_POST[ "application" ] ) )
  $Application = $_POST[ "application" ];
if( $Application == "" ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 18 ] );
if( strlen( $Application ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 46 ] );
$Application = mysql_escape_string( $Application );

// check if target_resources was given...
$TargetResources = "";
if( isset( $_GET[ "target_resources" ] ) )
 $TargetResources = $_GET[ "target_resources" ];
else
 if( isset( $_POST[ "target_resources" ] ) )
  $TargetResources = $_POST[ "target_resources" ];
if( $TargetResources == "" ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 27 ] );
if( strlen( $TargetResources ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 50 ] );
$TargetResources = mysql_escape_string( $TargetResources );

// check if job_specifics was given...
$JobSpecifics = "";
if( isset( $_GET[ "job_specifics" ] ) )
 $JobSpecifics = $_GET[ "job_specifics" ];
else
 if( isset( $_POST[ "job_specifics" ] ) )
  $JobSpecifics = $_POST[ "jobs_specifics" ];
if( strlen( $JobSpecifics ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 53 ] );
$JobSpecifics = mysql_escape_string( $JobSpecifics );

// check if owners was given...
$Owners = "";
if( isset( $_GET[ "owners" ] ) )
 $Owners = $_GET[ "owners" ];
else
 if( isset( $_POST[ "owners" ] ) )
  $Owners = $_POST[ "owners" ];
if( strlen( $Owners ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 52 ] );

// check if read_access was given...
$ReadAccess = "";
if( isset( $_GET[ "read_access" ] ) )
 $ReadAccess = $_GET[ "read_access" ];
else
 if( isset( $_POST[ "read_access" ] ) )
  $ReadAccess = $_POST[ "read_access" ];
if( strlen( $ReadAccess ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 51 ] );

// check if input was given...
$Input = "";
if( isset( $_GET[ "input" ] ) )
 $Input = $_GET[ "input" ];
else
 if( isset( $_POST[ "input" ] ) )
  $Input = $_POST[ "input" ];
if( strlen( $Input ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 54 ] );

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

// ...
// ...
// ...

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</font></center>" );
Row2( "<b>This project server:</b>", Get_Server_URL() );
Row2( "<b>Project master server:</b>", "<a href=".Get_Master_Server_URL()."/basic_interface>".Get_Master_Server_URL()."</a>" );
Row2( "<b>User:</b>", $User );
Row2( "<b>Groups:</b>", $Groups );
Row2( "<b>Project:</b>", $Project );
Row1( "<center><font color='green' size='4'><b>Job details</b></font></center>" );
Row2( "<b>Job ID:</b>", $Job_ID );
Row2( "<b>Application:</b>", $JobSpecs -> application );
Row2( "<b>State:</b>", $JobSpecs -> state );
Row2( "<b>State time stamp:</b>", gmdate( "j M Y G:i", $JobSpecs -> state_time_stamp )." UTC" );
Row2( "<b>Owners:</b>", $JobSpecs -> owners );
Row2( "<b>Read access:</b>", $JobSpecs -> read_access );
Row2( "<b>Target resources:</b>", $JobSpecs -> target_resources );
Row2( "<b>Job specifics:</b>", $JobSpecs -> job_specifics );
Row2( "<b>Input:</b>", $JobSpecs -> input );
End_Table();

Page_Tail();

?>

