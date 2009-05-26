<?php

// []--------------------------------------------------------[]
//  |                      Utils.inc                         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Some often used subroutines...             |
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

// --------------------------------------------------- 

function hexbin( $hex )
{
 $bin = "";

 $Length = strlen( $hex );

 $i = 0; 

 while( ( $hex[ $i ] == ' ' ) && $i < $Length ) $i++;

 for( ; $i < $Length; $i += 2 )
 {
  $bin .= chr( hexdec( $hex[ $i ].$hex[ $i + 1 ] ) );
 }

 return( $bin );
}

// --------------------------------------------------- 

function binhex( $bin )
{
 return( bin2hex( $bin ) );
}

// --------------------------------------------------- 

function CommaSeparatedField2Array( $Field, $Separator )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Length = strlen( $Field );

 $Array[ 0 ] = 1;
 $Array[ 1 ] = '';

 // build basic array...
 for( $i = 0; $i < $Length; $i++ )
  if( $Field[ $i ] != $Separator )
   $Array[ $Array[ 0 ] ] .= $Field[ $i ];
  else
  {
   $Array[ 0 ]++;
   $Array[ $Array[ 0 ] ] = '';
  } 

 // now remove possible leading spaces from all the elements in the array...
 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
 {
  if( strlen( $Array[ $i ] ) != 0 )           
   while( $Array[ $i ][ 0 ] == ' ' )
    $Array[ $i ] = substr( $Array[ $i ], 1 );

  if( strlen( $Array[ $i ] ) == 0 )               // also remove empty entries from the array...
  {
   $Array[ $i ] = $Array[ $Array[ 0 ] ];
   $Array[ 0 ]--;
   $i--;
  }
 }

 return( $Array ); 
}

// --------------------------------------------------- 

function MakeRegularExpressionForCommaSeparatedField( $Field, $Separator )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';
 
 return( "^ *".$Field." *$|".$Separator." *".$Field." *$|^ *".$Field." *".$Separator." *|".$Separator." *".$Field." *".$Separator." *" );
}

// --------------------------------------------------- 

function NormalizeCommaSeparatedField( $Field, $Separator )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Array = CommaSeparatedField2Array( $Field, $Separator );
 $NewField = "";

 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
  $NewField .= $Separator." ".$Array[ $i ];

 return( substr( $NewField, strlen( $Separator ) + 1 ) );
}

// --------------------------------------------------- 

function FoundInCommaSeparatedField( $Field, $Value, $Separator )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Array = CommaSeparatedField2Array( $Field, $Separator );

 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
  if( ( $Array[ $i ] == "any" ) || ( $Array[ $i ] == $Value ) )
   return( 1 );

 return( 0 );
}

// --------------------------------------------------- 

function Parse_XML( $XML, $Tag, &$Attributes )
{
 $StartPos = 0;
 return( Parse_XML_Extended( $XML, $Tag, $Attributes, $StartPos ) );
}

// --------------------------------------------------- 

function Parse_XML_Extended( $XML, $Tag, &$Attributes, &$StartPos )
{
 $XMLLength = strlen( $XML );
 $TagLength = strlen( $Tag );

 $Empty = "";

 if( $XMLLength <= 0 ) return( $Empty );
 if( $TagLength <= 0 ) return( $Empty );
 if( $StartPos >= $XMLLength ) return( $Empty );
 if( $StartPos < 0 ) return( $Empty );

 for( $Index = $StartPos; $Index < $XMLLength; ++$Index )
  if( $XML[ $Index ] == '<' )
  {

   $BeginTagEnd = $BeginTagStart = $Index + 1;

   while( $BeginTagEnd < $XMLLength  && $XML[ $BeginTagEnd ] != ' ' && $XML[ $BeginTagEnd ] != '>' ) ++$BeginTagEnd;

   if( $BeginTagEnd >= $XMLLength ) return( $Empty );

   $FoundTag = substr( $XML, $BeginTagStart, $BeginTagEnd - $BeginTagStart );
   $FoundTagLength = $BeginTagEnd - $BeginTagStart;

   if( $XML[ $BeginTagEnd ] == ' ' )
   {
    $AttributeStart = $BeginTagEnd + 1;

    while( $AttributeStart < $XMLLength  && $XML[ $AttributeStart ] == ' ' ) ++$AttributeStart;

    if( $AttributeStart >= $XMLLength ) return( $Empty );

    $AttributeEnd = $AttributeStart + 1;

    while( $AttributeEnd < $XMLLength  && $XML[ $AttributeEnd ] != '>' ) ++$AttributeEnd;

    if( $AttributeEnd >= $XMLLength ) return( $Empty );

    $FoundAttr = substr( $XML, $AttributeStart, $AttributeEnd - $AttributeStart );
   }
   else
    $AttributeStart = $AttributeEnd = $BeginTagEnd;

   $EndTagStart = strpos( $XML, "</".$FoundTag.">", $AttributeEnd + 1 );

   if( $EndTagStart === FALSE ) return( $Empty );

   $EndTagStart = $EndTagStart + 2;

   if( $EndTagStart >= $XMLLength ) return( $Empty );

   $EndTagEnd = $EndTagStart + $FoundTagLength;

   if( $EndTagEnd >= $XMLLength ) return( $Empty );

   if( $FoundTag == $Tag )
   {
    $StartPos = $EndTagEnd + 1;
    $Attributes = substr( $XML, $AttributeStart, $AttributeEnd - $AttributeStart );
    return( substr( $XML, $AttributeEnd + 1, $EndTagStart - $AttributeEnd - 3 ) );
   }

   $Index = $EndTagEnd;
  }

 return( $Empty );
}

// --------------------------------------------------- 

function NormalizeString( $String )
{
 $Start = 0;
 $End = strlen( $String );

 if( $End <= 0 ) return( "" );

 while( ( $String[ $Start ] == " " ) && ( $Start < $End ) ) $Start++;  
 if( $Start >= $End ) return( "" );

 while( ( $String[ $End - 1 ] == " " ) && ( $End > $Start ) ) $End--; 
 if( $End <= $Start ) return( "" );

 return( substr( $String, $Start, $End - $Start ) );
}

// --------------------------------------------------- 

function rmpath( $Path )
{
 if( is_dir( $Path ) )
 {
  $DirHandle = opendir( $Path );

  while( ( $Entry = readdir( $DirHandle ) ) !== FALSE )
   if( ( $Entry != "." ) && ( $Entry != ".." ) )
    rmpath( $Path."/".$Entry );

  closedir( $DirHandle );

  rmdir( $Path );
 }
 elseif( is_file( $Path ) )
  unlink( $Path );
} 

// --------------------------------------------------- 

?>
