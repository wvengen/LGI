<?php

// []--------------------------------------------------------[]
//  |                      Interfaces.inc                    |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Subroutines for interface API...           |
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

function Interface_Verify( &$Project, &$User, &$Groups, $ExitOnError = true )
{
 global $ErrorMsgs;
 global $Config;

 // first see if valid client certificate was used in the connection ...
 if( SSL_Verify_Client( $ExitOnError ) ) 
  return( LGI_Error_Response( 2, $ErrorMsgs[ 2 ], $ExitOnError ) );

 if( !isset( $Groups ) || ( $Groups == "" ) )
  return( LGI_Error_Response( 31, $ErrorMsgs[ 31 ], $ExitOnError ) );

 if( strlen( $Groups ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 56, $ErrorMsgs[ 56 ], $ExitOnError ) );

 if( !isset( $User ) || ( $User == "" ) )
  return( LGI_Error_Response( 32, $ErrorMsgs[ 32 ], $ExitOnError ) );
 
 if( strlen( $User ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
  return( LGI_Error_Response( 57, $ErrorMsgs[ 57 ], $ExitOnError ) );

 // if it was not posted, use default value for this server...
 if( !isset( $Project ) || ( $Project == "" ) )
  $Project = $Config[ "MYSQL_DEFAULT_DATABASE" ];
 else 
 {
  if( strlen( $Project ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] )
   return( LGI_Error_Response( 58, $ErrorMsgs[ 58 ], $ExitOnError ) );
 }

 // normalize some strings and fields... 
 $User = NormalizeString( $User );
 $Project = NormalizeString( $Project );
 $Groups = NormalizeCommaSeparatedField( $Groups, "," );

 // get common name from certificate ...
 $CommonNameArray = CommaSeparatedField2Array( SSL_Get_Common_Name(), ";" );
 $CommonName = mysql_escape_string( $CommonNameArray[ 1 ] );

 // check common name against posted username...
 if( $CommonName != $User )
  return( LGI_Error_Response( 30, $ErrorMsgs[ 30 ], $ExitOnError ) );

 // check if certificate has any allowed groups and project listed...
 if( $CommonNameArray[ 0 ] >= 3 )
 {
  // check which posted groups are allowed by the certificate list...
  $GroupArray = CommaSeparatedField2Array( $Groups, "," );
  $NewGroupList = "";
  $FoundAGroup = 0;

  for( $i = 1; $i <= $GroupArray[ 0 ]; $i++ )
   if( FoundInCommaSeparatedField( $CommonNameArray[ 2 ], $GroupArray[ $i ], "," )  || ( $GroupArray[ $i ] == $User ) )
   { 
    $NewGroupList .= ", ".$GroupArray[ $i ];
    $FoundAGroup = 1;
   }

  // check if any posted group is allowed...
  if( !$FoundAGroup )
   return( LGI_Error_Response( 38, $ErrorMsgs[ 38 ], $ExitOnError ) );
  
  // now only use the groups that are also allowed by the certificate...
  $Groups = substr( $NewGroupList, 2 );

  // now check the allowed projects...
  if( FoundInCommaSeparatedField( $CommonNameArray[ 3 ], $Project, "," ) )
   return( Select_MySQL_DataBase( $Project, $ExitOnError ) );
  else
   return( LGI_Error_Response( 37, $ErrorMsgs[ 37 ], $ExitOnError ) );
 }

 // check if certificate has any allowed projects listed...
 if( $CommonNameArray[ 0 ] >= 2 )
 {
  // check if allowed...
  if( FoundInCommaSeparatedField( $CommonNameArray[ 2 ], $Project, "," ) )
   return( Select_MySQL_DataBase( $Project, $ExitOnError ) );
  else
   return( LGI_Error_Response( 37, $ErrorMsgs[ 37 ], $ExitOnError ) );
 }
  
 // otherwise just try the possibly posted Project...
 return( Select_MySQL_DataBase( $Project, $ExitOnError ) );
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

function Get_Master_Server_URL( $ExitOnError = true )
{
 global $ErrorMsgs;

 $queryresult = mysql_query( "SELECT url FROM active_resources WHERE project_server=1" );

 if( mysql_num_rows( $queryresult ) != 1 )
  return( LGI_Error_Response( 6, $ErrorMsgs[ 6 ], $ExitOnError ) );

 $MasterServer = mysql_fetch_object( $queryresult );

 mysql_free_result( $queryresult );

 return( $MasterServer->url );
}

// -------------------------------------------------------------------- 

function Interface_User_In_Group_Is_Allowed_To_Submit( $User, $Group, $Application, &$NrOfJobsAllowed )
{

 // escape the variables first...
 $User = mysql_escape_string( $User );
 $Group = mysql_escape_string( $Group );
 $Application = mysql_escape_string( $Application );

 // check if user or 'any' user is present in users_denied list for specific or 'any' app...
 $queryresult = mysql_query( "SELECT user_name FROM users_denied WHERE ( user_name='".$User."' OR user_name='any' ) AND ( application='".$Application."' OR application='any' )" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  mysql_free_result( $queryresult );
  return( 0 );
 }
 mysql_free_result( $queryresult );

 // check if user is present in users_allowed for specific application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='".$User."' AND application='".$Application."'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if user is present in users_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM users_allowed WHERE user_name='".$User."' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 1 );
 }
 mysql_free_result( $queryresult );

 // check if group or 'any' group is present in groups_denied list for specific or 'any' app...
 if( $Group != "" )
 {
  $queryresult = mysql_query( "SELECT group_name FROM groups_denied WHERE ( group_name='".$Group."' OR group_name='any' ) AND ( application='".$Application."' OR application='any' )" );
  if( mysql_num_rows( $queryresult ) >= 1 )
  {
   mysql_free_result( $queryresult );
   return( 0 );
  }
  mysql_free_result( $queryresult );
 }

 // check if group is present in groups_allowed for specific application...
 if( $Group != "" )
 {
  $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='".$Group."' AND application='".$Application."'" );
  if( mysql_num_rows( $queryresult ) >= 1 )
  {
   $querydata = mysql_fetch_object( $queryresult );
   $NrOfJobsAllowed = $querydata -> job_limit;
   mysql_free_result( $queryresult );
   return( 2 );
  }
  mysql_free_result( $queryresult );
 }

 // check if group is present in groups_allowed for 'any' application...
 if( $Group != "" )
 {
  $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='".$Group."' AND application='any'" );
  if( mysql_num_rows( $queryresult ) >= 1 )
  {
   $querydata = mysql_fetch_object( $queryresult );
   $NrOfJobsAllowed = $querydata -> job_limit;
   mysql_free_result( $queryresult );
   return( 2 );
  }
  mysql_free_result( $queryresult );
 }

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
  return( 2 );
 }
 mysql_free_result( $queryresult );

 // check if 'any' group is present in groups_allowed for 'any' application...
 $queryresult = mysql_query( "SELECT job_limit FROM groups_allowed WHERE group_name='any' AND application='any'" );
 if( mysql_num_rows( $queryresult ) >= 1 )
 {
  $querydata = mysql_fetch_object( $queryresult );
  $NrOfJobsAllowed = $querydata -> job_limit;
  mysql_free_result( $queryresult );
  return( 2 );
 }
 mysql_free_result( $queryresult );

 // by default we do not allow jobs if we know nothing...
 return( 0 );   
}

// -------------------------------------------------------------------- 

function Interface_Count_Jobs_In_Queue( $Owner, $Application, &$TotalNrOfJobs, &$NrOfRunningJobs, $SearchFor = "owners" )
{
 
 // first escape the variables...
 $Owner = mysql_escape_string( $Owner );
 $Application = mysql_escape_string( $Application );

 $TheRegExpression = MakeRegularExpressionForCommaSeparatedField( $Owner, "," );

 // query for number of 'running' jobs in job_queue of given owner and application..
 $queryresult = mysql_query( "SELECT COUNT(job_id) AS N FROM job_queue WHERE $SearchFor REGEXP '".$TheRegExpression."' AND application='".$Application."' AND ( state='running' OR state='queued' )" );
 $querydata = mysql_fetch_object( $queryresult );

 $NrOfRunningJobs = $querydata -> N;

 mysql_free_result( $queryresult );

 // query for number of 'running' jobs in job_queue of given owner and application..
 $queryresult = mysql_query( "SELECT COUNT(job_id) AS N FROM job_queue WHERE $SearchFor REGEXP '".$TheRegExpression."' AND application='".$Application."'" );
 $querydata = mysql_fetch_object( $queryresult );

 $TotalNrOfJobs = $querydata -> N;

 mysql_free_result( $queryresult );
}

// -------------------------------------------------------------------- 

function Interface_Is_Target_Resource_Known( $Resource, $Application = "" )
{
 if( $Resource == "any" ) return( 1 );

 $Resource = mysql_escape_string( $Resource );

 $ReturnValue = 0;

 if( $Application === "" )
  $queryresult = mysql_query( "SELECT resource_id FROM active_resources WHERE resource_name LIKE ".$Resource."'" );
 else
  $queryresult = mysql_query( "SELECT resource_id FROM active_resources WHERE resource_name LIKE '".$Resource."' AND resource_capabilities LIKE '%<".$Application.">%</".$Application.">%'" );

 if( mysql_num_rows( $queryresult ) >= 1 )
  $ReturnValue = 1;

 mysql_free_result( $queryresult );
 return( $ReturnValue );
}

// -------------------------------------------------------------------- 

function Interface_Clear_Spin_Lock_On_Job( $JobID, $ExitOnError = true )
{
 global $ErrorMsgs;

 $queryresult = mysql_query( "SELECT lock_state FROM job_queue WHERE job_id=".$JobID." AND lock_state>=1" );
 if( mysql_num_rows( $queryresult ) <= 0 )
 {
  mysql_free_result( $queryresult );
  return( LGI_Error_Response( 36, $ErrorMsgs[ 36 ], $ExitOnError ) );
 }

 $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE job_id=".$JobID );

 return( 0 );
}

// -------------------------------------------------------------------- 

function Interface_Set_Spin_Lock_On_Job( $JobID, $ExitOnError = true )
{
 global $Config;
 global $ErrorMsgs;

 $Count = 0; 

 while( $Count < $Config[ "LGI_SPIN_LOCK_ATTEMPTS" ] )
 {
  $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state+1 WHERE job_id=".$JobID );

  // if the update couldn't be done, the job wasn't there...
  if( !$queryresult )
   return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ], $ExitOnError ) );

  // check the value, which is now 1 or bigger...
  $queryresult = mysql_query( "SELECT lock_state FROM job_queue WHERE job_id=".$JobID );
  $LockState = mysql_fetch_object( $queryresult );
  mysql_free_result( $queryresult );

  // we did set it...
  if( $LockState->lock_state == 1 )
   return( 0 );

  // we didn't set it... decrease value again and try again later...
  $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE job_id=".$JobID );

  // if the update couldn't be done, the job wasn't there...
  if( !$queryresult )
   return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ], $ExitOnError ) );

  $Count++;
  sleep( 1 );
 }
 
 // lock spin has timed-out... so we are to busy...
 return( LGI_BackOff_Response( $ExitOnError ) );
}

// -------------------------------------------------------------------- 

function Interface_Wait_For_Cleared_Spin_Lock_On_Job( $JobID, $ExitOnError = true )
{
 global $Config;
 global $ErrorMsgs;

 $Count = 0; 

 // now make sure we get data and there is no lock set...
 while( $Count < $Config[ "LGI_SPIN_LOCK_ATTEMPTS" ] )
 {
  $queryresult = mysql_query( "SELECT * FROM job_queue WHERE job_id=".$JobID );

  if( mysql_num_rows( $queryresult ) <= 0 )
  {
   mysql_free_result( $queryresult );
   return( LGI_Error_Response( 15, $ErrorMsgs[ 15 ], $ExitOnError ) );
  }

  $JobSpecs = mysql_fetch_object( $queryresult );
  mysql_free_result( $queryresult );

  // was lock not set...
  if( $JobSpecs -> lock_state == 0 )
   return( $JobSpecs );

  sleep( 1 );
  $Count++;
 }

 // lock spin has timed-out... so we are to busy...
 return( LGI_BackOff_Response( $ExitOnError ) );
}

// -------------------------------------------------------------------- 

function Interface_Make_Query_For_Work_For_Owners( $OwnerArray, $State, $Application, $Start, $Limit )
{
 // first escape the variables...
 $State = mysql_escape_string( $State );
 $Application = mysql_escape_string( $Application );

 $Query = "SELECT job_id,state,application,state_time_stamp,target_resources,owners,read_access,write_access,job_specifics FROM job_queue WHERE ( ";

 $Owner = mysql_escape_string( $OwnerArray[ 1 ] );
 $RegExpUser = MakeRegularExpressionForCommaSeparatedField( $Owner, "," );
 $Query .= " ( read_access REGEXP '".$RegExpUser."' )";
 for( $o = 2; $o <= $OwnerArray[ 0 ]; $o++ )
 {
  $Owner = mysql_escape_string( $OwnerArray[ $o ] );
  $RegExpUser = MakeRegularExpressionForCommaSeparatedField( $Owner, "," );
  $Query .= " OR ( read_access REGEXP '".$RegExpUser."' )";
 }

 if( isset( $State ) && ( $State != "" ) && ( $State != "any" ) )
 {
  if( $State[ 0 ] == "!" )
   $Query .= " ) AND ( state!='".substr($State,1)."'";
  else
   $Query .= " ) AND ( state='".$State."'";
 }

 if( isset( $Application ) && ( $Application != "" ) && ( $Application != "any" ) )
 {
  if( $Application[ 0 ] == "!" )
   $Query .= " ) AND ( application!='".substr($Application,1)."'";
  else
   $Query .= " ) AND ( application='".$Application."'";
 }

 $Query .= " ) ORDER BY state_time_stamp LIMIT ".$Limit." OFFSET ".$Start;

 return( $Query );
}

// -------------------------------------------------------------------- 

function Interface_Is_User_Allowed_To_Read_Job( $JobSpecs, $User, $Groups )
{
 $PossibleJobOwnersArray = CommaSeparatedField2Array( $User.", ".$Groups, "," );
 $MatchFlag = 0;

 for( $i = 1; $i <= $PossibleJobOwnersArray[ 0 ]; $i++ )
  if( FoundInCommaSeparatedField( $JobSpecs->read_access, $PossibleJobOwnersArray[ $i ], "," ) )
   $MatchFlag = 1;

 return( $MatchFlag );
}

// -------------------------------------------------------------------- 

function Interface_Is_User_Allowed_To_Modify_Job( $JobSpecs, $User, $Groups )
{
 $PossibleJobOwnersArray = CommaSeparatedField2Array( $User.", ".$Groups, "," );
 $MatchFlag = 0;

 for( $i = 1; $i <= $PossibleJobOwnersArray[ 0 ]; $i++ )
  if( FoundInCommaSeparatedField( $JobSpecs->write_access, $PossibleJobOwnersArray[ $i ], "," ) )
   $MatchFlag = 1;

 return( $MatchFlag );
}

// -------------------------------------------------------------------- 

function Interface_Found_Id_In_List( $JobIdArray, $JobId )
{
 for( $i = 1; $i <= $JobIdArray[ 0 ]; $i++ )
  if( $JobIdArray[ $i ] == $JobId ) 
   return( 1 );

 return( 0 );
}

// -------------------------------------------------------------------- 

function Interface_Get_Groups_From_Common_Name( $CommonName )
{
 $CommonNameArray = CommaSeparatedField2Array( $CommonName, ';' );

 if( $CommonNameArray[ 0 ] >= 3 )     // see if we have a group list in the cert...
 {
  $Groups = $CommonNameArray[ 1 ];      // by default add user's personal group into the list...

  $GroupArray = CommaSeparatedField2Array( $CommonNameArray[ 2 ] , "," );
 
  for( $i = 1; $i <= $GroupArray[ 0 ]; $i++ )
   if( $GroupArray[ $i ] != "any" )
    $Groups = $Groups.", ".$GroupArray[ $i ];       // only add group if not 'any' ...

  return( $Groups );
 }
 else
  return( $CommonNameArray[ 1 ] );    // if not default to user name...
}

// -------------------------------------------------------------------- 

?>
