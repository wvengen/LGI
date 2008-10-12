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
   $Respository = $_POST[ "repository" ];

  if( isset( $_GET[ "repository" ] ) )
   $Repository = $_GET[ "repository" ];

  if( isset( $Repository ) && is_dir( "./".escapeshellcmd( $Repository ) ) )
  {
   $Find = "find ./".escapeshellcmd( $Repository )." -type f -printf '<file name=\"%P\"> <size> %s </size> <date> %T@ </date> </file> '";
   $Content = shell_exec( $Find ); 
  }
  else
   $Content = "";

  echo "<repository_content name=\"$Repository\"> $Content </repository_content>";
?>
