
// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include <string>
#include <iostream>

using namespace std;

/* -------------------------------------------------------------- */

string Parse_XML( string XML, string Tag, string &Attributes )
{
 int XMLLength = XML.length();
 int TagLength = Tag.length();
 int Index, BeginTagStart, BeginTagEnd,
     AttributeStart, AttributeEnd,
     EndTagStart, EndTagEnd, FoundTagLength;
 string Empty( "" );
 string FoundTag, FoundAttr;

 if( XMLLength <= 0 ) return( Empty );
 if( TagLength <= 0 ) return( Empty );

 for( Index = 0; Index < XMLLength; ++Index )           
  if( XML[ Index ] == '<' )                             
  {
   
   BeginTagEnd = BeginTagStart = Index + 1; 

   while( BeginTagEnd < XMLLength  && XML[ BeginTagEnd ] != ' ' && XML[ BeginTagEnd ] != '>' ) ++BeginTagEnd;

   if( BeginTagEnd >= XMLLength ) return( Empty );             

   FoundTag = XML.substr( BeginTagStart, BeginTagEnd - BeginTagStart );    
   FoundTagLength = BeginTagEnd - BeginTagStart;

   if( XML[ BeginTagEnd ] == ' ' )                             
   {
    AttributeStart = BeginTagEnd + 1;
    
    while( AttributeStart < XMLLength  && XML[ AttributeStart ] == ' ' ) ++AttributeStart;

    if( AttributeStart >= XMLLength ) return( Empty );

    AttributeEnd = AttributeStart + 1;

    while( AttributeEnd < XMLLength  && XML[ AttributeEnd ] != '>' ) ++AttributeEnd;

    if( AttributeEnd >= XMLLength ) return( Empty );

    FoundAttr = XML.substr( AttributeStart, AttributeEnd - AttributeStart ); 
   }
   else
    AttributeStart = AttributeEnd = BeginTagEnd;

   EndTagStart = XML.find( "</" + FoundTag + ">", AttributeEnd + 1 ) + 2;

   if( EndTagStart >= XMLLength ) return( Empty );

   EndTagEnd = EndTagStart + FoundTagLength;
   
   if( EndTagEnd >= XMLLength ) return( Empty );

   if( FoundTag == Tag )                                 
   {
    Attributes = XML.substr( AttributeStart, AttributeEnd - AttributeStart );
    return( XML.substr( AttributeEnd + 1, EndTagStart - AttributeEnd - 3 ) );
   }
    
   Index = EndTagEnd;
  }
 
 return( Empty ); 
}

/* -------------------------------------------------------------- */

int main( void )
{
 string Attrs;
 string tstxml1 = "<testtag>first value</testtag>";
 string tstxml2 = "<testtag><testtag2>second value</testtag2></testtag>";
 string tstxml3 = "<testtag>first value</testtag><testtag><testtag2>second value</testtag2></testtag>";
 string tstxml4 = "<testtag name=1>first value</testtag>";
 string tstxml5 = "<testtag name=2><testtag2 name=3 looks=1>second value</testtag2></testtag>";

 cout << "1 DATA: '" << Parse_XML( tstxml1, "testtag", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "2 DATA: '" << Parse_XML( tstxml2, "testtag", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "3 DATA: '" << Parse_XML( Parse_XML( tstxml2, "testtag", Attrs ), "testtag2", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "4 DATA: '" << Parse_XML( tstxml2, "testtag2", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "5 DATA: '" << Parse_XML( tstxml3, "testtag", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "6 DATA: '" << Parse_XML( tstxml3, "testtag2", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "7 DATA: '" << Parse_XML( tstxml4, "testtag", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "8 DATA: '" << Parse_XML( tstxml5, "testtag", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;
 cout << "9 DATA: '" << Parse_XML( Parse_XML( tstxml5, "testtag", Attrs ), "testtag2", Attrs ) <<  "' ATTR: '" << Attrs << "'" << endl;

 return( 0 );
}
