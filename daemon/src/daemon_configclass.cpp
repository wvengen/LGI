/* []--------------------------------------------------------[]
    |                 daemon_configclass.cpp                 |
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

#include "daemon_configclass.h"

// -----------------------------------------------------------------------------

DaemonConfig::DaemonConfig( string ConfigFile )
{
 NORMAL_LOG( "DaemonConfig::DaemonConfig; Reading config file " << ConfigFile );

 ConfigurationXML = ReadStringFromFile( ConfigFile );

 DEBUG_LOG( "DaemonConfig::DaemonConfig; The configuration read in: " << ConfigurationXML );

 ConfigurationXML = NormalizeString( Parse_XML( ConfigurationXML, "LGI" ) );

 if( ConfigurationXML.empty() )
  CRITICAL_LOG( "DaemonConfig::DaemonConfig; No data in LGI tag found" );
}

// -----------------------------------------------------------------------------

int DaemonConfig::IsValidConfigured( void )
{
 if( ConfigurationXML.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; ConfigurationXML cache empty" );

 if( CA_Certificate_File().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; CA_Certificate_File() empty" );
 if( ReadStringFromFile( CA_Certificate_File() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Cannot read from file " << CA_Certificate_File() );

 if( Resource_Key_File().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Resource_Key_File() empty" );
 if( ReadStringFromFile( Resource_Key_File() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Cannot read form file " << Resource_Key_File() );

 if( Resource_Certificate_File().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Resource_Certificate_File() empty" );
 if( ReadStringFromFile( Resource_Certificate_File() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Cannot read form file " << Resource_Certificate_File() );

 if( Resource_Name().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Resource_Name() empty" );
 if( Resource_URL().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Resource_URL() empty" );
 if( RunDirectory().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; RunDirectory() empty" );
 if( Owner_Allow().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Owner_Allow() empty" );
 if( Owner_Deny().empty() ) CRITICAL_LOG( "DaemonConfig::IsValidConfigured; Warning: Owner_Deny() empty" );
 if( !Number_Of_Projects() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Number_Of_Projects() returned 0" );

 for( int p = 1; p <= Number_Of_Projects(); ++p )
  if( !Project( p ).IsValidConfigured() ) CRITICAL_LOG_RETURN( 0, "DaemonConfig::IsValidConfigured; Project not configured correctly for project number " << p );

 NORMAL_LOG_RETURN( 1, "DaemonConfig::IsValidConfigured; Configuration tested valid" );
}

// -----------------------------------------------------------------------------

string DaemonConfig::CA_Certificate_File( void )
{
 string Data = NormalizeString( Parse_XML( ConfigurationXML, "ca_certificate_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::CA_Certificate_File; No data in ca_certificate_file tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::CA_Certificate_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Name( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "resource_name" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Name; No data in resource_name tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Name; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_URL( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "resource_url" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_URL; No data in resource_url tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_URL; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Certificate_File( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "resource_certificate_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Certificate_File; No data in resource_certificate_file tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Certificate_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Resource_Key_File( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "resource_key_file" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Resource_Key_File; No data in resource_key_file tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Resource_Key_File; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::RunDirectory( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "run_directory" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::RunDirectory; No data in run_directory tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::RunDirectory; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Owner_Allow( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "owner_allow" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Owner_Allow; No data in owner_allow tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Owner_Allow; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfig::Owner_Deny( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "owner_deny" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfig::Owner_Deny; No data in owner_deny tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfig::Owner_Deny; Returned " << Data );
}

// -----------------------------------------------------------------------------

int DaemonConfig::Number_Of_Projects( void )
{
 string Data = NormalizeString( Parse_XML( Parse_XML( ConfigurationXML, "resource" ), "number_of_projects" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( 0, "DaemonConfig::Number_Of_Projects; No data in number_of_projects tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( atoi( Data.c_str() ), "DaemonConfig::Number_Of_Projects; Returned " << Data );
}

// -----------------------------------------------------------------------------

DaemonConfigProject DaemonConfig::Project( int Number )
{
 DaemonConfigProject EmptyProject;

 if( Number > Number_Of_Projects() )
  CRITICAL_LOG_RETURN( EmptyProject, "DaemonConfig::Project; Number > Number_Of_Projects" );
 
 if( Number <= 0 )
  CRITICAL_LOG_RETURN( EmptyProject, "DaemonConfig::Project; Number <= 0" );

 DaemonConfigProject TheProject( (*this), Number );

 VERBOSE_DEBUG_LOG_RETURN( TheProject, "DaemonConfig::Project; Returned project with Number=" << Number );
}

// -----------------------------------------------------------------------------

DaemonConfigProject::DaemonConfigProject( void )
{
 ProjectCache.clear();
}

// -----------------------------------------------------------------------------

DaemonConfigProject::DaemonConfigProject( DaemonConfig &TheConfig, int TheProjectNumber )
{
 ProjectCache.clear();

 if( TheProjectNumber > TheConfig.Number_Of_Projects() )
 {
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; TheProjectNumber > TheConfig.Number_Of_Projects()" );
  return;
 }

 if( TheProjectNumber <= 0 )
 {
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; TheProjectNumber <= 0" );
  return;
 }

 string ResourceCache = Parse_XML( TheConfig.ConfigurationXML, "resource" );

 VERBOSE_DEBUG_LOG( "DaemonConfigProject::DaemonConfigProject; Cached resource data with ResourceCache=" << ResourceCache );

 string Attributes;
 int FoundNumber, StartPos = 0;

 do
 {
  ProjectCache = Parse_XML( ResourceCache, "project", Attributes, StartPos );

  VERBOSE_DEBUG_LOG( "DaemonConfigProject::DaemonConfigProject; Cached project data with ProjectCache=" << ProjectCache );
 
  if( !ProjectCache.empty() )
   if( sscanf( Attributes.c_str(), "number='%d'", &FoundNumber ) != 1 )
    if( sscanf( Attributes.c_str(), "number=\"%d\"", &FoundNumber ) != 1 )
     CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; Error in project tag with Attributes=" << Attributes );

 } while ( ( StartPos < ResourceCache.length() ) && ( FoundNumber != TheProjectNumber ) );

 if( FoundNumber != TheProjectNumber )
 {
  ProjectCache.clear();
  CRITICAL_LOG( "DaemonConfigProject::DaemonConfigProject; Project tag with number=" << TheProjectNumber << " not found, cache cleared" );
 }

}

// -----------------------------------------------------------------------------

int DaemonConfigProject::IsValidConfigured( void )
{
 if( Project_Name().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::IsValidConfigured; Project_Name() empty" );
 if( Project_Master_Server().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::IsValidConfigured; Project_Master_Server() empty" );
 if( Owner_Allow().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::IsValidConfigured; Owner_Allow() empty" );
 if( Owner_Deny().empty() ) CRITICAL_LOG( "DaemonConfigProject::IsValidConfigured; Warning: Owner_Deny() empty" );
 if( !Number_Of_Applications() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::IsValidConfigured; Number_Of_Applications()returned 0" );

 for( int a = 1; a <= Number_Of_Applications(); ++a )
  if( !Application( a ).IsValidConfigured() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::IsValidConfigured; Application not configured corrctly for application number " << a );

 NORMAL_LOG_RETURN( 1, "DaemonConfigProject::IsValidConfigured; Configuration tested valid" );
}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Project_Name( void )
{
 string Data = NormalizeString( Parse_XML( ProjectCache, "project_name" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProject::Project_Name; No data in project_name tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProject::Project_Name; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Project_Master_Server( void )
{
 string Data = NormalizeString( Parse_XML( ProjectCache, "project_master_server" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProject::Project_Master_Server; No data in project_master_server tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProject::Project_Master_Server; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Owner_Allow( void )
{
 string Data = NormalizeString( Parse_XML( ProjectCache, "owner_allow" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProject::Owner_Allow; No data in owner_allow tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProject::Owner_Allow; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProject::Owner_Deny( void )
{
 string Data = NormalizeString( Parse_XML( ProjectCache, "owner_deny" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProject::Owner_Deny; No data in owner_deny tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProject::Owner_Deny; Returned " << Data );
}

// -----------------------------------------------------------------------------

int DaemonConfigProject::Number_Of_Applications( void )
{
 string Data = NormalizeString( Parse_XML( ProjectCache, "number_of_applications" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( 0, "DaemonConfigProject::Number_Of_Applications; No data in number_of_applications tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( atoi( Data.c_str() ), "DaemonConfigProject::Number_Of_Applications; Returned " << Data );
}

// -----------------------------------------------------------------------------

DaemonConfigProjectApplication DaemonConfigProject::Application( int Number )
{
 DaemonConfigProjectApplication EmptyApplication;

 if( Number > Number_Of_Applications() )
  CRITICAL_LOG_RETURN( EmptyApplication, "DaemonConfigProject::Application; Number > Number_Of_Applications" );

 if( Number <= 0 )
  CRITICAL_LOG_RETURN( EmptyApplication, "DaemonConfigProject::Application; Number <= 0" );

 DaemonConfigProjectApplication TheApplication( (*this), Number );

 VERBOSE_DEBUG_LOG_RETURN( TheApplication, "DaemonConfigProject::Application; Returned application with Number=" << Number );
}

// -----------------------------------------------------------------------------

DaemonConfigProjectApplication::DaemonConfigProjectApplication( void )
{
 ApplicationCache.clear();
}

// -----------------------------------------------------------------------------

DaemonConfigProjectApplication::DaemonConfigProjectApplication( DaemonConfigProject &TheProject, int TheApplicationNumber )
{
 ApplicationCache.clear();

 if( TheApplicationNumber > TheProject.Number_Of_Applications() )
 {
  CRITICAL_LOG( "DaemonConfigProjectApplication::DaemonConfigProjectApplication; TheApplicationNumber > TheProject.Number_Of_Applications()" );
  return;
 }

 if( TheApplicationNumber <= 0 )
 {
  CRITICAL_LOG( "DaemonConfigProjectApplication::DaemonConfigProjectApplication; TheApplicationNumber <= 0" );
  return;
 }

 string Attributes;
 int FoundNumber, StartPos = 0;

 do
 {
  ApplicationCache = Parse_XML( TheProject.ProjectCache, "application", Attributes, StartPos );

  VERBOSE_DEBUG_LOG( "DaemonConfigProjectApplication::DaemonConfigProjectApplication; Cached application data with ApplicationCache=" << ApplicationCache );

  if( !ApplicationCache.empty() )
   if( sscanf( Attributes.c_str(), "number='%d'", &FoundNumber ) != 1 )
    if( sscanf( Attributes.c_str(), "number=\"%d\"", &FoundNumber ) != 1 )
     CRITICAL_LOG( "DaemonConfigProjectApplication::DaemonConfigProjectApplication; Error in application tag with Attributes=" << Attributes );

 } while ( ( StartPos < TheProject.ProjectCache.length() ) && ( FoundNumber != TheApplicationNumber ) );

 if( FoundNumber != TheApplicationNumber )
 {
  ApplicationCache.clear();
  CRITICAL_LOG( "DaemonConfigProjectApplication::DaemonConfigProjectApplication; Application tag with number=" << TheApplicationNumber << " not found, cache cleared" );
 } 

}

// -----------------------------------------------------------------------------

int DaemonConfigProjectApplication::IsValidConfigured( void )
{
 if( Application_Name().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Application_Name() empty" );
 if( Owner_Allow().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Owner_Allow() empty" );
 if( Owner_Deny().empty() ) CRITICAL_LOG( "DaemonConfigProjectApplication::IsValidConfigured; Warning: Owner_Deny() empty" );

 if( Check_System_Limits_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Check_System_Limits_Script() empty" );
 if( Job_Check_Limits_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Check_Limits_Script() empty" );
 if( Job_Check_Running_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Check_Running_Script() empty" );
 if( Job_Check_Finished_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Check_Finished_Script() empty" );
 if( Job_Prologue_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Prologue_Script() empty" );
 if( Job_Run_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Run_Script() empty" );
 if( Job_Epilogue_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Epilogue_Script() empty" );
 if( Job_Abort_Script().empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Job_Abort_Script() empty" );

 if( ReadStringFromFile( Check_System_Limits_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Check_System_Limits_Script() );
 if( ReadStringFromFile( Job_Check_Running_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Check_Running_Script() );
 if( ReadStringFromFile( Job_Check_Finished_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Check_Finished_Script() );
 if( ReadStringFromFile( Job_Prologue_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Prologue_Script() );
 if( ReadStringFromFile( Job_Epilogue_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Epilogue_Script() );
 if( ReadStringFromFile( Job_Run_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Run_Script() );
 if( ReadStringFromFile( Job_Abort_Script() ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonConfigProjectApplication::IsValidConfigured; Cannot read from file " << Job_Abort_Script() );

 NORMAL_LOG_RETURN( 1, "DaemonConfigProjectApplication::IsValidConfigured; Configuration tested valid" );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Application_Name( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "application_name" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Application_Name; No data in application_name tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Application_Name; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Owner_Allow( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "owner_allow" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Owner_Allow; No data in owner_allow tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Owner_Allow; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Owner_Deny( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "owner_deny" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Owner_Deny; No data in owner_deny tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Owner_Deny; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Check_System_Limits_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "check_system_limits_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Check_System_Limits_Script; No data in check_system_limits_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Check_System_Limits_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Check_Limits_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_check_limits_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Limits_Script; No data in job_check_limits_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Limits_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Check_Running_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_check_running_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Running_Script; No data in job_check_running_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Running_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Check_Finished_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_check_finished_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Finished_Script; No data in job_check_finished_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Check_Finished_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Prologue_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_prologue_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Prologue_Script; No data in job_prologue_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Prologue_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Run_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_run_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Run_Script; No data in job_run_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Run_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Epilogue_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_epilogue_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Epilogue_Script; No data in job_epilogue_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Epilogue_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonConfigProjectApplication::Job_Abort_Script( void )
{
 string Data = NormalizeString( Parse_XML( ApplicationCache, "job_abort_script" ) );
 if( Data.empty() )
  CRITICAL_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Abort_Script; No data in job_abort_script tag found" )
 else
  VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonConfigProjectApplication::Job_Abort_Script; Returned " << Data );
}

// -----------------------------------------------------------------------------

string ReadStringFromFile( string FileName )
{
 fstream File( FileName.c_str(), fstream::in );
 string Buffer, Line;

 Buffer.reserve( 1024 );
 Buffer.clear();

 getline( File, Line );
 while( File )
 {
  Buffer.append( Line );
  Buffer.push_back( '\n' );
  getline( File, Line );
 }

 if( Buffer.length() >= 1 )
  Buffer = Buffer.substr( 0, Buffer.length() - 1 );
 else
  Buffer.clear();

 VERBOSE_DEBUG_LOG_RETURN( Buffer, "ReadStringFromFile; Data returned from file " << FileName << ": '" << Buffer << "'" );
}

// -----------------------------------------------------------------------------

void WriteStringToFile( string String, string FileName )
{
 fstream File( FileName.c_str(), fstream::out );

 File << String;

 VERBOSE_DEBUG_LOG( "WriteStringToFile; Wrote file " << FileName << " with String=" << String );
}

// -----------------------------------------------------------------------------
