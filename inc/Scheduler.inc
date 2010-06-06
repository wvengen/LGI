<?php

// []--------------------------------------------------------[]
//  |                  Scheduler.inc                         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, June 2010.                           |
//  | USE:        Subroutines of scheduler API..             |
//  |                                                        |
// []--------------------------------------------------------[]
//
//
// Copyright (C) 2010 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/MySQL.inc' );

// -------------------------------------------------------------------- 

function ScheduleEvent( $Event = "schedule_cycle", $Delay = 0, $Project = "" )
{
 if( $Project == "" ) $Project = Get_Selected_MySQL_DataBase();
 $DB = Get_Selected_MySQL_DataBase();
 if( $DB != $Project ) Select_MySQL_DataBase( $Project ); 
 mysql_query( "INSERT INTO event_queue SET event='".$Event."', event_time_stamp=UNIX_TIMESTAMP()+$Delay" );
 if( ( $DB != "" ) && ( $DB != $Project ) ) Select_MySQL_DataBase( $DB ); 
}

// -------------------------------------------------------------------- 

function WaitForEvent( $Projects = ""  )
{
 if( $Projects == "" ) $Projects = Get_Selected_MySQL_DataBase();
 if( is_string( $Projects ) ) $Projects = array( $Projects );

 while( true ) {

     for( $p = 0; $p < count( $Projects ); $p++ )
     {
      Select_MySQL_DataBase( $Projects[ $p ] );

      $Result = mysql_query( "SELECT event,event_time_stamp FROM event_queue WHERE event_time_stamp<=UNIX_TIMESTAMP() ORDER BY event_time_stamp LIMIT 1" );

      if( mysql_num_rows( $Result ) >= 1 )
      {
       $Event = mysql_fetch_object( $Result );
       mysql_free_result( $Result );
       mysql_query( "DELETE FROM event_queue WHERE event LIKE '".$Event->event."' AND event_time_stamp<=UNIX_TIMESTAMP()" );
       if( mysql_affected_rows() >= 1 ) return( $Event->event );
      }
      else
       mysql_free_result( $Result );
     }

     sleep( 1 );
 }

}

// -------------------------------------------------------------------- 

function ClearAllEvents( $Event = "schedule_cycle", $Project = "" )
{
 if( $Project == "" ) $Project = Get_Selected_MySQL_DataBase();
 $DB = Get_Selected_MySQL_DataBase();
 if( $DB != $Project ) Select_MySQL_DataBase( $Project ); 
 mysql_query( "DELETE FROM  event_queue WHERE event='".$Event."'" );
 if( ( $DB != "" ) && ( $DB != $Project ) ) Select_MySQL_DataBase( $DB ); 
}

// -------------------------------------------------------------------- 

?>
