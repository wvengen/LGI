/* []--------------------------------------------------------[]
    |                      xml.cpp                           |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 10 July 2007.                        |
    | USE:        Implements a basic xml parser...           |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "xml.h"

// -----------------------------------------------------------------------------

string Parse_XML( string XML, string Tag, string &Attributes, int &StartStop )
{
 int XMLLength = XML.length();
 int TagLength = Tag.length();
 int Index, BeginTagStart, BeginTagEnd,
     AttributeStart, AttributeEnd,
     EndTagStart, EndTagEnd, FoundTagLength;
 string Empty( "" );
 string FoundTag;

 if( XMLLength <= 0 ) return( Empty );
 if( TagLength <= 0 ) return( Empty );

 for( Index = StartStop; Index < XMLLength; ++Index )           
  if( XML[ Index ] == '<' )                             
  {
   
   BeginTagEnd = BeginTagStart = Index + 1;                                                                    // BeginTagStart marks the start of the tag 

   while( BeginTagEnd < XMLLength  && XML[ BeginTagEnd ] != ' ' && XML[ BeginTagEnd ] != '>' ) ++BeginTagEnd;  // BeginTagEnd marks the '>' or the ' ' 

   if( BeginTagEnd >= XMLLength ) { StartStop = XMLLength; Attributes = Empty; return( Empty ); }            

   FoundTag = XML.substr( BeginTagStart, BeginTagEnd - BeginTagStart );    
   FoundTagLength = BeginTagEnd - BeginTagStart;

   if( XML[ BeginTagEnd ] == ' ' )                                                               // if BeginTagEnd marks the ' ' there is an attribute 
   {
    AttributeStart = BeginTagEnd + 1;                                                     
    
    while( AttributeStart < XMLLength  && XML[ AttributeStart ] == ' ' ) ++AttributeStart;       // AttributeStart marks start of attribute

    if( AttributeStart >= XMLLength ) { StartStop = XMLLength; Attributes = Empty; return( Empty ); }

    AttributeEnd = AttributeStart + 1;

    while( AttributeEnd < XMLLength  && XML[ AttributeEnd ] != '>' ) ++AttributeEnd;             // AttributeEnd marks the '>' of the tag

    if( AttributeEnd >= XMLLength ) { StartStop = XMLLength; Attributes = Empty; return( Empty ); }
   }
   else
    AttributeStart = AttributeEnd = BeginTagEnd;

   EndTagStart = XML.find( "</" + FoundTag + ">", AttributeEnd + 1 ) + 2;               // EndTagStart marks start of the end tag

   if( EndTagStart >= XMLLength ) { StartStop = XMLLength; Attributes = Empty; return( Empty ); }

   EndTagEnd = EndTagStart + FoundTagLength;                                            // EndTagEnd marks the '>' of the end tag
   
   if( EndTagEnd >= XMLLength ) { StartStop = XMLLength; Attributes = Empty; return( Empty ); }

   if( FoundTag == Tag )                                                               // if this is the tag we want, return stuff 
   {
    Attributes = XML.substr( AttributeStart, AttributeEnd - AttributeStart );
    StartStop = EndTagEnd + 1;
    return( XML.substr( AttributeEnd + 1, EndTagStart - AttributeEnd - 3 ) );
   }
    
   Index = EndTagEnd;
  }
 
 StartStop = XMLLength; Attributes = Empty; return( Empty );
}

// -----------------------------------------------------------------------------

string Parse_XML( string XML, string Tag, string &Attributes )
{
 int StartStop = 0;
 return( Parse_XML( XML, Tag, Attributes, StartStop ) );
}

// -----------------------------------------------------------------------------

string Parse_XML( string XML, string Tag )
{
 string Attributes;
 int StartStop = 0;
 return( Parse_XML( XML, Tag, Attributes, StartStop ) );
}

// -----------------------------------------------------------------------------
