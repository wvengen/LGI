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

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Html.inc' );
require_once( '../inc/Repository.inc' );

// check session...
session_start();
$SID = $_SESSION[ "sid" ];
if( $SID != $_POST[ "sid" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 66 ] );

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

// check if groups is set in request... or default to user's group...
$Groups = $User;
if( isset( $_GET[ "groups" ] ) )
 $Groups = $_GET[ "groups" ];
else
 if( isset( $_POST[ "groups" ] ) )
  $Groups = $_POST[ "groups" ];
if( strlen( $Groups ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 56 ] );

// check if project is set in request... or default to value set in config...
$Project = $Config[ "MYSQL_DEFAULT_DATABASE" ];
if( isset( $_GET[ "project" ] ) )
 $Project = $_GET[ "project" ];
else
 if( isset( $_POST[ "project" ] ) )
  $Project = $_POST[ "project" ];
if( strlen( $Project ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 58 ] );

// check if application was given...
$Application = "";
if( isset( $_GET[ "application" ] ) )
 $Application = $_GET[ "application" ];
else
 if( isset( $_POST[ "application" ] ) )
  $Application = $_POST[ "application" ];
if( $Application == "" ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 18 ] );
if( strlen( $Application ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 46 ] );

// check if target_resources was given...
$TargetResources = "";
if( isset( $_GET[ "target_resources" ] ) )
 $TargetResources = $_GET[ "target_resources" ];
else
 if( isset( $_POST[ "target_resources" ] ) )
  $TargetResources = $_POST[ "target_resources" ];
if( $TargetResources == "" ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 27 ] );
if( strlen( $TargetResources ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 50 ] );

// check if job_specifics was given...
$JobSpecifics = "";
if( isset( $_GET[ "job_specifics" ] ) )
 $JobSpecifics = $_GET[ "job_specifics" ];
else
 if( isset( $_POST[ "job_specifics" ] ) )
  $JobSpecifics = $_POST[ "job_specifics" ];
if( strlen( $JobSpecifics ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 53 ] );

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

// check posted number of uploaded files...
$NrOfUploadedFiles = -1;
if( isset( $_POST[ "number_of_uploaded_files" ] ) && is_numeric( $_POST[ "number_of_uploaded_files" ] ) )
 $NrOfUploadedFiles = $_POST[ "number_of_uploaded_files" ];
if( $NrOfUploadedFiles < 0 ) Exit_With_Text( "ERROR: Hidden field number_of_uploaded_files not correctly posted." );

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

// check if any of posted target resources is allowed...
$Resources = CommaSeparatedField2Array( $TargetResources, "," );

$NewTargetResourceList = "";
$FoundResourceFlag = 0;

for( $i = 1; $i <= $Resources[ 0 ]; $i++ )
 if( Interface_Is_Target_Resource_Known( $Resources[ $i ] ) )
 {
  $NewTargetResourceList .= ", ".$Resources[ $i ];
  $FoundResourceFlag = 1;
 }

if( !$FoundResourceFlag ) 
 Exit_With_Text( "ERROR: ".$ErrorMsgs[ 28 ] ); 

$TargetResources = substr( $NewTargetResourceList, 2 );

// check if any of the posted combinations of user+groups or just the user is allowed to submit a job...
$GroupsArray = CommaSeparatedField2Array( $Groups, "," );
$NewGroupsList = "";
$FoundPossibleGroup = 0;

for( $i = 1; $i <= $GroupsArray[ 0 ]; $i++ )
{
 $LimitType = Interface_User_In_Group_Is_Allowed_To_Submit( $User, $GroupsArray[ $i ], $Application, $NumberOfJobsAllowed );

 if( $LimitType )           // is allowed to submit a job?
 {
  if( $NumberOfJobsAllowed )           // if there is a limit set for this owner, check it...
  {

   if( $LimitType == 1 )          // was it a limit defined from the user tables or the group tables...
    Interface_Count_Owners_Jobs_In_Queue( $User, $Application, $TotalNrOfJobs, $NrOfRunningJobs );
   else
    Interface_Count_Owners_Jobs_In_Queue( $GroupsArray[ $i ], $Application, $TotalNrOfJobs, $NrOfRunningJobs );

   if( $NumberOfJobsAllowed > 0 )
   {
    if( $TotalNrOfJobs < $NumberOfJobsAllowed )   // check if total nr of jobs for this owner is allowed...
    {
     $NewGroupsList .= ", ".$GroupsArray[ $i ];
     $FoundPossibleGroup = 1;
    }
   }
   else
   {
    if( $NrOfRunningJobs + $NumberOfJobsAllowed < 0 ) // check if nr of running jobs for this owner is allowed...
    {
     $NewGroupsList .= ", ".$GroupsArray[ $i ];
     $FoundPossibleGroup = 1;
    }
   }

  }
  else
  {
   $NewGroupsList .= ", ".$GroupsArray[ $i ];
   $FoundPossibleGroup = 1;
  }
 }
}

if( $FoundPossibleGroup )
 $Groups = substr( $NewGroupsList, 2 );
else
 Exit_With_Text( "ERROR: ".$ErrorMsgs[ 33 ] );

// now determine the owners and read_access fields based on the possibly posted data and the user+groups data...
if( $ReadAccess != "" )
{
 if( $Owners != "" )
 {
  // if owners and read_access were posted...
  $Owners = $User.", ".$Owners;
  $ReadAccess = $User.", ".$Groups.", ".$ReadAccess.", ".$Owners;
 }
 else
 {
  // if only read_access was posted...
  $Owners = $User;
  $ReadAccess = $User.", ".$Groups.", ".$ReadAccess;
 }
}
else
{
 if( $Owners != "" )
 {
  // if only owners was posted...
  $Owners = $User.", ".$Owners;
  $ReadAccess = $User.", ".$Groups.", ".$Owners;
 }
 else
 {
  // if neither owners nor read_access were posted...
  $Owners = $User;
  $ReadAccess = $User.", ".$Groups;
 }
}

// create the job respository directory...
CreateRepository( $RepositoryDir, $RepositoryURL, $RepositoryIDFile );

// now handle file uploads...
for( $i = 1; $i <= $NrOfUploadedFiles; $i++ )
{
 $FileHandle = "uploaded_file_$i";

 if( isset( $_FILES[ $FileHandle ] ) )
 {
  $File = $_FILES[ $FileHandle ];

  if( $_FILES[ $FileHandle ][ "error" ] === UPLOAD_ERR_OK )
  {
   if( $RepositoryIDFile != "" )
   {
    exec( $Config[ "REPOSITORY_SCP_COMMAND" ]." -qBi $RepositoryIDFile ".$File[ "tmp_name" ]." \"$RepositoryURL:$RepositoryDir/'".$File[ "name" ]."'\"" );
    exec( $Config[ "REPOSITORY_SSH_COMMAND" ]." -i $RepositoryIDFile $RepositoryURL \"chmod 440 '$RepositoryDir/".$File[ "name" ]."'\"" );
   }
   else
    move_uploaded_file( $File[ "tmp_name" ], $RepositoryDir."/".$File[ "name" ] );
  }
  else
   if( isset( $File[ "name" ] ) && ( $File[ "name" ] != "" ) )
    Exit_With_Text( "ERROR: ".$ErrorMsgs[ 64 ].": '".$File[ "name" ]."'" );
 }
}

// make sure that future REGEXP's do work...
$Owners = mysql_escape_string( NormalizeCommaSeparatedField( $Owners, "," ) );
$ReadAccess = mysql_escape_string( NormalizeCommaSeparatedField( $ReadAccess, "," ) );
$Application = mysql_escape_string( $Application );
$TargetResources = mysql_escape_string( $TargetResources );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='queued', application='".$Application."', owners='".$Owners."', read_access='".$ReadAccess."', target_resources='".$TargetResources."', lock_state=0, state_time_stamp=UNIX_TIMESTAMP(), job_specifics='".mysql_escape_string( $JobSpecifics." <repository> $RepositoryURL:$RepositoryDir </repository>" )."'";

if( $Input != "" )
 $InsertQuery .= ", input='".mysql_escape_string( $Input )."'";

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// get repository url from specs...
$RepositoryURL = RepositoryURL2WWW( NormalizeString( Parse_XML( $JobSpecs -> job_specifics, "repository", $Attributes ) ) );

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</font></center>" );
Row2( "<b>Project:</b>", $Project );
Row2( "<b>This project server:</b>", Get_Server_URL() );
Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface/index.php?project=$Project&groups=$Groups&sid=$SID'>".Get_Master_Server_URL()."</a>" );
Row2( "<b>User:</b>", $User );
Row2( "<b>Groups:</b>", $Groups );
Row1( "<center><font color='green' size='4'><b>Submitted job details</b></font></center>" );
Row2( "<b>Job ID:</b>", $JobSpecs -> job_id );
Row2( "<b>Application:</b>", $JobSpecs -> application );
Row2( "<b>State:</b>", $JobSpecs -> state );
Row2( "<b>State time stamp:</b>", gmdate( "j M Y G:i", $JobSpecs -> state_time_stamp )." UTC" );
Row2( "<b>Owners:</b>", $JobSpecs -> owners );
Row2( "<b>Read access:</b>", $JobSpecs -> read_access );
Row2( "<b>Target resources:</b>", $JobSpecs -> target_resources );
Row2( "<b>Job specifics:</b>", nl2br( htmlentities( $JobSpecs -> job_specifics ) ) );
if( $RepositoryURL != "" ) Row2( "<b>Repository:</b>", "<a href='".$RepositoryURL."'> $RepositoryURL </a>" );
Row2( "<b>Input:</b>", nl2br( htmlentities( $JobSpecs -> input ) ) );
End_Table();

echo "<br><a href='basic_interface_delete_job.php?project=$Project&groups=$Groups&job_id=".$JobSpecs -> job_id."&sid=$SID'>Abort or Delete this job</a>\n";
echo "<br><a href='basic_interface_list.php?project=$Project&groups=$Groups&project_server=1&sid=$SID'>Show project server list</a>\n";
echo "<br><a href='basic_interface_list.php?project=$Project&groups=$Groups&project_server=0&sid=$SID'>Show project resource list</a>\n";
echo "<br><a href='basic_interface_submit_job_form.php?project=$Project&groups=$Groups&sid=$SID'>Submit a job</a>\n";
echo "<br><a href='basic_interface_job_state.php?project=$Project&groups=$Groups&sid=$SID'>Show job list</a>\n";
echo "<br><a href='index.php?project=$Project&groups=$Groups&sid=$SID'>Go to main menu</a>\n";

Page_Tail();

?>

