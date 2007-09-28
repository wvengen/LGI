<?php

// []--------------------------------------------------------[]
//  |           resource_request_job_details.php             |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Get job details from project DB...         |
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
$ResourceData = Resource_Verify( $_POST[ "project" ], true );

// check if compulsory post variable was set...
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || ! is_numeric( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ], "" ) );
else
 $JobId = $_POST[ "job_id" ];

// check if this call is valid...
if( !$ResourceData->active )
  return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ], "" ) );

// check if this resource has this job locked...
if( !Resource_Has_Job_Locked( $ResourceData, $JobId ) )
 return( LGI_Error_Response( 20, $ErrorMsgs[ 20 ], "" ) ); 

// query for the job specs...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=".$JobId );
$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// build response for this job...
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
$Response .= " <output> ".binhex( $JobSpecs->output )." </output> </job> "; 

// return the response...
return( LGI_Response( $Response, "" ) );
?>

