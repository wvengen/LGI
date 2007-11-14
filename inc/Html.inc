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
  echo "<html><head><title>$Title</title></head><body bgcolor='ffffff'>";
 else
  echo "<html><body bgcolor='ffffff'>";
}

// --------------------------------------------------------------------

function Page_Tail( )
{
 echo "</body></html>";
}

// -------------------------------------------------------------------- 

function Start_Table( $Extra = "width='100%'" )
{
 echo "<table border='1' $Extra>";
}

// -------------------------------------------------------------------- 

function Start_Table_noborder( $Extra = "width='100%'" )
{
 echo "<table border='0' $Extra>";
}

// -------------------------------------------------------------------- 

function End_Table()
{
 echo "</table>\n";
}

// -------------------------------------------------------------------- 

function Row1( $X )
{
 echo "<tr><td colspan='120'>$X</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Row2( $X, $Y )
{
 echo "<tr><td colspan='60'>$X</td><td colspan='60'>$Y</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Row3( $X, $Y, $Z ) 
{
 echo "<tr><td colspan='40'>$X</td><td colspan='40'>$Y</td><td colspan='40'>$Z</td></tr>\n";
}

// --------------------------------------------------------------------

function Row4( $X, $Y, $Z, $XX )
{
 echo "<tr><td colspan='30'>$X</td><td colspan='30'>$Y</td><td colspan='30'>$Z</td><td colspan='30'>$XX</td></tr>\n";
}

// --------------------------------------------------------------------

function Row5( $X, $Y, $Z, $XX, $XY )
{
 echo "<tr><td colspan='24'>$X</td><td colspan='24'>$Y</td><td colspan='24'>$Z</td><td colspan='24'>$XX</td><td colspan='24'>$XY</td></tr>\n";
}

// --------------------------------------------------------------------

function Row6( $X, $Y, $Z, $XX, $XY, $XZ )
{
 echo "<tr><td colspan='20'>$X</td><td colspan='20'>$Y</td><td colspan='20'>$Z</td><td colspan='20'>$XX</td><td colspan='20'>$XY</td><td colspan='20'>$XZ</td></tr>\n";
}

// --------------------------------------------------------------------

function Row8( $X, $Y, $Z, $XX, $XY, $XZ, $YX, $YY )
{
 echo "<tr><td colspan='15'>$X</td><td colspan='15'>$Y</td><td colspan='15'>$Z</td><td colspan='15'>$XX</td><td colspan='15'>$XY</td><td colspan='15'>$XZ</td><td colspan='15'>$YX</td><td colspan='15'>$YY</td></tr>\n";
}

// -------------------------------------------------------------------- 

function Exit_with_text( $Text )
{
 Start_Table();
 Row1( "<font color='red'><b>$Text</b></font>" );
 End_Table();
 Page_Tail();
 exit;
}

// -------------------------------------------------------------------- 

?>
