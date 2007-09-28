<?php

// []--------------------------------------------------------[]
//  |                resource_unlock_job.php                 |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Unlock a job in  project DB...             |
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

global $ErrorMsgs;

// check if resource is known to the project and certified correctly...
$ResourceData = Resource_Verify( $_POST[ "project" ], true );

// check if compulsory post variable was set...
if( !isset( $_POST[ "job_id" ] ) || ( $_POST[ "job_id" ] == "" ) || !is_numeric( $_POST[ "job_id" ] ) )
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ], "" ) );
else
 $JobId = $_POST[ "job_id" ];

// check if this call is valid...
if( !$ResourceData->active )
  return( LGI_Error_Response( 16, $ErrorMsgs[ 16 ], "" ) );

// check if we had the job locked...
if( !Resource_Has_Job_Locked( $ResourceData, $JobId ) )
 return( LGI_Error_Response( 14, $ErrorMsgs[ 14 ], "" ) ); 

// query for the lock specs...
$LockQuery = mysql_query( "SELECT lock_id,job_id,lock_time FROM running_locks WHERE job_id=".$JobId );
$LockSpecs = mysql_fetch_object( $LockQuery );

// now try to unlock the job... return an error response if we failed somehow...
if( Resource_UnLock_Job( $ResourceData, $JobId ) )
{
 mysql_free_result( $LockQuery );
 return( LGI_Error_Response( 23, $ErrorMsgs[ 23 ], "" ) ); 
}

// build response for this lock...
$Response = " <resource> ".$ResourceData->resource_name." </resource> <resource_url> ".$ResourceData->url." </resource_url>";
$Response .= " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <this_project_server> ".Get_Server_URL()." </this_project_server>";
$Response .= " <resource_active> ".$ResourceData->active." </resource_active>";
$Response .= " <job_id> ".$JobId." </job_id> ";
$Response .= " <lock> <lock_id> ".$LockSpecs->lock_id." </lock_id> <job_id> ".$LockSpecs->job_id." </job_id>";
$Response .= " <unlock_time> ".$LockSpecs->lock_time." </unlock_time> </lock> ";

mysql_free_result( $LockQuery );

// return the response...
return( LGI_Response( $Response, "" ) );
?>

