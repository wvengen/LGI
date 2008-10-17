<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/Servers.inc' );
require_once( '../inc/Repository.inc' );

global $Config;

$Projects = array( $Config[ "MYSQL_DEFAULT_DATABASE" ] );  // add default project to list...

// array_push( $Projects, "projectname1" );                // add other project of this server to list if needed...
// array_push( $Projects, "projectname2" );                // add other project of this server to list if needed...



// now perform cron jobs for all projects in list...
$ResourceList = "";                                               // empty list first...

for( $p = 0; $p < count( $Projects ); $p++ )
{
 Select_MySQL_DataBase( $Projects[ $p ]  );                       // select database and...
 Server_Check_For_InActive_Resource_Sessions();                   // check if any of the resource sessions is deemed inactive...
 Server_Check_And_Perform_Updates();                              // check if there are any updates to perform...
 $ResourceList .= ", ".Server_Resource_List_For_Repository();     // add the list of resources for this project...
}

CreateRepositoryResourceFile( substr( $ResourceList, 2 ) );       // finally create repository resource list file for all the projects...
?>
