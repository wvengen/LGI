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

require_once( '../inc/Resources.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

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
 $Application = NormalizeString( $_POST[ "application" ] );
}

// check if non-compulsory posts were set...
if( isset( $_POST[ "start" ] ) && ( $_POST[ "start" ] != "" ) && ctype_digit( $_POST[ "start" ] ) )
{
 if( strlen( $_POST[ "start" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 48, $ErrorMsgs[ 48 ] ) );
 $JobIdStart = $_POST[ "start" ];
}
else 
 $JobIdStart = 0;

if( isset( $_POST[ "limit" ] ) && ( $_POST[ "limit" ] != "" ) && ctype_digit( $_POST[ "limit" ] ) )
{
 if( strlen( $_POST[ "limit" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 49, $ErrorMsgs[ 49 ] ) );
 $JobIdLimit = $_POST[ "limit" ];
}
else
 $JobIdLimit = $Config[ "DEFAULT_WORK_REQUEST_LIMIT" ];
if( (int)( $JobIdLimit ) > $Config[ "MAX_WORK_REQUEST_LIMIT" ] ) $JobIdLimit = $Config[ "MAX_WORK_REQUEST_LIMIT" ];

// check if this resource has any jobs locked...
if( Resource_Check_For_Job_Locks( $ResourceData ) != 0 )
 return( LGI_Error_Response( 17, $ErrorMsgs[ 17 ] ) ); 

// header of the response...
$Response = "<start> ".$JobIdStart." </start> <limit> ".$JobIdLimit." </limit>";
$Response .= " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";
$Response .= " <application> ".$Application." </application>";

if( (int)( $JobIdLimit ) <= 0 )     // see if we want a job-count only for pilotjob scheduling purposes...
{
 $RegExpResource = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( $ResourceData->resource_name, "," ) );
 $RegExpAny = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( "any", "," ) );
 $Application = mysql_escape_string( $Application );

 $TheWorkQuery = mysql_query( "SELECT COUNT(job_id) AS count FROM job_queue USE INDEX (resource_index) WHERE application='".$Application."' AND state='queued' AND ( target_resources REGEXP '".$RegExpResource."' OR target_resources REGEXP '".$RegExpAny."'" );
 $TheData = mysql_fetch_object( $TheWorkQuery );
 mysql_free_result( $TheWorkQuery );

 $NrOfPossibleJobs = $TheData -> count;
 $Response .= " <number_of_jobs> $NrOfPossibleJobs </number_of_jobs>"; 
 return( LGI_Response( $Response ) );
}

if( (int)( $JobIdStart ) >= 16 * (int)( $JobIdLimit ) )      // otherwise, make sure we have not too many requests from a resource...
 $NrOfPossibleJobs = 0;
else
{
 // start the work query...
 $RegExpResource = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( $ResourceData->resource_name, "," ) );
 $RegExpAny = mysql_escape_string( MakeRegularExpressionForCommaSeparatedField( "any", "," ) );
 $Application = mysql_escape_string( $Application );

 $TheWorkQuery = mysql_query( "SELECT job_id FROM job_queue USE INDEX (resource_index) WHERE application='".$Application."' AND state='queued' AND lock_state=0 AND ( target_resources REGEXP '".$RegExpResource."' OR target_resources REGEXP '".$RegExpAny."' ) LIMIT ".$JobIdLimit." OFFSET ".$JobIdStart );

 if( $TheWorkQuery )
  $NrOfPossibleJobs = mysql_num_rows( $TheWorkQuery );
 else
  $NrOfPossibleJobs = 0;
}

// now if there was work...
if( $NrOfPossibleJobs >= 1 )  
{
 $ActualNrOfJobs = 0;
 $ResponseJobs = "";

 // try and lock the jobs...
 for( $job = 0; $job < $NrOfPossibleJobs; $job++ )
 {
  $JobId = mysql_fetch_object( $TheWorkQuery );        

  if( !Resource_Lock_Job( $ResourceData, $JobId->job_id, False ) )   // we will do the extra checks so avoid some queries...
  {
   // we were able to get the lock... include this job into the response...
   $JobQuery = mysql_query( "SELECT job_id,target_resources,owners,read_access,write_access,state_time_stamp,job_specifics,state,lock_state FROM job_queue WHERE job_id=".$JobId->job_id );

   if( $JobQuery )
   {
    $JobSpecs = mysql_fetch_object( $JobQuery );
    mysql_free_result( $JobQuery );

    // double check if state is still 'queued' and locked as another thread could have taken it in the mean time...
    if( ( $JobSpecs->lock_state == 1 ) && ( $JobSpecs->state == "queued" ) && FoundInCommaSeparatedField( $JobSpecs->target_resources, $ResourceData->resource_name, "," ) )
    {
     // build job record for this job...
     $ActualNrOfJobs += 1; 
     $ResponseJobs .= " <job number='".$ActualNrOfJobs."'> <job_id> ".$JobSpecs->job_id." </job_id>"; 
     $ResponseJobs .= " <target_resources> ".$JobSpecs->target_resources." </target_resources>"; 
     $ResponseJobs .= " <owners> ".$JobSpecs->owners." </owners>"; 
     $ResponseJobs .= " <read_access> ".$JobSpecs->read_access." </read_access>"; 
     $ResponseJobs .= " <write_access> ".$JobSpecs->write_access." </write_access>"; 
     $ResponseJobs .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>"; 
     $ResponseJobs .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> </job>"; 
    }
    else
     Resource_UnLock_Job( $ResourceData, $JobId->job_id, False ); 
   }
   else
    Resource_UnLock_Job( $ResourceData, $JobId->job_id, False ); 
  } 
 }

 // free query and make final response... 
 mysql_free_result( $TheWorkQuery );
 $Response .= " <number_of_jobs> ".$ActualNrOfJobs." </number_of_jobs>".$ResponseJobs;
} 
else
 $Response .= " <number_of_jobs> 0 </number_of_jobs>"; // there was no work...

// return the response...
return( LGI_Response( $Response ) );
?>

