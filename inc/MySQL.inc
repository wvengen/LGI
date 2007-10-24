<?php

// []--------------------------------------------------------[]
//  |                       MySQL.inc                        |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        MySQL connection subroutines...            |
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
require_once( '../inc/Response.inc' );
require_once( '../inc/Errors.inc' );

global $MySQLSetup_Flag;
global $MySQLCurrentProjectDatabase;

$MySQLSetup_Flag = 0;
$MySQLCurrentProjectDatabase = "";

// -------------------------------------------------------------------- 

function Setup_MySQL_Connection( $ExitOnError = true )
{
 global $MySQLSetup_Flag;
 global $MySQLCurrentProjectDatabase;
 global $Config;
 global $ErrorMsgs;

 // Check if we have setup already ...
 if( $MySQLSetup_Flag ) return( 0 );

 $Host = mysql_escape_string( $Config[ "MYSQL_URL" ] );
 $User = mysql_escape_string( $Config[ "MYSQL_USER" ] );
 $Passwd = mysql_escape_string( $Config[ "MYSQL_PASSWD" ] );

 // Connect ...
 if( !mysql_pconnect( $Host, $User, $Passwd ) )
  return( LGI_Error_Response( 3, $ErrorMsgs[ 3 ], $ExitOnError ) );

 $DataBase = mysql_escape_string( $Config[ "MYSQL_DEFAULT_DATABASE" ] );

 // Select default database ...
 if( isset( $DataBase ) )
  if( !mysql_select_db( $DataBase ) )
   return( LGI_Error_Response( 4, $ErrorMsgs[ 4 ], $ExitOnError ) );
  else
   $MySQLCurrentProjectDatabase = $DataBase;

 // And flag the connection we made ...
 $MySQLSetup_Flag = 1;

 return( 0 );
}

// -------------------------------------------------------------------- 

function Select_MySQL_DataBase( $DataBase, $ExitOnError = true )
{
 global $ErrorMsgs;
 global $MySQLCurrentProjectDatabase;

 // Setup connection ...
 Setup_MySQL_Connection( $ExitOnError );

 $DataBase = mysql_escape_string( $DataBase );

 // and try to use requested database ...
 if( !mysql_select_db( $DataBase ) )
  return( LGI_Error_Response( 4, $ErrorMsgs[ 4 ], $ExitOnError ) );
 else
  $MySQLCurrentProjectDatabase = $DataBase;

 return( 0 );
}

// -------------------------------------------------------------------- 

function Get_Selected_MySQL_DataBase( )
{
 global $MySQLCurrentProjectDatabase;
 return( $MySQLCurrentProjectDatabase );
}

// -------------------------------------------------------------------- 

// Make connection at include ...
Setup_MySQL_Connection();

?>
