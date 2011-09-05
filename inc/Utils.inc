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

function NormalizeString( $String )
{
 $Start = 0;
 $End = strlen( $String );

 if( $End <= 0 ) return( "" );

 while( ( $Start < $End ) && ( $String[ $Start ] == " " ) ) $Start++;
 if( $Start >= $End ) return( "" );

 while( ( $End > $Start ) && ( $String[ $End - 1 ] == " " ) ) $End--;
 if( $End <= $Start ) return( "" );

 return( substr( $String, $Start, $End - $Start ) );
}

// --------------------------------------------------- 

function CommaSeparatedField2Array( $Field, $Separator = ',' )
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

 // now remove possible leading and trailing spaces from all the elements in the array...
 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
 {
  $Array[ $i ] = NormalizeString( $Array[ $i ] );

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

function MakeRegularExpressionForCommaSeparatedField( $Field, $Separator = ',' )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';
 
 return( "^ *".$Field." *$|".$Separator." *".$Field." *$|^ *".$Field." *".$Separator." *|".$Separator." *".$Field." *".$Separator." *" );
}

// --------------------------------------------------- 

function NormalizeCommaSeparatedField( $Field, $Separator = ',' )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Array = CommaSeparatedField2Array( $Field, $Separator );
 $NewField = "";

 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
  $NewField .= $Separator." ".$Array[ $i ];

 return( substr( $NewField, strlen( $Separator ) + 1 ) );
}

// --------------------------------------------------- 

function FoundInCommaSeparatedField( $Field, $Value, $Separator = ',' )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Array = CommaSeparatedField2Array( $Field, $Separator );
 $Value = NormalizeString( $Value );

 for( $i = 1; $i <= $Array[ 0 ]; $i++ )
  if( ( $Array[ $i ] == "any" ) || ( $Array[ $i ] == $Value ) )
   return( 1 );

 return( 0 );
}

// --------------------------------------------------- 

function RemoveDoubleEntriesFromCommaSeparatedField( $Field, $Separator = ',' )
{
 if( !isset( $Separator ) || ( $Separator == "" ) ) $Separator = ',';

 $Array = CommaSeparatedField2Array( $Field, $Separator );

 if( $Array[ 0 ] <= 1 ) return( $Field );

 $NewList = $Array[ 1 ];
 $AccountedFor[ 0 ] = 1;
 $AccountedFor[ 1 ] = $Array[ 1 ];

 for( $i = 2; $i <= $Array[ 0 ]; $i++ )
 {
  for( $j = 1; $j <= $AccountedFor[ 0 ]; $j++ )
   if( $AccountedFor[ $j ] == $Array[ $i ] ) break;

  if( $j <= $AccountedFor[ 0 ] ) continue;

  $NewList .= $Separator." ".$Array[ $i ];

  $AccountedFor[ 0 ]++;
  $AccountedFor[ $AccountedFor[ 0 ] ] = $Array[ $i ];
 }

 return( $NewList );
}

// --------------------------------------------------- 

function Parse_XML( $XML, $Tag, &$Attributes )
{
 $StartPos = 0;
 return( Parse_XML_Extended( $XML, $Tag, $Attributes, $StartPos ) );
}

// --------------------------------------------------- 

function XML_Escape( $text )
{
 return( str_replace( '&#039;', '&apos;', htmlspecialchars( $text, ENT_QUOTES ) ) );
}

// --------------------------------------------------- 

function XML_UnEscape( $text )
{
 return( htmlspecialchars_decode( str_replace( '&apos;', '&#039;', $text ), ENT_QUOTES ) );
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

   if( $XML[ $BeginTagStart ] == '/' ) return( $Empty );

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
    return( XML_UnEscape( substr( $XML, $AttributeEnd + 1, $EndTagStart - $AttributeEnd - 3 ) ) );
   }

   $Index = $EndTagEnd;
  }

 return( $Empty );
}

// --------------------------------------------------- 

function XML_Sanity_Check( $Data )
{
 $Length = strlen( $Data );
 $Stack[ 0 ] = 0;

 for( $i = 0; $i < $Length - 1; ++$i )
  if( $Data[ $i ] == '<' )    // located start of a tag...
  {
   // found a closing tag...
   if( $Data[ $i + 1 ] == '/' )
   {
    $EndTag = $i + 2;
    while( $EndTag < $Length  && $Data[ $EndTag ] != '>' ) ++$EndTag;
    if( $EndTag >= $Length ) return( True );

    // check if found closing tag matches last pushed tag on stack...
    if( substr( $Data, $i + 2, $EndTag - $i - 2 ) !== $Stack[ $Stack[ 0 ] ] ) return( True );

    // pop entry from stack and continue...
    --$Stack[ 0 ];
    $i = $EndTag;
    continue;
   }

   // found a starting tag...
   $EndTag = $i + 1;
   while( $EndTag < $Length  && $Data[ $EndTag ] != ' ' && $Data[ $EndTag ] != '>' ) ++$EndTag;
   if( $EndTag >= $Length ) return( True );

   // push it onto stack and continue the scan...
   $Stack[ $Stack[ 0 ] + 1 ] = substr( $Data, $i + 1, $EndTag - $i - 1 );
   ++$Stack[ 0 ];
   $i = $EndTag;   // just scan through the possible attribute...
  }

 // check if all tags were nicely closed...
 if( $Stack[ 0 ] != 0 ) return( True );

 return( False );
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
