/* []--------------------------------------------------------[]
    |                  user_configclass.h                    |
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

#if !defined(__USERCONFIGCLASS_INCLUDED__)
#define __USERCONFIGCLASS_INCLUDED__

#include <string>

#include "logger.h"
#include "csv.h"
#include "xml.h"
#include "util.h"

using namespace std;

// -----------------------------------------------------------------------------

class UserConfig;

// -----------------------------------------------------------------------------

class UserConfig
      {
       public: 

              UserConfig( string Config = "" );

              // read a configuration file or directory
              void Read( string Config = "" );

              string User( void );
              string Groups( void );
              string DefaultServer( void );
              string DefaultProject( void );

              string CA_ChainFile( void );
              string CertificateFile( void );
              string PrivateKeyFile( void );

       private:

              string user, groups, defaultserver, defaultproject;
              string ca_chain_file, certificate_file, privatekey_file;

              void ReadDirectory( string ConfigDir );
              void ReadFile( string ConfigFile );
              void OverrideFromFile( string &to, string ConfigFile );
      };

// -----------------------------------------------------------------------------

#endif /* __USERCONFIGCLASS_INCLUDED__ */
