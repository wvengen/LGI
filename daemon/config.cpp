/* []--------------------------------------------------------[]
    |                      config.cpp                        |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 2 August 2007.                       |
    | USE:        Implements config file class...            |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "config.h"

// -----------------------------------------------------------------------------

DaemonConfig::DaemonConfig( string ConfigFile )
{
 fstream Cfg( ConfigFile.c_str(), fstream::in );
 string Line;

 ConfigurationXML.reserve( 8192 ); 
 ConfigurationXML.clear();

 while( Cfg )
 {
  getline( Cfg, Line );
  Line.append( "\n" );
  ConfigurationXML.append( Line );
 }
}

// -----------------------------------------------------------------------------

string DaemonConfig::CA_Certificate_File( void )
{
 return( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "ca_certificate_file" ) );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Name( void )
{
 return( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_name" ) );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_URL( void )
{
 return( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_url" ) );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Certificate_File( void )
{
 return( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_certificate_file" ) );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Key_File( void )
{
 return( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_key_file" ) );
}

// -----------------------------------------------------------------------------

int DaemonConfig::Number_Of_Projects( void )
{
 return( atoi( NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "number_of_projects" ) ).c_str() ) );
}

// -----------------------------------------------------------------------------

