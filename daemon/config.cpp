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

 NORMAL_LOG( "DaemonConfig::DaemonConfig; Reading config file " << ConfigFile );

 ConfigurationXML.reserve( 8192 );         
 ConfigurationXML.clear();

 while( Cfg )                     // read in the file into the string...
 {
  getline( Cfg, Line );
  ConfigurationXML.append( Line );
 }

 DEBUG_LOG( "DaemonConfig::DaemonConfig; The configuration read in: " << ConfigurationXML );
}

// -----------------------------------------------------------------------------

string DaemonConfig::CA_Certificate_File( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "ca_certificate_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::CA_Certificate_File; No data in ca_certificate_file tag found" )
 else
  DEBUG_LOG_RETURN( Data, "DaemonConfig::CA_Certificate_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Name( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_name" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Name; No data in resource_name tag found" )
 else
  DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Name; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_URL( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_url" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_URL; No data in resource_url tag found" )
 else
  DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_URL; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Certificate_File( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_certificate_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Certificate_File; No data in resource_certificate_file tag found" )
 else
  DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Certificate_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Key_File( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "resource_key_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Key_File; No data in resource_key_file tag found" )
 else
  DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Key_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

int DaemonConfig::Number_Of_Projects( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( Parse_XML( ConfigurationXML, "LGI" ), "resource" ), "number_of_projects" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( 0, "DaemonConfig::Number_Of_Projects; No data in number_of_projects tag found" )
 else
  DEBUG_LOG_RETURN( atoi( Data.c_str() ), "DaemonConfig::Number_Of_Projects; Returned " << Data );
}

// -----------------------------------------------------------------------------

DaemonConfigProject DaemonConfig::Project( int Number )
{
 if( Number > Number_Of_Projects() )
  CRITICAL_LOG( "DaemonConfig::Project; Number > Number_Of_Projects" );
 
 if( Number <= 0 )
  CRITICAL_LOG( "DaemonConfig::Project; Number <= 0" );

 DaemonConfigProject TheProject( (*this), Number );

 DEBUG_LOG_RETURN( TheProject, "DaemonConfig::Project; Returned project with Number=" << Number );
}

// -----------------------------------------------------------------------------

DaemonConfigProject::DaemonConfigProject( DaemonConfig &TheConfig, int TheProjectNumber )
{
 Config = &TheConfig;
 ProjectNumber = TheProjectNumber;
 ProjectCache.clear();

 if( ProjectNumber > Config -> Number_Of_Projects() )
 {
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; TheProjectNumber > TheConfig -> Number_Of_Projects()" );
  return;
 }

 if( ProjectNumber <= 0 )
 {
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; TheProjectNumber <= 0" );
  return;
 }

 string ResourceCache = Parse_XML( Parse_XML( Config -> ConfigurationXML, "LGI" ), "resource" );

 DEBUG_LOG( "DaemonConfigProject::DaemonConfigProject; Cached resource data with ResourceCache=" << ResourceCache );

 string Attributes;
 int FoundNumber, StartPos = 0;

 do
 {
  ProjectCache = Parse_XML( ResourceCache, "project", Attributes, StartPos );

  DEBUG_LOG( "DaemonConfigProject::DaemonConfigProject; Cached project data with ProjectCache=" << ProjectCache );
 
  if( !ProjectCache.empty() )
   if( sscanf( Attributes.c_str(), "number=%d", &FoundNumber ) != 1 )
    CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; Error in project tag with Attributes=" << Attributes );

 } while ( ( StartPos < ResourceCache.length() ) && ( FoundNumber != ProjectNumber ) );

 if( FoundNumber != ProjectNumber )
 {
  ProjectCache.clear();
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; Project tag with number=" << ProjectNumber << " not found" );
 }

}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Project_Name( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Project_Master_Server( void )
{
}

// -----------------------------------------------------------------------------

int DaemonConfigProject::Number_Of_Applications( void )
{
}

// -----------------------------------------------------------------------------

DaemonConfigProjectApplication DaemonConfigProject::Application( int Number )
{
}

// -----------------------------------------------------------------------------

DaemonConfigProjectApplication::DaemonConfigProjectApplication( DaemonConfig &TheConfig, int TheProjectNumber, int TheApplicationNumber )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Application_Name( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Owner_Allow( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Owner_Deny( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Check_System_Limits_Script( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Check_Limits_Script( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Check_Running_Script( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Prologue_Script( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Run_Script( void )
{
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Epilogue_Script( void )
{
}

// -----------------------------------------------------------------------------
