/* []--------------------------------------------------------[]
    |                 user_configclass.cpp                   |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     W. van Engen                               |
    | VERSION:    1.00, 31 January 2012.                     |
    | USE:        User configuration access...               |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2012 W. van Engen, Nikhef / Stichting FOM
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "user_configclass.h"

// -----------------------------------------------------------------------------

UserConfig::UserConfig( string Config )
{
  Read( Config );
}

// -----------------------------------------------------------------------------

void UserConfig::Read( string Config )
{
 VERBOSE_DEBUG_LOG( "UserConfig::Read; Reading configuration " << Config );

 // use default if no argument given:
 //  - environment variable LGI_CONFIG, or if not set
 //  - ~/.LGI, or if not a valid directory
 //  - ~/LGI.cfg
 if( Config.empty() )
 {
  const char *lgi_config = getenv( "LGI_CONFIG" );
  if( lgi_config )
  {
   Config = string( lgi_config );
  }
  else
  {
   Config = string( getenv("HOME") ) + "/.LGI";
   if( !DirectoryExists( Config ) )
   {
    Config = string( getenv("HOME") ) + "/LGI.cfg";
    if( !FileExists( Config ) )
    {
     CRITICAL_LOG( "UserConfig::Read; no user configuration found" );
     return;
    }
   }
  }
 }
  
 // read either directory or file
 if( DirectoryExists( Config ) )
 {
  ReadDirectory( Config );
 }
 else
 {
  ReadFile( Config );
 }
}

// -----------------------------------------------------------------------------

void UserConfig::ReadDirectory( string ConfigDir )
{
 DEBUG_LOG( "UserConfig::ReadDirectory; Reading configuration from directory " << ConfigDir );

 string OptConfigFile = ConfigDir + "/LGI.cfg";
 if( FileExists( OptConfigFile ) )
 {
  DEBUG_LOG( "UserConfig::ReadDirectory; LGI.cfg present, reading that first" );
  ReadFile( OptConfigFile );
 }

 OverrideFromFile( user, ConfigDir + "/user" );
 OverrideFromFile( groups, ConfigDir + "/groups" );
 OverrideFromFile( defaultserver, ConfigDir + "/defaultserver" );
 OverrideFromFile( defaultproject, ConfigDir + "/defaultproject" );

 if( FileExists( ConfigDir + "/privatekey" ) )
  privatekey_file = ConfigDir + "/privatekey";
 if( FileExists( ConfigDir + "/certificate" ) )
  certificate_file = ConfigDir + "/certificate";
 if( FileExists( ConfigDir + "/ca_chain" ) )
  ca_chain_file = ConfigDir + "/ca_chain";
}

// -----------------------------------------------------------------------------

void UserConfig::ReadFile( string ConfigFile )
{
 string ConfigurationXML = ReadStringFromFile( ConfigFile );

 DEBUG_LOG( "UserConfig::ReadFile; The configuration read in: " << ConfigurationXML );

 ConfigurationXML = NormalizeString( Parse_XML( ConfigurationXML, "LGI_user_config" ) );

 if( ConfigurationXML.empty() )
  CRITICAL_LOG( "UserConfig::ReadFile; No valid LGI user config: " << ConfigFile );

 user = NormalizeString( Parse_XML( ConfigurationXML, "user" ) );
 groups = NormalizeString( Parse_XML( ConfigurationXML, "groups" ) );
 defaultserver = NormalizeString( Parse_XML( ConfigurationXML, "defaultserver" ) );
 defaultproject = NormalizeString( Parse_XML( ConfigurationXML, "defaultproject" ) );
 // assuming correct order of elements: certificate before chain
 privatekey_file = ConfigFile;
 certificate_file = ConfigFile;
 ca_chain_file = ConfigFile;
}

// -----------------------------------------------------------------------------

void UserConfig::OverrideFromFile( string &to, string ConfigFile )
{
 if( FileExists( ConfigFile ) )
 {
   to = ReadLineFromFile( ConfigFile );
   VERBOSE_DEBUG_LOG( "UserConfig::OverrideFromFile; Returned " << to << " from " << ConfigFile );
 }
}

// -----------------------------------------------------------------------------

string UserConfig::User( void )
{
 VERBOSE_DEBUG_LOG_RETURN( user, "UserConfig::User; Returned " << user );
}

// -----------------------------------------------------------------------------

string UserConfig::Groups( void )
{
 VERBOSE_DEBUG_LOG_RETURN( groups, "UserConfig::Groups; Returned " << groups );
}

// -----------------------------------------------------------------------------

string UserConfig::DefaultServer( void )
{
 VERBOSE_DEBUG_LOG_RETURN( defaultserver, "UserConfig::DefaultServer; Returned " << defaultserver );
}

// -----------------------------------------------------------------------------

string UserConfig::DefaultProject( void )
{
 VERBOSE_DEBUG_LOG_RETURN( defaultproject, "DefaultProjectConfig::DefaultProject; Returned " << defaultproject );
}

// -----------------------------------------------------------------------------

string UserConfig::CA_ChainFile( void )
{
 VERBOSE_DEBUG_LOG_RETURN( ca_chain_file, "CA_ChainConfig::CA_Chain; Returned " << ca_chain_file );
}

// -----------------------------------------------------------------------------

string UserConfig::CertificateFile( void )
{
 VERBOSE_DEBUG_LOG_RETURN( certificate_file, "CertificateConfig::Certificate; Returned " << certificate_file );
}

// -----------------------------------------------------------------------------

string UserConfig::PrivateKeyFile( void )
{
 VERBOSE_DEBUG_LOG_RETURN( privatekey_file, "PrivateKeyConfig::PrivateKey; Returned " << privatekey_file );
}

// -----------------------------------------------------------------------------
