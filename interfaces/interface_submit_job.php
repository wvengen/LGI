<?php

// []--------------------------------------------------------[]
//  |                interface_submit_job.php                |
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
require_once( '../inc/Repository.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// check if other compulsory post variables were set and are correct...
if( !isset( $_POST[ "application" ] ) || ( $_POST[ "application" ] == "" ) )
 return( LGI_Error_Response( 18, $ErrorMsgs[ 18 ] ) );
else
{
 if( strlen( $_POST[ "application" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 46, $ErrorMsgs[ 46 ] ) );
 $JobApplication = NormalizeString( $_POST[ "application" ] );
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
 if( Interface_Is_Target_Resource_Known( $Resources[ $i ], $JobApplication ) )
 {
  $NewTargetResourceList .= ", ".$Resources[ $i ];
  $FoundResourceFlag = 1;
 }
if( !$FoundResourceFlag )
 return( LGI_Error_Response( 28, $ErrorMsgs[ 28 ] ) );
$JobTargetResources = substr( $NewTargetResourceList, 2 );

// check if any of the posted combinations of user+groups or just the user is allowed to submit a job...
$GroupsArray = CommaSeparatedField2Array( $JobGroups, "," );

if( !isset( $_POST[ "groups" ] ) || ( $_POST[ "groups" ] == "" ) )
{
 $GroupsArray[ $GroupsArray[ 0 ] + 1 ] = $JobUser;         // add user personal group at the end of the list...
 $GroupsArray[ 0 ]++;
}

$NewGroupsList = "";
$FoundPossibleGroup = 0;

for( $i = 1; $i <= $GroupsArray[ 0 ]; $i++ )
{
 $LimitType = Interface_User_In_Group_Is_Allowed_To_Submit( $JobUser, $GroupsArray[ $i ], $JobApplication, $NumberOfJobsAllowed );

 if( $LimitType )           // is allowed to submit a job? 
 {
  if( $NumberOfJobsAllowed )           // if there is a limit set for this owner, check it...
  {

   if( $LimitType == 1 )          // was it a limit defined from the user tables or the group tables...
    Interface_Count_Jobs_In_Queue( $JobUser, $JobApplication, $TotalNrOfJobs, $NrOfRunningJobs );
   else
    Interface_Count_Jobs_In_Queue( $GroupsArray[ $i ], $JobApplication, $TotalNrOfJobs, $NrOfRunningJobs );

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
 $JobGroups = substr( $NewGroupsList, 2 );
else
 return( LGI_Error_Response( 33, $ErrorMsgs[ 33 ] ) );

// now determine the read_access and write_access fields based on the possibly posted data and the user+groups data...
if( isset( $_POST[ "read_access" ] ) && ( $_POST[ "read_access" ] != "" ) )
{
 if( strlen( $_POST[ "read_access" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 51, $ErrorMsgs[ 51 ] ) );

 // check perhaps if read_access is part of owners=user+groups allowed to submit?

 $JobReadAccess = $JobUser.", ".NormalizeCommaSeparatedField( $_POST[ "read_access" ], "," );
}
else
 $JobReadAccess = $JobUser;

if( isset( $_POST[ "write_access" ] ) && ( $_POST[ "write_access" ] != "" ) )
{
 if( strlen( $_POST[ "write_access" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 52, $ErrorMsgs[ 52 ] ) );
 
 // check perhaps if write_access is part of owners=user+groups allowed to submit?

 $JobWriteAccess = $JobUser.", ".NormalizeCommaSeparatedField( $_POST[ "write_access" ], "," );
}
else
 $JobWriteAccess = $JobUser;

// add admins groups to read and write access lists...
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ] != "" ) ) $JobWriteAccess = $Config[ "LGI_ADMIN_GROUP_FOR_WRITE" ].", ".$JobWriteAccess;
if( isset( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] ) && ( $Config[ "LGI_ADMIN_GROUP_FOR_READ" ] != "" ) ) $JobReadAccess = $Config[ "LGI_ADMIN_GROUP_FOR_READ" ].", ".$JobReadAccess;

// create the job respository directory...
CreateRepository( $RepositoryDir, $RepositoryURL, $RepositoryIDFile, $JobWriteAccess );
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

// make sure that future REGEXP's do work...
$JobOwners = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $JobUser.", ".$JobGroups, "," ) );
$JobReadAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $JobReadAccess, "," ) );
$JobWriteAccess = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $JobWriteAccess, "," ) );
$JobApplication = mysql_escape_string( $JobApplication );
$JobTargetResources = mysql_escape_string( RemoveDoubleEntriesFromCommaSeparatedField( $JobTargetResources, "," ) );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='queued', application='".$JobApplication."', owners='".$JobOwners."', read_access='".$JobReadAccess."', write_access='".$JobWriteAccess."', target_resources='".$JobTargetResources."', lock_state=0, state_time_stamp=UNIX_TIMESTAMP(), priority=UNIX_TIMESTAMP()";

if( isset( $_POST[ "job_specifics" ] ) && ( $_POST[ "job_specifics" ] != "" ) )
{
 if( strlen( $_POST[ "job_specifics" ] ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] )
  return( LGI_Error_Response( 53, $ErrorMsgs[ 53 ] ) );
 if( XML_Sanity_Check( $_POST[ "job_specifics" ] ) )
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

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// get repository content...
$RepoContent = GetRepositoryContent( NormalizeString( Parse_XML( $JobSpecs->job_specifics, "repository", $Attributes ) ) );

// schedule default scheduler event...
ScheduleEvent();

// and build response for this job submition...
$Response =  " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server> <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";
$Response .= " <user> ".XML_Escape( $JobUser )." </user> <groups> ".XML_Escape( $JobGroups )." </groups>";
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

