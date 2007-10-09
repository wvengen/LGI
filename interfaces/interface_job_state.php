<?php

// []--------------------------------------------------------[]
//  |                interface_job_state.php                 |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
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

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;
global $Config;

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// check if job_id was posted...
if( isset( $_POST[ "job_id" ] ) && ( $_POST[ "job_id" ] != "" ) && is_numeric( $_POST[ "job_id" ] ) )
{
 $JobId = $_POST[ "job_id" ];
 $DetailMode = 1;
}
else
 $DetailMode = 0;

// build response header...
$Response =  " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <project_server> ".Get_Server_URL()." </project_server>";
$Response .= " <user> ".$JobUser." </user> <groups> ".$JobGroups." </groups>";

// should we detail on a job or not...
if( !$DetailMode )
{
 // see if state field was posted...
 if( isset( $_POST[ "state" ] ) && ( $_POST[ "state" ] != "" ) )
  $JobState = $_POST[ "state" ];
 else
  $JobState = "any";

 $Response .= " <state> ".$JobState." </state>";

 // see if application field was posted...
 if( isset( $_POST[ "application" ] ) && ( $_POST[ "application" ] != "" ) )
  $JobApplication = $_POST[ "application" ];
 else
  $JobApplication = "any";

 $Response .= " <application> ".$JobApplication." </application>";

 // see if start field was posted...
 if( isset( $_POST[ "start" ] ) && ( $_POST[ "start" ] != "" ) && is_numeric( $_POST[ "start" ] ) ) 
  $JobStart = $_POST[ "start" ];
 else
  $JobStart = 0;

 // see if limit field was posted...
 if( isset( $_POST[ "limit" ] ) && ( $_POST[ "limit" ] != "" )  && is_numeric( $_POST[ "limit" ] ) )
  $JobLimit = $_POST[ "limit" ];
 else
  $JobLimit = $Config[ "DEFAULT_JOB_STATUS_LIMIT" ];

 $Response .= " <start> ".$JobStart." </start>";
 $Response .= " <limit> ".$JobLimit." </limit>";

 // now look for jobs of this user and any of the groups requested...
 $PossibleJobOwnersArray = CommaSeparatedField2Array( $JobUser.", ".$JobGroups.", any", "," );

 $NrOfJobsReported = 0;
 $JobResponses = "";
 $JobIdArray[ 0 ] = 0;

 // do the specific query for this owner array...
 $queryresult = mysql_query( Interface_Make_Query_For_Work_For_Owners( $PossibleJobOwnersArray, $JobState, $JobApplication, $JobStart, $JobLimit ) );
  
 $NrOfResults = mysql_num_rows( $queryresult );

 // we have some jobs in the table to report... 
 if( $NrOfResults >= 1 ) 
  for( $job = 0; ( $job < $NrOfResults ) && ( $NrOfJobsReported < $JobLimit ); $job++ )
  {
   $JobData = mysql_fetch_object( $queryresult );

   // check if we have already reported this job...
   if( Interface_Found_Id_In_List( $JobIdArray, $JobData->job_id ) ) continue;

   // add this found job into our report list...
   $JobIdArray[ 0 ]++;
   $JobIdArray[ $JobIdArray[ 0 ] ] = $JobData->job_id;

   // and build it's response...
   $NrOfJobsReported++;
   $JobResponses .= " <job number='".$NrOfJobsReported."'> <job_id> ".$JobData->job_id." </job_id>";
   $JobResponses .= " <state> ".$JobData->state." </state> <state_time_stamp> ".$JobData->state_time_stamp." </state_time_stamp>";
   $JobResponses .= " <target_resources> ".$JobData->target_resources." </target_resources>";
   $JobResponses .= " <owners> ".$JobData->owners." </owners>";
   $JobResponses .= " <read_access> ".$JobData->read_access." </read_access>";
   $JobResponses .= " <job_specifics> ".$JobData->job_specifics." </job_specifics>";
   $JobResponses .= " <application> ".$JobData->application." </application> </job>";
  }

 mysql_free_result( $queryresult );

 $Response .= " <number_of_jobs> ".$NrOfJobsReported." </number_of_jobs> ".$JobResponses;
}
else
{
 // do query for specific job number... make sure job is not locked...
 $JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobId );

 // now check if user+groups is allowed to get the data anyway...
 if( !Interface_Is_User_Allowed_To_Read_Job( $JobSpecs, $JobUser, $JobGroups ) )
  return( LGI_Error_Response( 34, $ErrorMsgs[ 34 ], "" ) );

 // finally send out the data in the response... 
 $Response .= " <number_of_jobs> 1 </number_of_jobs> <job number='1'>";
 $Response .= " <job_id> ".$JobSpecs->job_id." </job_id>";
 $Response .= " <target_resources> ".$JobSpecs->target_resources." </target_resources>";
 $Response .= " <owners> ".$JobSpecs->owners." </owners>";
 $Response .= " <read_access> ".$JobSpecs->read_access." </read_access>";
 $Response .= " <application> ".$JobSpecs->application." </application>";
 $Response .= " <state> ".$JobSpecs->state." </state>";
 $Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>";
 $Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics>";
 $Response .= " <input> ".binhex( $JobSpecs->input )." </input>";
 $Response .= " <output> ".binhex( $JobSpecs->output )." </output> </job>";
}

// return the response...
return( LGI_Response( $Response, "" ) );
?>

