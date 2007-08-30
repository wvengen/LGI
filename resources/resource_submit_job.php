<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Resources.inc' );
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ] );

// check if compulsory post variables were set...
if( !isset( $_POST[ "state" ] ) || ( $_POST[ "state" ] == "" ) )
 return( LGI_Error_Response( 25, $ErrorMsgs[ 25 ], "" ) );
else
 $JobState = $_POST[ "state" ];

if( !isset( $_POST[ "application" ] ) || ( $_POST[ "application" ] == "" ) )
 return( LGI_Error_Response( 18, $ErrorMsgs[ 18 ], "" ) );
else
 $JobApplication = $_POST[ "application" ];

if( !isset( $_POST[ "owners" ] ) || ( $_POST[ "owners" ] == "" ) )
 return( LGI_Error_Response( 26, $ErrorMsgs[ 26 ], "" ) );
else
 $JobOwners = NormalizeCommaSeparatedField( $_POST[ "owners" ], "," );

if( !isset( $_POST[ "target_resources" ] ) || ( $_POST[ "target_resources" ] == "" ) )
 return( LGI_Error_Response( 27, $ErrorMsgs[ 27 ], "" ) );
else
 $JobTargetResources = NormalizeCommaSeparatedField( $_POST[ "target_resources" ], "," );

// check if this call is valid...
if( !$ResourceData->active )
  return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ], "" ) );

// check if this resource has any jobs locked...
if( Resource_Check_For_Job_Locks( $ResourceData ) != 0 )
 return( LGI_Error_Response( 17, $ErrorMsgs[ 17 ], "" ) );

// check if any of posted target resources is allowed...
$Resources = CommaSeparatedField2Array( $JobTargetResources, "," );

$NewTargetResourceList = "";
$FoundResourceFlag = 0;

for( $i = 1; $i <= $Resources[ 0 ]; $i++ )
 if( Resource_Is_Target_Resource_Known( $Resources[ $i ] ) )
 {
  $NewTargetResourceList .= ", ".$Resources[ $i ];
  $FoundResourceFlag = 1;
 }

if( !$FoundResourceFlag )
 return( LGI_Error_Response( 28, $ErrorMsgs[ 28 ], "" ) );

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
 return( LGI_Error_Response( 29, $ErrorMsgs[ 29 ], "" ) );

$JobOwners = mysql_escape_string( substr( $NewOwnersList, 2 ) );
$JobState = mysql_escape_string( $JobState );
$JobApplication = mysql_escape_string( $JobApplication );
$JobTargetResources = mysql_escape_string( $JobTargetResources );

// start building the insert query based on all possible posted fields...
$InsertQuery = "INSERT INTO job_queue SET state='".$JobState."', application='".$JobApplication."', owners='".$JobOwners."', target_resources='".$JobTargetResources."', lock_state=1, state_time_stamp=UNIX_TIMESTAMP()";

if( isset( $_POST[ "job_specifics" ] ) && ( $_POST[ "job_specifics" ] != "" ) )
 $InsertQuery .= ", job_specifics='".mysql_escape_string( $_POST[ "job_specifics" ] )."'";

if( isset( $_POST[ "input" ] ) && ( $_POST[ "input" ] != "" ) )
 $InsertQuery .= ", input='".mysql_escape_string( hexbin( $_POST[ "input" ] ) )."'";

if( isset( $_POST[ "output" ] ) && ( $_POST[ "output" ] != "" ) )
 $InsertQuery .= ", output='".mysql_escape_string( hexbin( $_POST[ "output" ] ) )."'";

if( isset( $_POST[ "read_access" ] ) && ( $_POST[ "read_access" ] != "" ) )
 $InsertQuery .= ", read_access='".mysql_escape_string( NormalizeCommaSeparatedField( $_POST[ "read_access" ], "," ) )."'";
else
 $InsertQuery .= ", read_access='".mysql_escape_string( NormalizeCommaSeparatedField( $_POST[ "owners" ], "," ) )."'";

// insert the job into the database...
$queryresult = mysql_query( $InsertQuery );

// retrieve job just inserted...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=LAST_INSERT_ID()" );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// insert a lock into the running_locks table for this inserted job...
$queryresult = mysql_query( "INSERT INTO running_locks SET job_id=".$JobSpecs->job_id.",resource_id=".$ResourceData->resource_id.",lock_time=UNIX_TIMESTAMP()" );

// and build response for this job submition...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <resource_active> ".$ResourceData->active." </resource_active>";
$Response .= " <job> <job_id> ".$JobSpecs->job_id." </job_id> ";
$Response .= " <target_resources> ".$JobSpecs->target_resources." </target_resources> ";
$Response .= " <owners> ".$JobSpecs->owners." </owners> ";
$Response .= " <read_access> ".$JobSpecs->read_access." </read_access> ";
$Response .= " <application> ".$JobSpecs->application." </application> ";
$Response .= " <state> ".$JobSpecs->state." </state> ";
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp> ";
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> ";
$Response .= " <input> ".binhex( $JobSpecs->input )." </input> ";
$Response .= " <output> ".binhex( $JobSpecs->output )." </output> </job>";

// return the response...
return( LGI_Response( $Response, "" ) );
?>

