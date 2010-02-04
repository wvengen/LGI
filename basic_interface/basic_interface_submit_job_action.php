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
if( !isset( $_POST[ "sid" ] ) || ( $SID != $_POST[ "sid" ] ) || ( $_POST[ "sid" ] == "" ) ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 66 ] );

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

// check if groups is set in request... or default to data from cert...
$Groups = Interface_Get_Groups_From_Common_Name( SSL_Get_Common_Name() );
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
$Application = NormalizeString( $Application );

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

// check if write_access was given...
$WriteAccess = "";
if( isset( $_GET[ "write_access" ] ) )
 $WriteAccess = $_GET[ "write_access" ];
else
 if( isset( $_POST[ "write_access" ] ) )
  $WriteAccess = $_POST[ "write_access" ];
if( strlen( $WriteAccess ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 52 ] );

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

// Check if application is known...
$QueryResult = mysql_query( "SELECT COUNT(resource_id) AS count FROM active_resources WHERE resource_capabilities LIKE '%<".$Application.">%</".$Application.">%'" );
$QueryData = mysql_fetch_object( $QueryResult );
mysql_free_result( $QueryResult);
if( $QueryData -> count <= 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 70 ] );

// check if any of posted target resources is allowed...
$Resources = CommaSeparatedField2Array( $TargetResources, "," );

$NewTargetResourceList = "";
$FoundResourceFlag = 0;

for( $i = 1; $i <= $Resources[ 0 ]; $i++ )
 if( Interface_Is_Target_Resource_Known( $Resources[ $i ], $Application ) )
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
    Interface_Count_Jobs_In_Queue( $User, $Application, $TotalNrOfJobs, $NrOfRunningJobs );
   else
    Interface_Count_Jobs_In_Queue( $GroupsArray[ $i ], $Application, $TotalNrOfJobs, $NrOfRunningJobs );

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

// now determine the write_access and read_access fields based on the possibly posted data and the user+groups data...
if( $ReadAccess != "" )
 $ReadAccess = $User.", ".NormalizeCommaSeparatedField( $ReadAccess, "," );     // check perhaps if read_access is part of owners=user+groups allowed to submit?
else
 $ReadAccess = $User;

if( $WriteAccess != "" )
 $WriteAccess = $User.", ".NormalizeCommaSeparatedField( $WriteAccess, "," );  // check perhaps if write_access is part of owners=user+groups allowed to submit?
else
 $WriteAccess = $User;

// add admins groups to read and write access lists...
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] != "" ) ) $WriteAccess = $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ].", ".$WriteAccess
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] != "" ) ) $ReadAccess = $Config[ "LGI_ADMIN_GROUP_FOR_READ" ].", ".$ReadAccess

// create the job respository directory...
CreateRepository( $RepositoryDir, $RepositoryURL, $RepositoryIDFile, $WriteAccess );
$RepositoryWWWURL = RepositoryURL2WWW( $RepositoryURL.":".$RepositoryDir );

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
   {
    DeleteRepository( $RepositoryURL.":".$RepositoryDir );
    Exit_With_Text( "ERROR: ".$ErrorMsgs[ 64 ].": '".$File[ "name" ]."'" );
   }
 }
}

// make sure that future REGEXP's do work...
$Owners = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $User.", ".$Groups, "," ) );
$ReadAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $ReadAccess, "," ) );
$WriteAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $WriteAccess, "," ) );
$Application = mysql_escape_string( $Application );
$TargetResources = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $TargetResources, "," ) );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='queued', application='".$Application."', owners='".$Owners."', write_access='".$WriteAccess."', read_access='".$ReadAccess."', target_resources='".$TargetResources."', lock_state=0, state_time_stamp=UNIX_TIMESTAMP(), job_specifics='".mysql_escape_string( $JobSpecifics." <repository> $RepositoryURL:$RepositoryDir </repository> <repository_url> $RepositoryWWWURL </repository_url>" )."'";

if( $Input != "" )
 $InsertQuery .= ", input='".mysql_escape_string( $Input )."'";

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// and redirect browser with a simple java script to the job_state page so that the POST url is not in browser history...
echo '<script type="text/javascript">window.location.replace("basic_interface_job_state.php?project='.$Project.'&groups='.$Groups.'&sid='.$SID.'&job_id='.$JobSpecs->job_id.'");</script>'
?>
