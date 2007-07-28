<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Servers.inc' );

Server_Check_For_InActive_Resources();       // check if any of the resources is deemed inactive...
Server_Check_And_Perform_Updates();          // check if there are any updates to perform...

?>
