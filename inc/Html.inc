<?php

// []--------------------------------------------------------[]
//  |                       Html.inc                         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Subroutines for basic interface HTML...    |
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

// -------------------------------------------------------------------- 

function Page_Head( $Title = "" )
{
 if( $Title != "" )
  echo "<html><head><title>$Title</title></head><body bgcolor=ffffff>";
 else
  echo "<html><body bgcolor=ffffff>";
}

// --------------------------------------------------------------------

function Page_Tail( )
{
 echo "</body></html>";
}

// -------------------------------------------------------------------- 

function Start_Table( $Extra = "width=100%" )
{
 echo "<table border=1 $Extra>";
}

// -------------------------------------------------------------------- 

function Start_Table_noborder( $Extra = "width=100%" )
{
 echo "<table border=0 $Extra>";
}

// -------------------------------------------------------------------- 

function End_Table()
{
 echo "</table>\n";
}

// -------------------------------------------------------------------- 

function Row1( $X )
{
 if ( $X == "" ) $X = "<br>";
 echo "<tr><td colspan=12>$X</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Row2( $X, $Y )
{
 if ( $X == "" ) $X = "<br>";
 if ( $Y == "" ) $Y = "<br>";
 echo "<tr><td colspan=6>$X</td><td colspan=6>$Y</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Row3( $X, $Y, $Z ) 
{
 if ( $X == "" ) $X = "<br>";
 if ( $Y == "" ) $Y = "<br>";
 if ( $Z == "" ) $Z = "<br>";
 echo "<tr><td colspan=4>$X</td><td colspan=4>$Y</td><td colspan=4>$Z</td></tr>\n";
}

// --------------------------------------------------------------------

function Row4( $X, $Y, $Z, $XX )
{
 if ( $X == "" ) $X = "<br>";
 if ( $Y == "" ) $Y = "<br>";
 if ( $Z == "" ) $Z = "<br>";
 if ( $XX == "" ) $XX = "<br>";
 echo "<tr><td colspan=3>$X</td><td colspan=3>$Y</td><td colspan=3>$Z</td><td colspan=3>$XX</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Exit_with_text( $Text )
{
 Start_Table();
 Row1( "<font color='red'><b>".$Text."</b></font>" );
 End_Table();
 Page_Tail();
 exit;
}

// -------------------------------------------------------------------- 

?>
