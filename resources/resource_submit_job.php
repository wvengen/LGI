<?php

// []--------------------------------------------------------[]
//  |               resource_submit_job.php                  |
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

require_once( '../inc/Resources.inc' );
require_once( '../inc/Repository.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], $_POST[ "session_id" ] );

// check if this call is valid...
if( Resource_Verify_Session( $ResourceData ) )
 return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ] ) );

// check if compulsory post variables were set...
if( !isset( $_POST[ "state" ] ) || ( $_POST[ "state" ] == "" ) )
 return( LGI_Error_Response( 25, $ErrorMsgs[ 25 ] ) );
else
{
 if( strlen( $_POST[ "state" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 45, $ErrorMsgs[ 45 ] ) );
 $JobState = NormalizeString( $_POST[ "state" ] );
}

if( !isset( $_POST[ "application" ] ) || ( $_POST[ "application" ] == "" ) )
 return( LGI_Error_Response( 18, $ErrorMsgs[ 18 ] ) );
else
{
 if( strlen( $_POST[ "application" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 46, $ErrorMsgs[ 46 ] ) );
 $JobApplication = NormalizeString( $_POST[ "application" ] );
}

if( !isset( $_POST[ "owners" ] ) || ( $_POST[ "owners" ] == "" ) )
 return( LGI_Error_Response( 26, $ErrorMsgs[ 26 ] ) );
else
{
 if( strlen( $_POST[ "owners" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 71, $ErrorMsgs[ 71 ] ) );
 $JobOwners = NormalizeCommaSeparatedField( $_POST[ "owners" ], "," );
}

if( !isset( $_POST[ "target_resources" ] ) || ( $_POST[ "target_resources" ] == "" ) )
 return( LGI_Error_Response( 27, $ErrorMsgs[ 27 ] ) );
else
{
 if( strlen( $_POST[ "target_resources" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 50, $ErrorMsgs[ 50 ] ) );
 $JobTargetResources = NormalizeCommaSeparatedField( $_POST[ "target_resources" ], "," );
}

// check posted number of uploaded files...
$NrOfUploadedFiles = -1;
if( isset( $_POST[ "number_of_uploaded_files" ] ) && ctype_digit( $_POST[ "number_of_uploaded_files" ] ) )
 $NrOfUploadedFiles = $_POST[ "number_of_uploaded_files" ];

// check if this resource has any jobs locked...
if( Resource_Check_For_Job_Locks( $ResourceData ) != 0 )
 return( LGI_Error_Response( 17, $ErrorMsgs[ 17 ] ) );

// Check if application is known...
$QueryResult = mysql_query( "SELECT COUNT(resource_id) AS count FROM active_resources WHERE resource_capabilities LIKE '%<".$JobApplication.">%</".$JobApplication.">%'" );
$QueryData = mysql_fetch_object( $QueryResult );
mysql_free_result( $QueryResult);
if( $QueryData -> count <= 0 ) return( LGI_Error_Response( 70, $ErrorMsgs[ 70 ] ) );

// check if any of posted target resources is allowed...
$Resources = CommaSeparatedField2Array( $JobTargetResources, "," );

$NewTargetResourceList = "";
$FoundResourceFlag = 0;

for( $i = 1; $i <= $Resources[ 0 ]; $i++ )
 if( Resource_Is_Target_Resource_Known( $Resources[ $i ], $JobApplication ) )
 {
  $NewTargetResourceList .= ", ".$Resources[ $i ];
  $FoundResourceFlag = 1;
 }

if( !$FoundResourceFlag )
 return( LGI_Error_Response( 28, $ErrorMsgs[ 28 ] ) );

$JobTargetResources = substr( $NewTargetResourceList, 2 );

// check if any of the posted owners is allowed to submit a job...
$OwnersArray = CommaSeparatedField2Array( $JobOwners, "," );

$NewOwnersList = "";
$FoundPossibleOwner = 0;

for( $i = 1; $i <= $OwnersArray[ 0 ]; $i++ )
 if( Resource_Is_Owner_Allowed_To_Submit( $OwnersArray[ $i ], $JobApplication, $NumberOfJobsAllowed ) )
 {
  if( $NumberOfJobsAllowed )           // if there is a limit set for this owner, check it...
  {
   Resource_Count_Owners_Jobs_In_Queue( $OwnersArray[ $i ], $JobApplication, $TotalNrOfJobs, $NrOfRunningJobs );

   if( $NumberOfJobsAllowed > 0 )
   {
    if( $TotalNrOfJobs < $NumberOfJobsAllowed )   // check if total nr of jobs for this owner is allowed...
    {
     $NewOwnersList .= ", ".$OwnersArray[ $i ];
     $FoundPossibleOwner = 1;
    }
   }
   else
   {
    if( $NrOfRunningJobs + $NumberOfJobsAllowed < 0 ) // check if nr of running jobs for this owner is allowed...
    {
     $NewOwnersList .= ", ".$OwnersArray[ $i ];
     $FoundPossibleOwner = 1;
    }
   }
    
  }
  else
  {
   $NewOwnersList .= ", ".$OwnersArray[ $i ];
   $FoundPossibleOwner = 1;
  }
 }

if( !$FoundPossibleOwner )
 return( LGI_Error_Response( 29, $ErrorMsgs[ 29 ] ) );

// create a repository...
CreateRepository( $RepositoryDir, $RepositoryURL, $RepositoryIDFile, $NewOwnersList );
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
    return( LGI_Error_Response( 68, $ErrorMsgs[ 68 ]) );
   }
 }
}

$JobOwners = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( substr( $NewOwnersList, 2 ), "," ) );
$JobState = mysql_escape_string( $JobState );
$JobApplication = mysql_escape_string( $JobApplication );
$JobTargetResources = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $JobTargetResources, "," ) );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='".$JobState."', application='".$JobApplication."', owners='".$JobOwners."', target_resources='".$JobTargetResources."', lock_state=1, state_time_stamp=UNIX_TIMESTAMP(), priority=UNIX_TIMESTAMP()";

if( isset( $_POST[ "job_specifics" ] ) && ( $_POST[ "job_specifics" ] != "" ) )
{
 if( strlen( $_POST[ "job_specifics" ] ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] )
  return( LGI_Error_Response( 53, $ErrorMsgs[ 53 ] ) );
 if( XML_Sanity_check( $_POST[ "job_specifics" ] ) )
  return( LGI_Error_Response( 72, $ErrorMsgs[ 72 ] ) );
 $InsertQuery .= ", job_specifics='".mysql_escape_string( $_POST[ "job_specifics" ]." <repository> $RepositoryURL:$RepositoryDir </repository> <repository_url> $RepositoryWWWURL </repository_url>" )."'";
}
else
 $InsertQuery .= ", job_specifics='".mysql_escape_string( "<repository> $RepositoryURL:$RepositoryDir </repository> <repository_url> $RepositoryWWWURL </repository_url>" )."'";

if( isset( $_POST[ "input" ] ) && ( $_POST[ "input" ] != "" ) )
{
 $Input = hexbin( $_POST[ "input" ] );
 if( strlen( $Input ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] )
  return( LGI_Error_Response( 54, $ErrorMsgs[ 54 ] ) );
 $InsertQuery .= ", input='".mysql_escape_string( $Input )."'";
}

if( isset( $_POST[ "output" ] ) && ( $_POST[ "output" ] != "" ) )
{
 $Output = hexbin( $_POST[ "output" ] );
 if( strlen( $Output ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] )
  return( LGI_Error_Response( 55, $ErrorMsgs[ 55 ] ) );
 $InsertQuery .= ", output='".mysql_escape_string( $Output )."'";
}

if( isset( $_POST[ "read_access" ] ) && ( $_POST[ "read_access" ] != "" ) )
{
 if( strlen( $_POST[ "read_access" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 51, $ErrorMsgs[ 51 ] ) );
 $ReadAccess = $JobOwners.", ".NormalizeCommaSeparatedField( $_POST[ "read_access" ], "," );
}
else
 $ReadAccess = $JobOwners;

if( isset( $_POST[ "write_access" ] ) && ( $_POST[ "write_access" ] != "" ) )
{
 if( strlen( $_POST[ "write_access" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 52, $ErrorMsgs[ 52 ] ) );
 $WriteAccess = $JobOwners.", ".NormalizeCommaSeparatedField( $_POST[ "write_access" ], "," );
}
else
 $WriteAccess = $JobOwners;

// add admins groups to read and write access lists...
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] != "" ) ) $WriteAccess = $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ].", ".$WriteAccess;
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] != "" ) ) $ReadAccess = $Config[ "LGI_ADMIN_GROUP_FOR_READ" ].", ".$ReadAccess;

$ReadAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $ReadAccess, "," ) );
$WriteAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $WriteAccess, "," ) );
$InsertQuery .= ", read_access='".$ReadAccess."', write_access='".$WriteAccess."'";

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// insert a lock into the running_locks table for this inserted job...
$queryresult = mysql_query( "INSERT INTO running_locks SET job_id=".$JobSpecs->job_id.",resource_id=".$ResourceData->resource_id.",lock_time=UNIX_TIMESTAMP(),session_id=".$ResourceData->SessionID );

// get repository content...
$RepoContent = GetRepositoryContent( NormalizeString( Parse_XML( $JobSpecs->job_specifics, "repository", $Attributes ) ) );

// schedule default scheduler event...
ScheduleEvent();

// and build response for this job submition...
$Response = " <resource> ".XML_Escape( $ResourceData->resource_name )." </resource> <resource_url> ".XML_Escape( $ResourceData->url )." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server> <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";
$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";
$Response .= " <job> <job_id> ".$JobSpecs->job_id." </job_id>";
$Response .= " <target_resources> ".XML_Escape( $JobSpecs->target_resources )." </target_resources>";
$Response .= " <owners> ".XML_Escape( $JobSpecs->owners )." </owners>";
$Response .= " <application> ".XML_Escape( $JobSpecs->application )." </application>";
$Response .= " <state> ".XML_Escape( $JobSpecs->state )." </state>";
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>";
$Response .= " <repository_content> ".$RepoContent." </repository_content>";
$Response .= " <input> ".binhex( $JobSpecs->input )." </input>";
$Response .= " <output> ".binhex( $JobSpecs->output )." </output>";
$Response .= " <read_access> ".XML_Escape( $JobSpecs->read_access )." </read_access>";
$Response .= " <write_access> ".XML_Escape( $JobSpecs->write_access )." </write_access>";
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> </job>";

// return the response...
return( LGI_Response( $Response ) );
?>

