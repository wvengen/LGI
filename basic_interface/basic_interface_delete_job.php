<?php

// []--------------------------------------------------------[]
//  |             basic_interface_delete_job.php             |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, October 2007.                        |
//  | USE:        Basic interface delete job...              |
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
require_once( '../inc/Errors.inc' );
require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );
require_once( '../inc/Html.inc' );

global $Config;
global $ErrorMsgs;

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

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

// check if job_id is set in request... 
if( isset( $_GET[ "job_id" ] ) )
 $Job_ID = $_GET[ "job_id" ];
else
 if( isset( $_POST[ "job_id" ] ) )
  $Job_ID = $_POST[ "job_id" ];
if( !isset( $Job_ID ) ) Exit_With_Text( "ERROR: Job_id was not posted" );
if( strlen( $Job_ID ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 47 ] );
if( !is_numeric( $Job_ID ) ) Exit_With_Text( "ERROR: Job_id is not a number" );

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

// wait until we can get details...
$JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $Job_ID, false );
if( $JobSpecs === 15 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );
if( $JobSpecs === 2 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );

// check if we are allowed to delete the job...
if( !Interface_Is_User_Allowed_To_Modify_Job( $JobSpecs, $User, $Groups ) ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 35 ] );

// if the job can be actually removed...
if( ( $JobSpecs->state == 'queued' ) || ( $JobSpecs->state == 'finished' ) || ( $JobSpecs->state == 'aborted' ) )
{
 $ErrorCode = Interface_Set_Spin_Lock_On_Job( $Job_ID, false );
 if( $ErrorCode != 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

 $RepositoryURL = NormalizeString( Parse_XML( $JobSpecs -> job_specifics, "repository", $Attributes ) );
 if( $RepositoryURL != "" )
 {
  $RepositoryArray = CommaSeparatedField2Array( $RepositoryURL, ":" );

  if( $RepositoryArray[ 0 ] == 2 )
   rmpath( $RepositoryArray[ 2 ] );
 }

 // now delete from db...
 $queryresult = mysql_query( "DELETE FROM job_queue WHERE job_id=".$Job_ID );
 $JobState = "deleted";
 $JobStateTimeStamp = time();
}
else
{
 $ErrorCode = Interface_Set_Spin_Lock_On_Job( $Job_ID, false );
 if( $ErrorCode != 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );
 $queryresult = mysql_query( "UPDATE job_queue SET state='aborting',state_time_stamp=UNIX_TIMESTAMP() WHERE job_id=".$Job_ID );
 $ErrorCode = Interface_Clear_Spin_Lock_On_Job( $Job_ID, false );
 if( $ErrorCode != 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );
 $JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $Job_ID, false );
 if( $JobSpecs === 15 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );
 if( $JobSpecs === 2 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );
 $JobState = $JobSpecs->state;
 $JobStateTimeStamp = $JobSpecs->state_time_stamp;
}

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</b></font></center>" );
Row2( "<b>Project:</b>", $Project ); 
Row2( "<b>This project server:</b>", Get_Server_URL() ); 
Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface'>".Get_Master_Server_URL()."</a>" );
Row2( "<b>User:</b>", $User ); 
Row2( "<b>Groups:</b>", $Groups ); 
Row1( "<center><font color='green' size='4'><b>Job deleted or aborted</b></font></center>" );
Row2( "<b>Job ID:</b>", $Job_ID );
Row2( "<b>Application:</b>", $JobSpecs -> application );
Row2( "<b>State:</b>", "<b>$JobState</b>" );
Row2( "<b>State time stamp:</b>", "<b>".gmdate( "j M Y G:i", $JobStateTimeStamp )." UTC</b>" );
Row2( "<b>Owners:</b>", $JobSpecs -> owners );
Row2( "<b>Read access:</b>", $JobSpecs -> read_access );
Row2( "<b>Target resources:</b>", $JobSpecs -> target_resources );
Row2( "<b>Job specifics:</b>", htmlentities( $JobSpecs -> job_specifics ) );
End_Table();

echo "<br><a href='basic_interface_list.php?project_server=1'>Show project server list</a>\n";
echo "<br><a href='basic_interface_list.php?project_server=0'>Show project resource list</a>\n"; 
echo "<br><a href='basic_interface_submit_job_form.php'>Submit a job</a>\n";
echo "<br><a href='basic_interface_job_state.php'>Show job list</a>\n"; 
echo "<br><a href='index.php'>Go to main menu</a>\n"; 

Page_Tail();
?>

