<?php

// []--------------------------------------------------------[]
//  |                interface_delete_job.php                |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Remove job from project DB...              |
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
require_once( '../inc/Repository.inc' );

// check session existance... if we happen to be hit through by browser that has a session running, we error!
session_start();
if( isset( $_SESSION[ "sid" ] ) ) return( LGI_Error_Response( 67, $ErrorMsgs[ 67 ] ) );
session_destroy();

// check if resource is known to the project and certified correctly...
Interface_Verify( $_POST[ "project" ], $_POST[ "user" ], $_POST[ "groups" ] );

// get posted field that are there by default...
$JobGroups = NormalizeCommaSeparatedField( $_POST[ "groups" ], "," );
$JobUser = $_POST[ "user" ];

// check if job_id was posted...
if( isset( $_POST[ "job_id" ] ) && ( $_POST[ "job_id" ] != "" ) && ctype_digit( $_POST[ "job_id" ] ) )
{
 if( strlen( $_POST[ "job_id" ] ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 47, $ErrorMsgs[ 47 ] ) );
 $JobId = $_POST[ "job_id" ];
}
else
 return( LGI_Error_Response( 19, $ErrorMsgs[ 19 ] ) );

// do query for specific job number... make sure job is not locked...
$JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobId );

// now check if user+groups is allowed to modify the data anyway...
if( !Interface_Is_User_Allowed_To_Modify_Job( $JobSpecs, $JobUser, $JobGroups ) )
 return( LGI_Error_Response( 35, $ErrorMsgs[ 35 ] ) );

// if the job can be actually removed...
if( ( $JobSpecs->state == 'queued' ) || ( $JobSpecs->state == 'finished' ) || ( $JobSpecs->state == 'aborted' ) )
{
 Interface_Set_Spin_Lock_On_Job( $JobId );   // lock job or send back error response...
 $RepositoryURL = NormalizeString( Parse_XML( $JobSpecs -> job_specifics, "repository", $Attributes ) );
 if( $RepositoryURL != "" ) DeleteRepository( $RepositoryURL );
 mysql_query( "DELETE FROM job_queue WHERE job_id=".$JobId );
 Interface_Clear_Spin_Lock_On_Job( $JobId );   // unlock job again...
 $JobState = "deleted";
 $JobStateTimeStamp = time();
}
else
{
 Interface_Set_Spin_Lock_On_Job( $JobId );     // lock job or return error...
 $queryresult = mysql_query( "UPDATE job_queue SET state='aborting',state_time_stamp=UNIX_TIMESTAMP() WHERE job_id=".$JobId );
 Interface_Clear_Spin_Lock_On_Job( $JobId );   // unlock job again...
 $JobSpecs = Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobId );
 $JobState = $JobSpecs->state;
 $JobStateTimeStamp = $JobSpecs->state_time_stamp;
}

// schedule default scheduler event...
ScheduleEvent();

// build response header...
$Response =  " <project> ".XML_Escape( Get_Selected_MySQL_DataBase() )." </project>";
$Response .= " <project_master_server> ".XML_Escape( Get_Master_Server_URL() )." </project_master_server> <this_project_server> ".XML_Escape( Get_Server_URL() )." </this_project_server>";
$Response .= " <user> ".XML_Escape( $JobUser )." </user> <groups> ".XML_Escape( $JobGroups )." </groups>";
$Response .= " <number_of_jobs> 1 </number_of_jobs> <job number='1'>";
$Response .= " <job_id> ".$JobSpecs->job_id." </job_id>";
$Response .= " <target_resources> ".XML_Escape( $JobSpecs->target_resources )." </target_resources>";
$Response .= " <application> ".XML_Escape( $JobSpecs->application )." </application>";
$Response .= " <owners> ".XML_Escape( $JobSpecs->owners )." </owners>";
$Response .= " <read_access> ".XML_Escape( $JobSpecs->read_access )." </read_access>";
$Response .= " <write_access> ".XML_Escape( $JobSpecs->write_access )." </write_access>";
$Response .= " <job_specifics> ".$JobSpecs->job_specifics." </job_specifics>";
$Response .= " <state> ".XML_Escape( $JobState )." </state> <state_time_stamp> ".$JobStateTimeStamp." </state_time_stamp> </job>";

// return the response...
return( LGI_Response( $Response ) );
?>

