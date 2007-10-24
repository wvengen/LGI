<?php

// []--------------------------------------------------------[]
//  |                resource_request_work.php               |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Return work from project  DB...            |
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
require_once( '../inc/Resources.inc' );

global $Config;
global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], $_POST[ "session_id" ] );

// check if this call is valid...
if( Resource_Verify_Session( $ResourceData ) )
 return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ] ) );

// check if compulsory post variable was set...
if( !isset( $_POST[ "application" ] ) || ( $_POST[ "application" ] == "" ) )
 return( LGI_Error_Response( 18, $ErrorMsgs[ 18 ] ) );
else
{
 if( strlen( $_POST[ "application" ] ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 46, $ErrorMsgs[ 46 ] ) );
 $Application = $_POST[ "application" ];
}

// check if non-compulsory posts were set...
if( isset( $_POST[ "start" ] ) && ( $_POST[ "start" ] != "" ) && is_numeric( $_POST[ "start" ] ) )
{
 if( strlen( $_POST[ "start" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 48, $ErrorMsgs[ 48 ] ) );
 $JobIdStart = $_POST[ "start" ];
}
else 
 $JobIdStart = 0;

if( isset( $_POST[ "limit" ] ) && ( $_POST[ "limit" ] != "" ) && is_numeric( $_POST[ "limit" ] ) )
{
 if( strlen( $_POST[ "limit" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 49, $ErrorMsgs[ 49 ] ) );
 $JobIdLimit = $_POST[ "limit" ];
}
else
 $JobIdLimit = mysql_escape_string( $Config[ "DEFAULT_WORK_REQUEST_LIMIT" ] );

// check if this resource has any jobs locked...
if( Resource_Check_For_Job_Locks( $ResourceData ) != 0 )
 return( LGI_Error_Response( 17, $ErrorMsgs[ 17 ] ) ); 

// header of the response...
$Response = "<start> ".$JobIdStart." </start> <limit> ".$JobIdLimit." </limit>";
$Response .= " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";
$Response .= " <application> ".$Application." </application>";

// start the work query...
$RegExpResource = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( $ResourceData->resource_name, "," ) );
$RegExpAny = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( "any", "," ) );
$Application = mysql_escape_string( $Application );

$TheWorkQuery = mysql_query( "SELECT job_id FROM job_queue WHERE application='".$Application."' AND state='queued' AND lock_state=0 AND ( target_resources REGEXP '".$RegExpResource."' OR target_resources REGEXP '".$RegExpAny."' ) ORDER BY state_time_stamp LIMIT ".$JobIdLimit." OFFSET ".$JobIdStart );

$NrOfPossibleJobs = mysql_num_rows( $TheWorkQuery );

// now if there is work...
if( $NrOfPossibleJobs >= 1 )  
{
 $ActualNrOfJobs = 0;
 $ResponseJobs = "";

 // try and lock the jobs...
 for( $job = 0; $job < $NrOfPossibleJobs; $job++ )
 {
  $JobId = mysql_fetch_object( $TheWorkQuery );        

  if( !Resource_Lock_Job( $ResourceData, $JobId->job_id ) )    
  {
   // we were able to get the lock... include this job into the response...
   $JobQuery = mysql_query( "SELECT job_id,target_resources,owners,read_access,state_time_stamp,job_specifics FROM job_queue WHERE job_id=".$JobId->job_id );
   $JobSpecs = mysql_fetch_object( $JobQuery );

   // build job record for this job...
   $ActualNrOfJobs += 1; 
   $ResponseJobs .= " <job number='".$ActualNrOfJobs."'> <job_id> ".$JobSpecs->job_id." </job_id>"; 
   $ResponseJobs .= " <target_resources> ".$JobSpecs->target_resources." </target_resources>"; 
   $ResponseJobs .= " <owners> ".$JobSpecs->owners." </owners>"; 
   $ResponseJobs .= " <read_access> ".$JobSpecs->read_access." </read_access>"; 
   $ResponseJobs .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>"; 
   $ResponseJobs .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> </job>"; 

   mysql_free_result( $JobQuery );
  } 
 }

 // free query and make final response... 
 mysql_free_result( $TheWorkQuery );
 $Response .= " <number_of_jobs> ".$ActualNrOfJobs." </number_of_jobs>".$ResponseJobs;
} 
else
{
 // there was no work...
 mysql_free_result( $TheWorkQuery );
 $Response .= " <number_of_jobs> 0 </number_of_jobs>";
}

// return the response...
return( LGI_Response( $Response ) );
?>

