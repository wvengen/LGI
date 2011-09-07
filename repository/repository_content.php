<?php

// []--------------------------------------------------------[]
//  |               repository_content.php                   |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2008.                         |
//  | USE:        Simple API to list repository content...   |
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

  if( isset( $_POST[ "repository" ] ) )
   $Repository = $_POST[ "repository" ];

  if( isset( $_GET[ "repository" ] ) )
   $Repository = $_GET[ "repository" ];

  if( isset( $Repository ) && ( preg_match( "/^JOB_[a-zA-Z0-9]{32}$/", $Repository ) == 1 ) )
  {
   $RepositoryDir = substr( $Repository, -3 )."/".$Repository;
   if( is_dir( "./".escapeshellcmd( $RepositoryDir ) ) )
   {
    $Find = "find ./".escapeshellcmd( $RepositoryDir )." -type f -printf '//%P/%s/%T@//'";    // use seperators that cannot appear in paths or filenames...
    $Content = shell_exec( $Find ); 
   }
   else
    $Content = "";
  }
  else
   $Content = "";

  echo "<repository_content> <repository_name> $Repository </repository_name>";
 
  $NrOfFiles = 0; $Length = strlen( $Content );
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
   echo " <file number=\"$NrOfFiles\"> <file_name> ".str_replace( '&#039;', '&apos;', htmlspecialchars( $File, ENT_QUOTES ) )." </file_name> <size> $Size </size> <date> $Date </date> </file>";

   $i = $SepPos + 4;
  }

  echo " <number_of_files> $NrOfFiles </number_of_files> </repository>";
?> 
