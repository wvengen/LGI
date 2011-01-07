/* []--------------------------------------------------------[]
    |                 daemon_configclass.h                   |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
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

#if !defined(__DAEMONCONFIGCLASS_INCLUDED__)
#define __DAEMONCONFIGCLASS_INCLUDED__

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>

#include "logger.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"

using namespace std;

// -----------------------------------------------------------------------------

class DaemonConfig;
class DaemonConfigProject;
class DaemonConfigProjectApplication;

// -----------------------------------------------------------------------------

class DaemonConfig
      {
       public: 

              DaemonConfig( string ConfigFile );

              int    IsValidConfigured( void );
              string CA_Certificate_File( void );
              string Resource_Certificate_File( void );
              string Resource_Key_File( void );
              string RunDirectory( void );
              string Owner_Allow( void );
              string Owner_Deny( void );
              int    Number_Of_Projects( void );
              int    Job_Limit( void );

              DaemonConfigProject Project( int Number );

              friend class DaemonConfigProject;
              friend class DaemonConfigProjectApplication;

       protected:

       string ConfigurationXML;
      };

// -----------------------------------------------------------------------------

class DaemonConfigProject
      {
       public:

              DaemonConfigProject();
              DaemonConfigProject( DaemonConfig &TheConfig, int TheProjectNumber = 0 );

              int    IsValidConfigured( void );
              string Project_Name( void );
              string Project_Master_Server( void );
              string Owner_Allow( void );
              string Owner_Deny( void );
              int    Number_Of_Applications( void );
              int    Job_Limit( void );

              DaemonConfigProjectApplication Application( int Number );
              
              friend class DaemonConfigProjectApplication;

       protected:

       string ProjectCache;
      };

// -----------------------------------------------------------------------------

class DaemonConfigProjectApplication
      {
       public:
              
              DaemonConfigProjectApplication();
              DaemonConfigProjectApplication( DaemonConfigProject &TheProject, int TheApplicationNumber = 0 );

              int    IsValidConfigured( void );
              string Application_Name( void );
              string Owner_Allow( void );
              string Owner_Deny( void );
              string Check_System_Limits_Script( void );
              string Job_Check_Limits_Script( void );
              string Job_Check_Running_Script( void );
              string Job_Check_Finished_Script( void );
              string Job_Prologue_Script( void );
              string Job_Run_Script( void );
              string Job_Epilogue_Script( void );
              string Job_Abort_Script( void );
              string Capabilities( void );
              int    Job_Limit( void );
              int    Max_Output_Size( void );
              int    Job_Sandbox_UID( void );

       protected:

       string ApplicationCache;
      };

// -----------------------------------------------------------------------------

string ReadStringFromFile( string FileName, int MaxSize = -1 );
void   WriteStringToFile( string String, string FileName );
string AbsolutePath( string FileName );

// -----------------------------------------------------------------------------

#endif
