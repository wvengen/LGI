<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Resources.inc' );

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], false );

// check if compulsory post variable was set...
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || !is_numeric( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ], "" ) );
else
 $JobId = $_POST[ "job_id" ];

// query for the job specs...
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=".$JobId );

if( mysql_num_rows( $JobQuery ) != 1 )
{
 mysql_free_result( $JobQuery );
 return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ], "" ) );
}

$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// check if this resource is allowed to get the state of this job...
if( !FoundInCommaSeparatedField( $JobSpecs->target_resources, $ResourceData->resource_name, "," ) )
 return( LGI_Error_Response( 24, $ErrorMsgs[ 24 ], "" ) );

// build response for this job...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .=" <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <resource_active> ".$ResourceData->active." </resource_active>";
$Response .= " <job> <job_id> ".$JobSpecs->job_id." </job_id> "; 
$Response .= " <target_resources> ".$JobSpecs->target_resources." </target_resources> "; 
$Response .= " <owners> ".$JobSpecs->owners." </owners> "; 
$Response .= " <read_access> ".$JobSpecs->read_access." </read_access> "; 
$Response .= " <application> ".$JobSpecs->application." </application> "; 
$Response .= " <state> ".$JobSpecs->state." </state> "; 
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp> "; 
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> </job> "; 

// return the response...
return( LGI_Response( $Response, "" ) );
?>

