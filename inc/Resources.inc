<?php

// []--------------------------------------------------------[]
//  |                      Resources.inc                     |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Subroutines for resources API...           |
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
require_once( '../inc/Ssl.inc' );
require_once( '../inc/Errors.inc' );
require_once( '../inc/Response.inc' );
require_once( '../inc/MySQL.inc' );
require_once( '../inc/Utils.inc' );
require_once( '../inc/Scheduler.inc' );

// -------------------------------------------------------------------- 

function Resource_Verify_Session( &$ResourceData )
{
 global $Config;
 global $ErrorMsgs;

 // check if session was verified allready...
 if( isset( $ResourceData -> SessionVerified ) && ( $ResourceData -> SessionVerified == $ResourceData -> SessionID ) )
  return( 0 );

 // check if variable was set correctly...
 if( !isset( $ResourceData -> SessionID ) || ( $ResourceData -> SessionID == "" ) || !ctype_digit( $ResourceData -> SessionID ) )
  return( LGI_Error_Response( 8, $ErrorMsgs[ 8 ] ) );

 if( strlen( $ResourceData -> SessionID ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
  return( LGI_Error_Response( 62, $ErrorMsgs[ 62 ] ) );

 // get session id for this resource from running_sessions...
 $query = mysql_query( "SELECT session_id FROM running_sessions WHERE session_id=".$ResourceData->SessionID." AND resource_id=".$ResourceData->resource_id );

 // if not found... bugger out...
 if( mysql_num_rows( $query ) != 1 )
 {
  mysql_free_result( $query );
  return( 1 );
 }

 mysql_free_result( $query );

 // update session_time_stamp...
 $query = mysql_query( "UPDATE running_sessions SET session_time_stamp=UNIX_TIMESTAMP() WHERE session_id=".$ResourceData->SessionID );

 // mark the verification for speedup...
 $ResourceData -> SessionVerified = $ResourceData -> SessionID;

 return( 0 );
}

// -------------------------------------------------------------------- 

function Resource_Setup_Session( &$ResourceData )
{
 global $ErrorMsgs;

 // insert entry into running_sessions table...
 $queryresult = mysql_query( "INSERT INTO running_sessions SET resource_id=".$ResourceData->resource_id.",session_time_stamp=UNIX_TIMESTAMP()" );

 // get inserted id...
 $query = mysql_query( "SELECT session_id FROM running_sessions WHERE session_id=LAST_INSERT_ID() AND resource_id=".$ResourceData->resource_id );

 if( mysql_num_rows( $query ) != 1 )
  return( LGI_Error_Response( 44, $ErrorMsgs[ 44 ] ) );

 $querydata = mysql_fetch_object( $query );

 mysql_free_result( $query );

 // and set value into structure...
 $ResourceData -> SessionID = $querydata -> session_id;
 $ResourceData -> SessionVerified = $querydata -> session_id;

 return( 0 );
}

// --------------------------------------------------------------------

function Resource_Kill_Session( &$ResourceData )
{
 global $ErrorMsgs;

 if( Resource_Verify_Session( $ResourceData ) )
  return( LGI_Error_Response( 42, $ErrorMsgs[ 42 ] ) );

 // remove any stale locks this resource might have of this session...
 Resource_Remove_Stale_Locks( $ResourceData );

 // just remove entry from the table...
 $query = mysql_query( "DELETE FROM running_sessions WHERE session_id=".$ResourceData->SessionID );

 // invalidate data in structure...
 unset( $ResourceData -> SessionID );
 unset( $ResourceData -> SessionVerified );

 return( 0 );
}

// --------------------------------------------------------------------

function Resource_Verify( &$Project, $SessionID = "", $Capabilities = "" )
{
 global $Config;
 global $ErrorMsgs;

 // first see if valid client certificate was used in the connection...
 if( SSL_Verify_Client() )
  return( LGI_Error_Response( 2, $ErrorMsgs[ 2 ] ) );

 // if it was not posted, use default value for this server...
 if( !isset( $Project ) || ( $Project == "" ) )
  $Project = $Config[ "MYSQL_DEFAULT_DATABASE" ];
 else
 {
  if( strlen( $Project ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
   return( LGI_Error_Response( 58, $ErrorMsgs[ 58 ] ) );
 }
 $Project = NormalizeString( $Project );

 // check if capabilites were given and is correct...
 if( isset( $Capabilities ) && ( $Capabilities !== "" ) )
 {
  if( strlen( $Capabilities ) >= $Config[ "MAX_POST_SIZE_FOR_BLOB" ] ) 
   return( LGI_Error_Response( 69, $ErrorMsgs[ 69 ] ) );

  if( XML_Sanity_Check( $Capabilities ) )
   return( LGI_Error_Response( 73, $ErrorMsgs[ 73 ] ) );
 }
 
 // get common name from certificate ...
 $CommonNameArray = CommaSeparatedField2Array( SSL_Get_Common_Name(), ";" );
 $CommonName = mysql_escape_string( $CommonNameArray[ 1 ] );

 // check if certificate has any allowed projects listed...
 if( $CommonNameArray[ 0 ] >= 2 )  
 {
  // check if allowed...
  if( FoundInCommaSeparatedField( $CommonNameArray[ 2 ], $Project, "," ) ) 
   Select_MySQL_DataBase( $Project ); 
   else
    return( LGI_Error_Response( 37, $ErrorMsgs[ 37 ] ) );
 }
 else
 {
  // otherwise just try the possibly posted Project...

  if( isset( $Project ) && ( $Project != "" ) )
    Select_MySQL_DataBase( $Project ); 
 }

 // do query...
 $queryresult = mysql_query( "SELECT * FROM active_resources WHERE resource_name='".$CommonName."'" );

 // bug out if not found...
 if( mysql_num_rows( $queryresult ) <= 0 )
 {
  mysql_free_result( $queryresult );
  return( LGI_Error_Response( 5, $ErrorMsgs[ 5 ] ) );
 }

 // otherwise get the recorded data...
 $Data = mysql_fetch_object( $queryresult );
 mysql_free_result( $queryresult );

 // and check if the certificate's match... 
 if( $Data->client_certificate != SSL_Get_Full_Client_Certificate() )
  return( LGI_Error_Response( 2, $ErrorMsgs[ 2 ] ) );

 // update time_stamp of resource... and capabilities if posted...
 if( $Capabilities !== "" )
 {
  $queryresult = mysql_query( "UPDATE active_resources SET last_call_time=UNIX_TIMESTAMP(),resource_capabilities='".mysql_escape_string( $Capabilities )."' WHERE resource_name='".$CommonName."'" );
  $Data -> resource_capabilities = $Capabilities;
 }
 else
  $queryresult = mysql_query( "UPDATE active_resources SET last_call_time=UNIX_TIMESTAMP() WHERE resource_name='".$CommonName."'" );

 // pass through the SessionID into the resource data structure...
 if( isset( $SessionID ) && ctype_digit( $SessionID ) ) 
 {
  if( strlen( $SessionID ) >= $Config[ "MAX_POST_SIZE_FOR_INTEGER" ] )
   return( LGI_Error_Response( 62, $ErrorMsgs[ 62 ] ) );
  $Data -> SessionID = $SessionID;
  unset( $Data -> SessionVerified );
 }

 return( $Data );
}

// --------------------------------------------------------------------

function Resource_Check_For_Job_Locks( $ResourceData )
{
 global $ErrorMsgs;

 if( Resource_Verify_Session( $ResourceData ) )
  return( LGI_Error_Response( 9, $ErrorMsgs[ 9 ] ) );

 // count the number of locks this resource has...
 $TheNumberOfLocks = mysql_fetch_object(mysql_query( "SELECT COUNT(lock_id) AS N FROM running_locks WHERE resource_id=".$ResourceData->resource_id." AND session_id=".$ResourceData->SessionID )); 
 
 return( $TheNumberOfLocks->N ); 
}

// -------------------------------------------------------------------- 

function Resource_Has_Job_Locked( $ResourceData, $JobId )
{
 global $ErrorMsgs;

 if( Resource_Verify_Session( $ResourceData ) )
  return( LGI_Error_Response( 10, $ErrorMsgs[ 10 ] ) );

 // get lock data from running_locks table...
 $queryresult = mysql_query( "SELECT lock_id FROM running_locks WHERE job_id=".$JobId." AND resource_id=".$ResourceData->resource_id." AND session_id=".$ResourceData->SessionID );

 if( mysql_num_rows( $queryresult ) == 1 )
  $HasLocked = 1; 
 else
  $HasLocked = 0; 
 
 mysql_free_result( $queryresult );

 return( $HasLocked );
}

// -------------------------------------------------------------------- 

function Resource_Lock_Job( $ResourceData, $JobId, $CheckTheResource = True )
{
 global $ErrorMsgs;

 // check if this resource is allowed to lock the job by default...
 if( $CheckTheResource )
 {
  if( Resource_Verify_Session( $ResourceData ) )
   return( LGI_Error_Response( 11, $ErrorMsgs[ 11 ] ) );

  $queryresult = mysql_query( "SELECT lock_state,target_resources FROM job_queue WHERE job_id=".$JobId );  

  if( mysql_num_rows( $queryresult ) < 1 )                      // the job was not found in the db anymore...
  {
   mysql_free_result( $queryresult );
   return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ] ) );    
  }

  $JobData = mysql_fetch_object( $queryresult );
  mysql_free_result( $queryresult );

  if( !FoundInCommaSeparatedField( $JobData->target_resources, $ResourceData->resource_name, "," ) )
   return( LGI_Error_Response( 21, $ErrorMsgs[ 21 ] ) );

  if( $JobData->lock_state > 0 )                                // it was already locked by another process running concurrently...
   return( 1 );
 }
  
 // now try and get a lock on the job in job_queue...
 if( !mysql_query( "UPDATE job_queue SET lock_state=lock_state+1 WHERE job_id=".$JobId ) )
  return( 1 );

 // if the insert failed, we couldn't get the lock because probably an entry with given job_id is already inserted...
 if( mysql_query( "INSERT INTO running_locks SET job_id=".$JobId.",resource_id=".$ResourceData->resource_id.",lock_time=UNIX_TIMESTAMP(),session_id=".$ResourceData->SessionID ) )
  return( 0 );

 // this trick works because of the UNIQUE job_id index in the running_locks table... it avoids table locks...
 mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE lock_state>0 AND job_id=".$JobId );        
 return( 1 );
}

// -------------------------------------------------------------------- 

function Resource_UnLock_Job( $ResourceData, $JobId, $CheckTheResource = True )
{
 global $ErrorMsgs;

 if( $CheckTheResource )
 {
  if( Resource_Verify_Session( $ResourceData ) )
   return( LGI_Error_Response( 10, $ErrorMsgs[ 10 ] ) );

  // get lock data from running_locks table...
  $queryresult = mysql_query( "SELECT lock_id,job_id,resource_id,session_id FROM running_locks WHERE job_id=".$JobId );

  if( mysql_num_rows( $queryresult ) > 1 )
   return( LGI_Error_Response( 12, $ErrorMsgs[ 12 ] ) ); 

  if( mysql_num_rows( $queryresult ) < 1 )
   return( LGI_Error_Response( 13, $ErrorMsgs[ 13 ] ) ); 

  // now check if it was locked by current resource...
  $TheLockData = mysql_fetch_object( $queryresult );
  mysql_free_result( $queryresult );
 
  if( ( $TheLockData -> resource_id != $ResourceData -> resource_id ) || ( $TheLockData -> session_id != $ResourceData -> SessionID ) )
   return( LGI_Error_Response( 14, $ErrorMsgs[ 14 ] ) );
 }

 // remove the lock from the running_locks table...
 $queryresult = mysql_query( "DELETE FROM running_locks WHERE job_id=".$JobId." AND resource_id=".$ResourceData -> resource_id." AND session_id=".$ResourceData -> SessionID );

 // and unlock the job in the job_queue table...
 $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE lock_state>0 AND job_id=".$JobId );

 return( 0 );
}

// -------------------------------------------------------------------- 

function Resource_Remove_Stale_Locks( $ResourceData )
{
 global $ErrorMsgs;

 if( Resource_Verify_Session( $ResourceData ) )
  return( LGI_Error_Response( 7, $ErrorMsgs[ 7 ] ) );

 // get list of locks of this resource...
 $TheLocks = mysql_query( "SELECT lock_id,job_id FROM running_locks WHERE resource_id=".$ResourceData->resource_id." AND session_id=".$ResourceData->SessionID ); 
 $NumberOfLocks = mysql_num_rows( $TheLocks );

 if( $NumberOfLocks >= 1 )
  for( $lock = 0; $lock < $NumberOfLocks; $lock++ )
  {
   $LockData = mysql_fetch_object( $TheLocks );

   // now remove the stale running_locks entry too...
   $queryresult = mysql_query( "DELETE FROM running_locks WHERE lock_id=".$LockData->lock_id );

   // then remove lock from job_queue table...
   $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE lock_state>0 AND job_id=".$LockData->job_id );
  }

 mysql_free_result( $TheLocks );

 return( 0 );
}

// --------------------------------------------------------------------

function Get_Server_URL( )
{
 global $Config;

 return( $Config[ "SERVER_URL" ] );
}

// --------------------------------------------------------------------

function Get_Server_Name( )
{
 global $Config;

 return( $Config[ "SERVER_NAME" ] );
}

// -------------------------------------------------------------------- 

function Get_Master_Server_URL( )
{
 global $ErrorMsgs;

 $queryresult = mysql_query( "SELECT url FROM active_resources WHERE project_server=1" );

 if( mysql_num_rows( $queryresult ) != 1 )
  return( LGI_Error_Response( 6, $ErrorMsgs[ 6 ] ) );

 $MasterServer = mysql_fetch_object( $queryresult );

 mysql_free_result( $queryresult );

 return( $MasterServer->url );
}

// -------------------------------------------------------------------- 

function Resource_Is_Target_Resource_Known( $Resource, $Application = "" )
{
 if( $Resource == "any" ) return( 1 );

 $ReturnValue = 0;

 $Resource = mysql_escape_string( $Resource );

 if( $Application === "" )
  $queryresult = mysql_query( "SELECT resource_id FROM active_resources WHERE resource_name LIKE '".$Resource."'" );
 else
  $queryresult = mysql_query( "SELECT resource_id FROM active_resources WHERE resource_name LIKE '".$Resource."' AND resource_capabilities LIKE '%<".$Application.">%</".$Application.">%'" );

 if( mysql_num_rows( $queryresult ) >= 1 )
  $ReturnValue = 1; 

 mysql_free_result( $queryresult );
 return( $ReturnValue );
}

// -------------------------------------------------------------------- 

function Resource_Is_Owner_Allowed_To_Submit( $Owner, $Application, &$NrOfJobsAllowed )
{
 // first escape strings...
 $Owner = mysql_escape_string( $Owner );
 $Application = mysql_escape_string( $Application );

 // check if owner or 'any' user is present in users_denied list for specific or 'any' app...
 $queryresult = mysql_query( "SELECT user_name FROM users_denied WHERE ( user_name='".$Owner."' OR user_name='any' ) AND ( application='".$Application."' OR application='any' )" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  mysql_free_result( $queryresult );
  return( 0 );
 }
 mysql_free_result( $queryresult );

 // check if owner is present in users_allowed for specific application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='".$Owner."' AND application='".$Application."'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if owner is present in users_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='".$Owner."' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if owner or 'any' group is present in groups_denied list for specific or 'any' app...
 $queryresult = mysql_query( "SELECT group_name FROM groups_denied WHERE ( group_name='".$Owner."' OR group_name='any' ) AND ( application='".$Application."' OR application='any' )" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  mysql_free_result( $queryresult );
  return( 0 );
 }
 mysql_free_result( $queryresult );

 // check if owner is present in groups_allowed for specific application...
 $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='".$Owner."' AND application='".$Application."'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if owner is present in groups_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='".$Owner."' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if 'any' user is present in users_allowed for specific application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='any' AND application='".$Application."'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if 'any' user is present in users_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='any' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );
 
 // check if 'any' group is present in groups_allowed for specific application...
 $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='any' AND application='".$Application."'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if 'any' group is present in groups_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='any' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // by default we do not allow jobs if we know nothing...
 return( 0 );   
}

// -------------------------------------------------------------------- 

function Resource_Count_Owners_Jobs_In_Queue( $Owner, $Application, &$TotalNrOfJobs, &$NrOfRunningJobs )
{
 // first escape strings...
 $Owner = mysql_escape_string( $Owner );
 $Application = mysql_escape_string( $Application );

 $TheRegExpression = MakeRegularExpressionForCommaSeparatedField( $Owner, "," );

 // query for number of 'running' or 'queued' jobs in job_queue of given owner and application..
 $queryresult = mysql_query( "SELECT COUNT(job_id) AS N FROM job_queue WHERE owners REGEXP '".$TheRegExpression."' AND application='".$Application."' AND ( state='running' OR state='queued' )" );
 $querydata = mysql_fetch_object( $queryresult );

 $NrOfRunningJobs = $querydata -> N;

 mysql_free_result( $queryresult );

 // query for total number of jobs in job_queue of given owner and application..
 $queryresult = mysql_query( "SELECT COUNT(job_id) AS N FROM job_queue WHERE owners REGEXP '".$TheRegExpression."' AND application='".$Application."'" );
 $querydata = mysql_fetch_object( $queryresult );

 $TotalNrOfJobs = $querydata -> N;

 mysql_free_result( $queryresult );
}

// -------------------------------------------------------------------- 
 
?>
