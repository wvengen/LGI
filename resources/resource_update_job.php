<?php

// []--------------------------------------------------------[]
//  |               resource_update_job.php                  |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Update job in project DB...                |
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
require_once( '../inc/Utils.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], $_POST[ "session_id" ] );

// check if this call is valid...
if( Resource_Verify_Session( $ResourceData ) )
  return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ], "" ) );

// check if compulsory post variable was set...
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || !is_numeric( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ], "" ) );
else
 $JobId = $_POST[ "job_id" ];

// check if we had the job locked...
if( !Resource_Has_Job_Locked( $ResourceData, $JobId ) )
 return( LGI_Error_Response( 14, $ErrorMsgs[ 14 ], "" ) ); 

// start building the update query based on the posted fields...
$UpdateQuery = "UPDATE job_queue SET job_id=".$JobId.", state_time_stamp=UNIX_TIMESTAMP()";

if( isset( $_POST[ "state" ] ) && ( $_POST[ "state" ] != "" ) )
 $UpdateQuery .= ", state='".mysql_escape_string( $_POST[ "state" ] )."'";

if( isset( $_POST[ "target_resources" ] ) && ( $_POST[ "target_resources" ] != "" ) )
{
 // only do the allowed target_recources...
 $TargetResourcesArray = CommaSeparatedField2Array( $_POST[ "target_resources" ], "," );

 $AllowedTargetResources = "";
 $FoundMatch = 0;

 for( $i = 1; $i <= $TargetResourcesArray[ 0 ]; $i++ )
  if( Resource_Is_Target_Resource_Known( $TargetResourcesArray[ $i ] ) )
  {
   $AllowedTargetResources .= ", ".$TargetResourcesArray[ $i ];
   $FoundMatch = 1;
  }

 if( $FoundMatch )
  $UpdateQuery .= ", target_resources='".mysql_escape_string( substr( $AllowedTargetResources, 2 ) )."'";
 else
  return( LGI_Error_Response( 28, $ErrorMsgs[ 28 ], "" ) );
}

if( isset( $_POST[ "input" ] ) && ( $_POST[ "input" ] != "" ) )
 $UpdateQuery .= ", input='".mysql_escape_string( hexbin( $_POST[ "input" ] ) )."'";

if( isset( $_POST[ "output" ] ) && ( $_POST[ "output" ] != "" ) )
 $UpdateQuery .= ", output='".mysql_escape_string( hexbin( $_POST[ "output" ] ) )."'";

if( isset( $_POST[ "job_specifics" ] ) && ( $_POST[ "job_specifics" ] != "" ) )
 $UpdateQuery .= ", job_specifics='".mysql_escape_string( $_POST[ "job_specifics" ] )."'";

$UpdateQuery .= " WHERE job_id=".$JobId;

// now update the job record...
$queryresult = mysql_query( $UpdateQuery );

// query for the job specs that are currently stored in DB...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=".$JobId );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// build response for this job...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";
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

