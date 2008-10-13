<?php

// []--------------------------------------------------------[]
//  |                   Repository.inc                       |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2008.                         |
//  | USE:        Subroutines to create a repo...            |
//  |                                                        |
// []--------------------------------------------------------[]
//
//
// Copyright (C) 2008 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/Utils.inc' );

// --------------------------------------------------- 

function CreateRepository( &$RepositoryDir, &$RepositoryURL, &$RepositoryIDFile )
{
 global $Config;

 $RepositoryName = "JOB_".md5( uniqid( time() ) );

 if( ( $Config[ "REPOSITORY_SERVER_NAME" ] != "" ) && ( $Config[ "REPOSITORY_SSH_IDENTITY_FILE" ] != "" ) )
 {
  $RepositoryURL = $Config[ "REPOSITORY_SERVER_NAME" ];
  $RepositoryIDFile = $Config[ "REPOSITORY_SSH_IDENTITY_FILE" ];
 }
 else
 {
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( $Config[ "REPOSITORY_DIRECTORY" ] != "" )
  $RepositoryDir = $Config[ "REPOSITORY_DIRECTORY" ]."/".$RepositoryName;
 else
 {
  $RepositoryDir = getcwd()."/".$RepositoryName;
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( $Config[ "REPOSITORY_SSH_COMMAND" ] != "" )
  $SSHCommand = $Config[ "REPOSITORY_SSH_COMMAND" ];
 else
 {
  $RepositoryDir = getcwd()."/".$RepositoryName;
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( ( $Config[ "REPOSITORY_SCP_COMMAND" ] == "" ) || ( $Config[ "REPOSITORY_URL" ] == "" ) )
 {
  $RepositoryDir = getcwd()."/".$RepositoryName;
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( $RepositoryIDFile != "" )
  exec( "$SSHCommand -i $RepositoryIDFile $RepositoryURL \"mkdir $RepositoryDir; chmod 770 $RepositoryDir\"" );
 else
 {
  if( !is_dir( $RepositoryDir ) )
  {
   $OldMask = umask( 0 );
   mkdir( $RepositoryDir, 0770 );
   umask( $OldMask );
  }
 }

}

// --------------------------------------------------- 

function DeleteRepository( $RepositoryURL )
{
 global $Config;

 if( $RepositoryURL == "" ) return;

 $RepositoryArray = CommaSeparatedField2Array( $RepositoryURL, ":" );

 if( $RepositoryArray[ 0 ] == 2 )
 {
  if( $Config[ "REPOSITORY_SSH_IDENTITY_FILE" ] != "" )
  {
   if( $Config[ "REPOSITORY_SSH_COMMAND" ] != "" )
    exec( $Config[ "REPOSITORY_SSH_COMMAND" ]." -i ".$Config[ "REPOSITORY_SSH_IDENTITY_FILE" ]." ".$RepositoryArray[ 1 ].' "rm -rf '.$RepositoryArray[ 2 ].'"' );
  }
  else
   if( $RepositoryArray[ 1 ] == Get_Server_Name() )
    rmpath( $RepositoryArray[ 2 ] );
 }

}

// --------------------------------------------------- 

function RepositoryURL2WWW( $RepositoryURL )
{
 global $Config;

 if( $RepositoryURL != "" )
 {  
  $RepositoryArray = CommaSeparatedField2Array( $RepositoryURL, ":" );
  
  if( $RepositoryArray[ 0 ] == 2 )
  {  
   $RepositoryURL = basename( $RepositoryArray[ 2 ] );
   if( $Config[ "REPOSITORY_URL" ] != "" ) $RepositoryURL = $Config[ "REPOSITORY_URL" ]."/".$RepositoryURL;
  }  
  else
   $RepositoryURL = "";
 }

 return( $RepositoryURL );
}

// --------------------------------------------------- 

function GetRepositoryContent( $RepositoryURL )
{
 global $Config;

 $Content = "";

 if( $RepositoryURL != "" )
 {
  $RepositoryArray = CommaSeparatedField2Array( $RepositoryURL, ":" );

  if( $RepositoryArray[ 0 ] == 2 )
  {
   if( $Config[ "REPOSITORY_SSH_IDENTITY_FILE" ] != "" )
   {
    $Find = "find ".$RepositoryArray[ 2 ]." -type f -printf '<file name=\\\"%P\\\"> <size> %s </size> <date> %T@ </date> </file> '";
    $Content = shell_exec( $Config[ "REPOSITORY_SSH_COMMAND" ]." -i ".$Config[ "REPOSITORY_SSH_IDENTITY_FILE" ]." ".$RepositoryArray[ 1 ]." \"$Find\"" );
   }
   else
   {
    $Find = "find ".$RepositoryArray[ 2 ]." -type f -printf '<file name=\"%P\"> <size> %s </size> <date> %T@ </date> </file> '";
    $Content = shell_exec( $Find );
   }
  }
 }

 return( $Content );
}


// ---------------------------------------------------

?>
