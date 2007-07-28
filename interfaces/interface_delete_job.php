<?php

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
 $JobId = $_POST[ "job_id" ];
else
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ], "" ) );

// do query for specific job number... make sure job is not locked...
$JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobId );

// now check if user+groups is allowed to modify the data anyway...
if( !Interface_Is_User_Allowed_To_Modify_Job( $JobSpecs, $JobUser, $JobGroups ) )
 return( LGI_Error_Response( 35, $ErrorMsgs[ 35 ], "" ) );

// if the job can be actually removed...
if( ( $JobSpecs->state == 'queued' ) || ( $JobSpecs->state == 'finished' ) )
{
 Interface_Set_Spin_Lock_On_Job( $JobId );
 $queryresult = mysql_query( "DELETE FROM job_queue WHERE job_id=".$JobId );
 $JobState = "deleted";
}
else
{
 Interface_Set_Spin_Lock_On_Job( $JobId );
 $queryresult = mysql_query( "UPDATE job_queue SET state='aborting',state_time_stamp=UNIX_TIMESTAMP() WHERE job_id=".$JobId );
 Interface_Clear_Spin_Lock_On_Job( $JobId );
 $JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobId );
 $JobState = $JobSpecs->state;
}

// build response header...
$Response =  " <project> ".Get_Selected_MySQL_DataBase()." </project>";
$Response .= " <project_master_server> ".Get_Master_Server_URL()." </project_master_server> <project_server> ".$Config[ "SERVER_URL" ]." </project_server>";
$Response .= " <user> ".$JobUser." </user> <groups> ".$JobGroups." </groups>";
$Response .= " <number_of_jobs> 1 </number_of_jobs> <job number=1>";
$Response .= " <job_id> ".$JobSpecs->job_id." </job_id> ";
$Response .= " <target_resources> ".$JobSpecs->target_resources." </target_resources> ";
$Response .= " <application> ".$JobSpecs->application." </application> ";
$Response .= " <state> ".$JobState." </state> ";
$Response .= " <state_time_stamp> ".$JobSpecs->state_time_stamp." </state_time_stamp> </job> ";

// return the response...
return( LGI_Response( $Response, "" ) );
?>

