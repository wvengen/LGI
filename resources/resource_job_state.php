<?php

// []--------------------------------------------------------[]
//  |                resource_job_state.php                  |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Get job state from project DB...           |
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
$ResourceData = Resource_Verify( $_POST[ "project" ] );

// check if compulsory post variable was set...
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || !ctype_digit( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ] ) );
else
{
 if( strlen( $_POST[ "job_id" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 47, $ErrorMsgs[ 47 ] ) );
 $JobId = $_POST[ "job_id" ];
}

// query for the job specs... 
$JobQuery = mysql_query( "SELECT * FROM job_queue WHERE job_id=".$JobId );
if( $JobQuery ) 
 $NrRows =  mysql_num_rows( $JobQuery );
else
 $NrRows = 0;

if( $NrRows != 1 )
{
 if( $NrRows ) mysql_free_result( $JobQuery );
 return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ] ) );
}

$JobSpecs = mysql_fetch_object( $JobQuery );
mysql_free_result( $JobQuery );

// check if this resource is allowed to get the state of this job...
if( !FoundInCommaSeparatedField( $JobSpecs->target_resources, $ResourceData->resource_name, "," ) )
 return( LGI_Error_Response( 24, $ErrorMsgs[ 24 ] ) );

// now update the pulse...
$JobQuery = mysql_query( "UPDATE job_queue SET daemon_pulse=UNIX_TIMESTAMP() WHERE job_id=".$JobId );

// build response for this job...
$Response = " <resource> ".XML_Escape( $ResourceData->resource_name )." </resource> <resource_url> ".XML_Escape( $ResourceData->url )." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server> <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";
$Response .= " <job> <job_id> ".$JobSpecs->job_id." </job_id>"; 
$Response .= " <target_resources> ".XML_Escape( $JobSpecs->target_resources )." </target_resources>"; 
$Response .= " <owners> ".XML_Escape( $JobSpecs->owners )." </owners>"; 
$Response .= " <application> ".XML_Escape( $JobSpecs->application )." </application>"; 
$Response .= " <state> ".XML_Escape( $JobSpecs->state )." </state>"; 
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp>"; 
$Response .= " <read_access> ".XML_Escape( $JobSpecs->read_access )." </read_access>"; 
$Response .= " <write_access> ".XML_Escape( $JobSpecs->write_access )." </write_access>"; 
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics> </job>"; 

// return the response...
return( LGI_Response( $Response ) );
?>

