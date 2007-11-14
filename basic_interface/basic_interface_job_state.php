<?php

// []--------------------------------------------------------[]
//  |             basic_interface_job_state.php              |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, October 2007.                        |
//  | USE:        Request job state from project DB...       |
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

// check if job_id is set in request... otherwise default to list mode...
if( isset( $_GET[ "job_id" ] ) )
 $Job_ID = $_GET[ "job_id" ];
else
 if( isset( $_POST[ "job_id" ] ) )
  $Job_ID = $_POST[ "job_id" ];

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

if( isset( $Job_ID ) )               // we requested details on a job...
{
 if( strlen( $Job_ID ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 47 ] );
 if( !is_numeric( $Job_ID ) ) Exit_With_Text( "ERROR: Job_id not a number" );

 $JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $Job_ID, false );
 if( $JobSpecs === 15 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );
 if( $JobSpecs === 2 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $JobSpecs ] );

 if( !Interface_Is_User_Allowed_To_Read_Job( $JobSpecs, $User, $Groups ) ) Exit_With_Text( "ERROR: User is not allowed to get details of this job" );

 // get repository url from specs...
 $RepositoryURL = NormalizeString( Parse_XML( $JobSpecs -> job_specifics, "repository", $Attributes ) );
 if( $RepositoryURL != "" )
 {
  $RepositoryArray = CommaSeparatedField2Array( $RepositoryURL, ":" );

  if( $RepositoryArray[ 0 ] == 2 )
  {
   $RepositoryArray = CommaSeparatedField2Array( $RepositoryArray[ 2 ], "/" );
   $RepositoryURL = $RepositoryArray[ 1 ];
   if( $Config[ "REPOSITORY_URL" ] != "" ) $RepositoryURL = $Config[ "REPOSITORY_URL" ]."/".$RepositoryURL;
  }
  else
   $RepositoryURL = "";
 }

 Start_Table();
 Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</font></center>" );
 Row2( "<b>Project:</b>", $Project ); 
 Row2( "<b>This project server:</b>", Get_Server_URL() ); 
 Row2( "<b>Project master server:</b>", "<a href=".Get_Master_Server_URL()."/basic_interface>".Get_Master_Server_URL()."</a>" );
 Row2( "<b>User:</b>", $User ); 
 Row2( "<b>Groups:</b>", $Groups ); 
 Row1( "<center><font color='green' size='4'><b>Job details</b></font></center>" );
 Row2( "<b>Job ID:</b>", $Job_ID ); 
 Row2( "<b>Application:</b>", $JobSpecs -> application ); 
 Row2( "<b>State:</b>", $JobSpecs -> state ); 
 Row2( "<b>State time stamp:</b>", gmdate( "j M Y G:i", $JobSpecs -> state_time_stamp )." UTC" ); 
 Row2( "<b>Owners:</b>", $JobSpecs -> owners ); 
 Row2( "<b>Read access:</b>", $JobSpecs -> read_access ); 
 Row2( "<b>Target resources:</b>", $JobSpecs -> target_resources ); 
 Row2( "<b>Job specifics:</b>", htmlentities( $JobSpecs -> job_specifics ) ); 
 if( $RepositoryURL != "" ) Row2( "<b>Repository:</b>", "<a href=$RepositoryURL> $RepositoryURL </a>" ); 
 Row2( "<b>Input:</b>", htmlentities( $JobSpecs -> input ) ); 
 Row2( "<b>Output:</b>", htmlentities( $JobSpecs -> output ) ); 
 End_Table();

 echo "<br><a href='basic_interface_delete_job.php?job_id=".$JobSpecs -> job_id."'>Abort or Delete this job</a>\n";
 echo "<br><a href='basic_interface_job_state.php?groups=".$Groups."&project=".$Project."'>Show job list</a>\n"; 
 echo "<br><a href='basic_interface_list.php?project_server=1'>Show project server list</a>\n"; 
 echo "<br><a href='basic_interface_list.php?project_server=0'>Show project resource list</a>\n"; 
 echo "<br><a href='basic_interface_submit_job_form.php'>Submit a job</a>\n";
 echo "<br><a href='index.php'>Go to main menu</a>\n"; 
}
else
{

 // check if state is set in request... otherwise default to any...
 $State = "any";
 if( isset( $_GET[ "state" ] ) )
  $State = $_GET[ "state" ];
 else
  if( isset( $_POST[ "state" ] ) )
   $State = $_POST[ "state" ];
 $State = mysql_escape_string( $State );

 // check if application is set in request... otherwise default to any...
 $Application = "any";
 if( isset( $_GET[ "application" ] ) )
  $Application = $_GET[ "application" ];
 else
  if( isset( $_POST[ "application" ] ) )
   $Application = $_POST[ "application" ];
 $Application = mysql_escape_string( $Application );

 // check if application is set in request... otherwise default to 0...
 $Start = "0";
 if( isset( $_GET[ "start" ] ) )
  $Start = $_GET[ "start" ];
 else
  if( isset( $_POST[ "start" ] ) )
   $Start = $_POST[ "start" ];
 if( !is_numeric( $Start ) ) Exit_With_Text( "ERROR: Start not a number" );

 // check if application is set in request... otherwise default to a nice number...
 $Limit = "64";
 if( isset( $_GET[ "limit" ] ) )
  $Limit = $_GET[ "limit" ];
 else
  if( isset( $_POST[ "limit" ] ) )
   $Limit = $_POST[ "limit" ];
 if( !is_numeric( $Limit ) ) Exit_With_Text( "ERROR: Limit not a number" );

 Start_Table();
 Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</b></font></center>" );
 Row2( "<b>Project:</b>", $Project ); 
 Row2( "<b>This project server:</b>", Get_Server_URL() ); 
 Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface'>".Get_Master_Server_URL()."</a>" );
 Row2( "<b>User:</b>", $User ); 
 Row2( "<b>Groups:</b>", $Groups ); 
 Row2( "<b>Application:</b>", $Application ); 
 Row2( "<b>State:</b>", $State ); 
 Row2( "<b>Start index:</b>", $Start ); 
 Row2( "<b>Index limit:</b>", $Limit ); 

 // build the query to execute...
 $PossibleJobOwnersArray = CommaSeparatedField2Array( $User.", ".$Groups.", any", "," );
 $Query = Interface_Make_Query_For_Work_For_Owners( $PossibleJobOwnersArray, $State, $Application, $Start, $Limit );

 // do the query and get number of jobs to list...
 $QueryResult = mysql_query( $Query );
 $Number = mysql_num_rows( $QueryResult );
 
 Row2( "<b>Number of jobs listed:</b>", $Number ); 

 Row5( "<center><font color='green' size='4'><b>job id</b></font></center>", "<center><font color='green' size='4'><b>job state</b></font></center>", "<center><font color='green' size='4'><b>target resources</b></font></center>", "<center><font color='green' size='4'><b>application</b></font></center>", "<center><font color='green' size='4'><b>time stamp</b></font></center>" );

 for( $i = 1; $i <= $Number; $i++ )
 {
  $Job = mysql_fetch_object( $QueryResult );
  Row5( "<center><a href='basic_interface_job_state.php?job_id=$Job->job_id&groups=$Groups&project=$Project'>$Job->job_id</a></center>", "<center>$Job->state</center>", "<center>$Job->target_resources</center>", "<center>$Job->application</center>", "<center>".gmdate( "j M Y G:i", $Job->state_time_stamp )." UTC </center>" );
 }

 mysql_free_result( $QueryResult ); 

 End_Table();

 $Menu = ""; 
 if( $Start > 0 )
 {
  if( $Start > $Limit )
   $Menu .= "<a href='basic_interface_job_state.php?start=".($Start-$Limit)."&limit=$Limit&state=$State&groups=$Groups&application=$Application&project=$Project'> Prev </a>";
  else
   $Menu .= "<a href='basic_interface_job_state.php?start=0&limit=$Limit&state=$State&groups=$Groups&application=$Application&project=$Project'> Prev </a>";
 }
 if( $Number >= $Limit  )
  $Menu .= "<a href='basic_interface_job_state.php?start=".($Start+$Number)."&limit=$Limit&state=$State&groups=$Groups&application=$Application&project=$Project'> Next </a>";
 if( $Menu != "" ) echo "$Menu<br>";

 echo "<br><a href='basic_interface_list.php?project_server=1'>Show project server list</a>\n"; 
 echo "<br><a href='basic_interface_list.php?project_server=0'>Show project resource list</a>\n"; 
 echo "<br><a href='basic_interface_submit_job_form.php'>Submit a job</a>\n";
 echo "<br><a href='index.php'>Go to main menu</a>\n"; 
}

Page_Tail();
?>

