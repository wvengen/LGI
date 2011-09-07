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

function CreateRepositoryResourceFile( $Resources = "" )
{
 global $Config;
 
 if( !isset( $Resources ) || ( $Resources == "" ) ) return;

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
  $RepositoryResourceFile = $Config[ "REPOSITORY_DIRECTORY" ]."/".$Config[ "REPOSITORY_RESOURCE_LIST_FILE" ];
 else
 {
  $RepositoryResourceFile = getcwd()."/".$Config[ "REPOSITORY_RESOURCE_LIST_FILE" ];
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( $Config[ "REPOSITORY_SSH_COMMAND" ] != "" )
  $SSHCommand = $Config[ "REPOSITORY_SSH_COMMAND" ];
 else
 {
  $RepositoryResourceFile = getcwd()."/".$Config[ "REPOSITORY_RESOURCE_LIST_FILE" ];
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( ( $Config[ "REPOSITORY_SCP_COMMAND" ] == "" ) || ( $Config[ "REPOSITORY_URL" ] == "" ) )
 {
  $RepositoryResourceFile = getcwd()."/".$Config[ "REPOSITORY_RESOURCE_LIST_FILE" ];
  $RepositoryURL = Get_Server_Name();
  $RepositoryIDFile = "";
 }

 if( $RepositoryIDFile != "" )
  exec( "$SSHCommand -i $RepositoryIDFile $RepositoryURL \"echo $Resources > $RepositoryResourceFile\"" );
 else
 {
  @$File = fopen( $RepositoryResourceFile, "w" );
  if( $File )
  {
   fwrite( $File, $Resources );
   fclose( $File );
  }
 }

}

// --------------------------------------------------- 

function CreateRepository( &$RepositoryDir, &$RepositoryURL, &$RepositoryIDFile, $WriteAccess = "any" )
{
 global $Config;

 $RepositoryName = "JOB_".md5( uniqid( rand(), TRUE ) );
 $RepositoryName = substr( $RepositoryName, -3 )."/".$RepositoryName;

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
  exec( "$SSHCommand -i $RepositoryIDFile $RepositoryURL \"mkdir -p $RepositoryDir; chmod 770 $RepositoryDir; echo $WriteAccess > $RepositoryDir/.LGI_repository_write_access\"" ); 
 else
 {
  if( !is_dir( $RepositoryDir ) )
  {
   $OldMask = umask( 0 );
   @mkdir( $RepositoryDir, 0770, true );
   $File = @fopen( "$RepositoryDir/.LGI_repository_write_access", "w" );
   if( $File )
   {
    fwrite( $File, $WriteAccess );
    fclose( $File );
   }
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
   $RepositoryURL = substr( $RepositoryURL, -3 )."/".$RepositoryURL;
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
   $Find = "find ".$RepositoryArray[ 2 ]." -type f -printf '//%P/%s/%T@//'";    // use seperators that cannot appear in paths or filenames...
   if( $Config[ "REPOSITORY_SSH_IDENTITY_FILE" ] != "" )
    $Content = shell_exec( $Config[ "REPOSITORY_SSH_COMMAND" ]." -i ".$Config[ "REPOSITORY_SSH_IDENTITY_FILE" ]." ".$RepositoryArray[ 1 ]." \"$Find\"" );
   else
    $Content = shell_exec( $Find );
  }
 }

 $Data = ""; $NrOfFiles = 0; $Length = strlen( $Content );
 for( $i = 2; $i < $Length; ++$i )
 {
  $SepPos = strpos( $Content, "//", $i );

  $DatePos = $SepPos - 1;
  while( $DatePos > $i && $Content[ $DatePos ] != '/' ) --$DatePos;
  $SizePos = $DatePos - 1;
  while( $SizePos > $i && $Content[ $SizePos ] != '/' ) --$SizePos;
  $FilePos = $DatePos - 1;
  while( $FilePos > 0 && substr( $Content, $FilePos, 2 ) != '//' ) --$FilePos;

  $Date = substr( $Content, $DatePos + 1, $SepPos - $DatePos - 1 );
  $Size = substr( $Content, $SizePos + 1, $DatePos - $SizePos - 1 );
  $File = substr( $Content, $FilePos + 2, $SizePos - $FilePos - 2 );

  $NrOfFiles++;
  $Data .= " <file number=\"$NrOfFiles\"> <file_name> ".XML_Escape( $File )." </file_name> <size> $Size </size> <date> $Date </date> </file>";

  $i = $SepPos + 4;
 }
 $Data .= " <number_of_files> $NrOfFiles </number_of_files>";

 return( $Data );
}


// ---------------------------------------------------

?>
