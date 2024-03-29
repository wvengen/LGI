<?php

// []--------------------------------------------------------[]
//  |                resource_lock_job.php                   |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Lock a job in project DB...                |
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
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || !ctype_digit( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ] ) );
else
{
 if( strlen( $_POST[ "job_id" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 47, $ErrorMsgs[ 47 ] ) );
 $JobId = $_POST[ "job_id" ];
}

// now try to lock the job... return an error response if we failed somehow... we do need to check everything now...
if( Resource_Lock_Job( $ResourceData, $JobId, True ) )
 return( LGI_Error_Response( 22, $ErrorMsgs[ 22 ] ) ); 

// query for the lock specs...
$LockQuery = mysql_query( "SELECT lock_id,job_id,lock_time,session_id,resource_id FROM running_locks WHERE job_id=".$JobId );
$LockSpecs = mysql_fetch_object( $LockQuery );
mysql_free_result( $LockQuery );

// build response for this lock...
$Response = " <resource> ".XML_Escape( $ResourceData->resource_name )." </resource> <resource_url> ".XML_Escape( $ResourceData->url )." </resource_url>";
$Response .= " <resource_capabilities> ".$ResourceData->resource_capabilities." </resource_capabilities>";
$Response .= " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server> <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";
$Response .= " <session_id> ".$ResourceData->SessionID." </session_id>";
$Response .= " <job_id> ".$JobId." </job_id>";
$Response .= " <lock> <lock_id> ".$LockSpecs->lock_id." </lock_id> <job_id> ".$LockSpecs->job_id." </job_id>";
$Response .= " <lock_time> ".$LockSpecs->lock_time." </lock_time> <session_id> ".$LockSpecs->session_id." </session_id>";
$Response .= " <resource_id> ".$LockSpecs->session_id." </resource_id> </lock>";

// return the response...
return( LGI_Response( $Response ) );
?>

