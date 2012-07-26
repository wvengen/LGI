#!/usr/bin/php -q
<?php

// []--------------------------------------------------------[]
//  |                    Scheduler.php                       |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, June 2010.                           |
//  | USE:        Basic scheduling code...                   |
//  |                                                        |
// []--------------------------------------------------------[]
//
// Copyright (C) 2010 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/MySQL.inc' );
require_once( '../inc/Servers.inc' );
require_once( '../inc/Repository.inc' );
require_once( '../inc/Scheduler.inc' );

global $Config;

$Projects = array( $Config[ "MYSQL_DEFAULT_DATABASE" ] );  // add default project to list...

// array_push( $Projects, "projectname1" );                // add other project of this server to list if needed...
// array_push( $Projects, "projectname2" );                // add other project of this server to list if needed...

// --------------------------------------------------------------------------------------------------------------
// now schedule default events... 

ScheduleEvent( "repository_file", 0, $Config[ "MYSQL_DEFAULT_DATABASE" ] );    // let default project deal with repo file...

for( $p = 0; $p < count( $Projects ); $p++ )               // and all projects should check for inactivity and updates...
{
 ScheduleEvent( "inactive_resources", 0, $Projects[ $p ] );
 ScheduleEvent( "jobs_without_pulse", 0, $Projects[ $p ] );
 ScheduleEvent( "check_updates", 0, $Projects[ $p ] );
 ScheduleEvent( "schedule_cycle", 0, $Projects[ $p ] );
}

// --------------------------------------------------------------------------------------------------------------
// start main event loop here...

set_time_limit( 0 );                 // keep on running...

while( true ) {

 $Event = WaitForEvent( $Projects );       // wait until an event is due in any of the projects...

 // echo "$Event \n";
 
 switch( $Event ) {

 case "schedule_cycle":                           // default schedule cylce for jobs...

      // ...
      // ... here job priorities can be determined and jobs can be associated to resources ...
      // ... this event is scheduled any time a job is inserted / deleted or changed state in the db...
      // ...

      ClearAllEvents( "schedule_cycle" );       
      ScheduleEvent( "schedule_cycle", 300 );
      break;

 case "check_updates":                            // schedule cycle to check for updates...

      Server_Check_And_Perform_Updates();         // check if there are any updates to perform...

      ClearAllEvents( "check_updates" );        
      ScheduleEvent( "check_updates", 900 );
      break;

 case "jobs_without_pulse":                       // schedule cycle to check job pulses...

      // here running jobs are checked for their pulse and requeued if needed...
      mysql_query( "UPDATE job_queue SET state='queued', state_time_stamp=UNIX_TIMESTAMP() WHERE state='running' AND daemon_pulse<=(UNIX_TIMESTAMP()-3600) AND lock_state=0" );

      // and aborting jobs are checked for their pulse and aborted if needed...
      mysql_query( "UPDATE job_queue SET state='aborted', state_time_stamp=UNIX_TIMESTAMP() WHERE state='aborting' AND daemon_pulse<=(UNIX_TIMESTAMP()-3600) AND lock_state=0" );

      ClearAllEvents( "jobs_without_pulse" );         
      ScheduleEvent( "jobs_without_pulse", 1800 );
      break;

 case "inactive_resources":                       // schedule cycle to check resources...

      Server_Check_For_InActive_Resource_Sessions();       // check if any of the resource sessions is deemed inactive...

      ClearAllEvents( "inactive_resources" );         
      ScheduleEvent( "inactive_resources", 600 );
      break;

 case "repository_file":                       // schedule cycle to generate a list with resources...

      $ResourceList = "";                                            // empty list first...
      for( $p = 0; $p < count( $Projects ); $p++ )
      {
       Select_MySQL_DataBase( $Projects[ $p ] );
       $ResourceList .= ", ".Server_Resource_List_For_Repository();
      }
      CreateRepositoryResourceFile( substr( $ResourceList, 2 ) );     // finally create repository resource list file for all the projects...

      ClearAllEvents( "repository_file", $Config[ "MYSQL_DEFAULT_DATABASE" ] );
      ScheduleEvent( "repository_file", 600, $Config[ "MYSQL_DEFAULT_DATABASE" ] );  
      break;
 }
}

?>
