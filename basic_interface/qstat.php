<?php

// []--------------------------------------------------------[]
//  |                       qstat.php                        |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Request job state from project DB...       |
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

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );
require_once( '../inc/Html.inc' );

Page_Head( "Title added" );
Start_Table();
Row1( "1" );
Row2( "1", "2" );
Row3( "1", "2", "3" );
Row4( "1", "2", "3", "4" );
Exit_With_Text( "error" );
End_Table();

Page_Tail();

?>

