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

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Html.inc' );
require_once( '../inc/Repository.inc' );

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

// check if job_id is set in request... 
if( isset( $_GET[ "job_id" ] ) )
 $Job_ID = $_GET[ "job_id" ];
else
 if( isset( $_POST[ "job_id" ] ) )
  $Job_ID = $_POST[ "job_id" ];
if( !isset( $Job_ID ) ) Exit_With_Text( "ERROR: Job_id was not posted" );
if( strlen( $Job_ID ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 47 ] );
if( !is_numeric( $Job_ID ) ) Exit_With_Text( "ERROR: Job_id is not a number" );

// now verify the user using the basic browser interface... also make MySQL connection...
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

 // remove the repository...
 $RepositoryURL = NormalizeString( Parse_XML( $JobSpecs -> job_specifics, "repository", $Attributes ) );
 if( $RepositoryURL != "" ) DeleteRepository( $RepositoryURL );

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
Row2( "<b>Project:</b>", htmlentities( $Project ) ); 
Row2( "<b>This project server:</b>", Get_Server_URL() ); 
Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface/index.php?project=$Project&groups=$Groups&sid=$SID'>".Get_Master_Server_URL()."</a>" );
Row2( "<b>User:</b>", htmlentities( $User ) ); 
Row2( "<b>Groups:</b>", htmlentities( $Groups ) ); 
Row1( "<center><font color='green' size='4'><b>Job deleted or aborted</b></font></center>" );
Row2( "<b>Job ID:</b>", $Job_ID );
Row2( "<b>Application:</b>", htmlentities( $JobSpecs -> application ) );
Row2( "<b>State:</b>", "<b>".htmlentities( $JobState )."</b>" );
Row2( "<b>State time stamp:</b>", "<b>".gmdate( "j M Y G:i", $JobStateTimeStamp )." UTC</b>" );
Row2( "<b>Owners:</b>", htmlentities( $JobSpecs -> owners ) );
Row2( "<b>Read access:</b>", htmlentities( $JobSpecs -> read_access ) );
Row2( "<b>Write access:</b>", htmlentities( $JobSpecs -> write_access ) );
Row2( "<b>Target resources:</b>", htmlentities( $JobSpecs -> target_resources ) );
Row2( "<b>Job specifics:</b>", nl2br( htmlentities( $JobSpecs -> job_specifics ) ) );
End_Table();

echo "<br><a href='basic_interface_list.php?project_server=1&project=$Project&groups=$Groups&sid=$SID'>Show project server list</a>\n";
echo "<br><a href='basic_interface_list.php?project_server=0&project=$Project&groups=$Groups&sid=$SID'>Show project resource list</a>\n"; 
echo "<br><a href='basic_interface_submit_job_form.php?project=$Project&groups=$Groups&sid=$SID'>Submit a job</a>\n";
echo "<br><a href='basic_interface_job_state.php?project=$Project&groups=$Groups&sid=$SID'>Show job list</a>\n"; 
echo "<br><a href='index.php?project=$Project&groups=$Groups&sid=$SID'>Go to main menu</a>\n"; 

Page_Tail();
?>

