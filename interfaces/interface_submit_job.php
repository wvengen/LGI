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
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// check if other compulsory post variables were set...
if( !isset( $_POST[ "application" ] ) || ( $_POST[ "application" ] == "" ) )
 return( LGI_Error_Response( 18, $ErrorMsgs[ 18 ], "" ) );
else
 $JobApplication = $_POST[ "application" ];

if( !isset( $_POST[ "target_resources" ] ) || ( $_POST[ "target_resources" ] == "" ) )
 return( LGI_Error_Response( 27, $ErrorMsgs[ 27 ], "" ) );
else
 $JobTargetResources = NormalizeCommaSeparatedField( $_POST[ "target_resources" ], "," );

// check if any of posted target resources is allowed...
$Resources = CommaSeparatedField2Array( $JobTargetResources, "," );

$NewTargetResourceList = "";
$FoundResourceFlag = 0;

for( $i = 1; $i <= $Resources[ 0 ]; $i++ )
 if( Interface_Is_Target_Resource_Known( $Resources[ $i ] ) )
 {
  $NewTargetResourceList .= ", ".$Resources[ $i ];
  $FoundResourceFlag = 1;
 }

if( !$FoundResourceFlag )
 return( LGI_Error_Response( 28, $ErrorMsgs[ 28 ], "" ) );

$JobTargetResources = substr( $NewTargetResourceList, 2 );

// check if any of the posted combinations of user+groups or just the user is allowed to submit a job...
$GroupsArray = CommaSeparatedField2Array( $JobGroups, "," );

if( !isset( $_POST[ "groups" ] ) || ( $_POST[ "groups" ] == "" ) )
{
 $GroupsArray[ $GroupsArray[ 0 ] + 1 ] = $JobUSer;         // add user personal group at the end of the list...
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
    Interface_Count_Owners_Jobs_In_Queue( $JobUser, $JobApplication, $TotalNrOfJobs, $NrOfRunningJobs );
   else
    Interface_Count_Owners_Jobs_In_Queue( $GroupsArray[ $i ], $JobApplication, $TotalNrOfJobs, $NrOfRunningJobs );

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
 return( LGI_Error_Response( 33, $ErrorMsgs[ 33 ], "" ) );

// now determine the owners and read_access fields based on the possibly posted data and the user+groups data...
if( isset( $_POST[ "read_access" ] ) && ( $_POST[ "read_access" ] != "" ) )
{
 if( isset( $_POST[ "owners" ] ) && ( $_POST[ "owners" ] != "" ) )
 {
  // if owners and read_access were posted...
  $JobOwners = $JobUser.", ".$_POST[ "owners" ];
  $JobReadAccess = $JobUser.", ".$JobGroups.", ".$_POST[ "read_access" ].", ".$_POST[ "owners" ];
 }
 else
 {
  // if only read_access was posted...
  $JobOwners = $JobUser;
  $JobReadAccess = $JobUser.", ".$JobGroups.", ".$_POST[ "read_access" ];
 }
}
else
{
 if( isset( $_POST[ "owners" ] ) && ( $_POST[ "owners" ] != "" ) )
 {
  // if only owners was posted...
  $JobOwners = $JobUser.", ".$_POST[ "owners" ];
  $JobReadAccess = $JobUser.", ".$JobGroups.", ".$_POST[ "owners" ];
 }
 else
 {
  // if neither owners nor read_access were posted...
  $JobOwners = $JobUser;
  $JobReadAccess = $JobUser.", ".$JobGroups;
 }
}

// make sure that future REGEXP's do work...
$JobOwners = mysql_escape_string( NormalizeCommaSeparatedField( $JobOwners, "," ) );
$JobReadAccess = mysql_escape_string( NormalizeCommaSeparatedField( $JobReadAccess, "," ) );
$JobApplication = mysql_escape_string( $JobApplication );
$JobTargetResources = mysql_escape_string( $JobTargetResources );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='queued', application='".$JobApplication."', owners='".$JobOwners."', read_access='".$JobReadAccess."', target_resources='".$JobTargetResources."', lock_state=0, state_time_stamp=UNIX_TIMESTAMP()";

if( isset( $_POST[ "job_specifics" ] ) && ( $_POST[ "job_specifics" ] != "" ) )
 $InsertQuery .= ", job_specifics='".mysql_escape_string( $_POST[ "job_specifics" ] )."'";

if( isset( $_POST[ "input" ] ) && ( $_POST[ "input" ] != "" ) )
 $InsertQuery .= ", input='".mysql_escape_string( hexbin( $_POST[ "input" ] ) )."'";

if( isset( $_POST[ "output" ] ) && ( $_POST[ "output" ] != "" ) )
 $InsertQuery .= ", output='".mysql_escape_string( hexbin( $_POST[ "output" ] ) )."'";

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// and build response for this job submition...
$Response =  " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <project_server> ".Get_Server_URL()." </project_server>";
$Response .= " <user> ".$JobUser." </user> <groups> ".$JobGroups." </groups>";
$Response .= " <job> <job_id> ".$JobSpecs->job_id." </job_id>";
$Response .= " <target_resources> ".$JobSpecs->target_resources." </target_resources>";
$Response .= " <owners> ".$JobSpecs->owners." </owners>";
$Response .= " <read_access> ".$JobSpecs->read_access." </read_access>";
$Response .= " <application> ".$JobSpecs->application." </application>";
$Response .= " <state> ".$JobSpecs->state." </state>";
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>";
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics>";
$Response .= " <input> ".binhex( $JobSpecs->input )." </input>";
$Response .= " <output> ".binhex( $JobSpecs->output )." </output> </job>";

// return the response...
return( LGI_Response( $Response, "" ) );
?>

