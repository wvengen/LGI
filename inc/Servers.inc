<?php

// []--------------------------------------------------------[]
//  |                      Servers.inc                       |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Subroutines of server API...               |
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

// -------------------------------------------------------------------- 

function Server_Verify( &$Project )
{
 global $ErrorMsgs;
 global $Config;

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

 // and also check if the resource is listed as a server for this project...
 if( !$Data->project_server )
  return( LGI_Error_Response( 39, $ErrorMsgs[ 39 ] ) );

 // update time_stamp of resource...
 $queryresult = mysql_query( "UPDATE active_resources SET last_call_time=UNIX_TIMESTAMP() WHERE resource_name='".$CommonName."'" );

 return( $Data );
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

function Server_Check_And_Perform_Updates( )
{
 global $Config;

 // get all project servers we know of...
 $ServerListQuery = mysql_query( "SELECT url FROM active_resources WHERE project_server>=1" );
 $NrOfServers = mysql_num_rows( $ServerListQuery );

 for( $i = 0; $i < $NrOfServers; $i++ )
 {
  $CurrentUpdateServer = mysql_fetch_object( $ServerListQuery );

  // run through the possible updates of this server...
  do {

   // get the latest update we have...
   $mysqlresult = mysql_query( "SELECT MAX(version) AS max FROM updates" );
   if( mysql_num_rows( $mysqlresult ) == 1 )
   {
    $VersionData = mysql_fetch_object( $mysqlresult );
    $CurrentVersionNumber = $VersionData->max;
    mysql_free_result( $mysqlresult );
   }
   else
    $CurrentVersionNumber = -1;

   // post our version number and get a possible update...
   $PostResponse =  SSL_Post_To_URL( $CurrentUpdateServer->url."/servers/server_get_update.php", "project=".Get_Selected_MySQL_DataBase()."&version=".$CurrentVersionNumber );

   $UpdateData = NormalizeString( Parse_XML( Parse_XML( Parse_XML( $PostResponse, "LGI", $Attributes ), "response", $Attributes ), "update", $Attributes ) );

   // check if update data was retrieved...
   if( $UpdateData != "" )
   {
    $UpdateFlag = NomalizeString( Parse_XML( $UpdateData, "updates", $Attributes ) );

    // an update was sent back in the response...
    if( $UpdateFlag >= 1 )
    {
     $UpdateVersion = NormalizeString( Parse_XML( $UpdateData, "update_version", $Attributes ) );
     $UpdateServers = NormalizeString( Parse_XML( $UpdateData, "target_servers", $Attributes ) );
     $UpdateQuery   = hexbin( NormalizeString( Parse_XML( $UpdateData, "update_query", $Attributes ) ) );

     // perform the update on the DB if needed...
     if( FoundInCommaSeparatedField( $UpdateServers, $Config[ "SERVER_NAME" ], "," ) )
      $mysqlresult = mysql_query( $UpdateQuery );

     // insert this update into the DB...
     mysql_query( "INSERT INTO updates SET version=".$UpdateVersion.", servers='".mysql_escape_string( $UpdateServers )."', update_query='".mysql_escape_string( $UpdateQuery )."'" );
    }
   }
   else
    $UpdateFlag = -1;        // this server did not return a valid response...

  } while( $UpdateFlag >= 1 );
  
 }

 return( 0 );
}

// -------------------------------------------------------------------- 

function Server_Remove_Stale_Locks_Of_Resource( $ResourceID, $SessionID )
{

 // get list of locks of this resource...
 $TheLocks = mysql_query( "SELECT lock_id,job_id FROM running_locks WHERE resource_id=".$ResourceID." AND session_id=".$SessionID );
 $NumberOfLocks = mysql_num_rows( $TheLocks );

 if( $NumberOfLocks >= 1 )
  for( $lock = 0; $lock < $NumberOfLocks; $lock++ )
  {
   $LockData = mysql_fetch_object( $TheLocks );

   // first remove lock from job_queue table...
   $queryresult = mysql_query( "UPDATE job_queue SET lock_state=lock_state-1 WHERE job_id=".$LockData->job_id );

   // now remove the stale running_locks entry too...
   $queryresult = mysql_query( "DELETE FROM running_locks WHERE lock_id=".$LockData->lock_id );
  }

 mysql_free_result( $TheLocks );

 return( 0 );
}

// --------------------------------------------------------------------

function Server_Check_For_InActive_Resource_Sessions( )
{
 global $Config;
 
 // get a list of apparently inactive resources...
 $TheSessions = mysql_query( "SELECT session_id,resource_id FROM running_sessions WHERE session_time_stamp<=(UNIX_TIMESTAMP()-".mysql_escape_string( $Config[ "LGI_INACTIVITY_VALUE" ] ).")" );
 $NumberOfSessions = mysql_num_rows( $TheSessions );

 if( $NumberOfSessions >= 1 )
  for( $s = 0; $s < $NumberOfSessions; $s++ )
  {
   // get id's of apparently inactive resource session...
   $ResourceID = mysql_fetch_object( $TheSessions );
   $SessionID = $ResourceID->session_id;
   $ResourceID = $ResourceID->resource_id;

   // remove any locks of this resource...
   Server_Remove_Stale_Locks_Of_Resource( $ResourceID, $SessionID );

   // and remove session entry...
   $queryresult = mysql_query( "DELETE FROM running_sessions WHERE session_id=".$SessionID );
  }
 
 mysql_free_result( $TheSessions );

 return( 0 );
}

// -------------------------------------------------------------------- 

?>
