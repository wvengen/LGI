/* []--------------------------------------------------------[]
    |                 daemon_mainclass.h                     |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 20 August 2007.                      |
    | USE:        Implements main scheduler class...         |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__DAEMONMAINCLASS_INCLUDED__)
#define __DAEMONMAINCLASS_INCLUDED__

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <list>
#include <map>
#include <set>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>

#include <curl/curl.h>

#include "logger.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "daemon_jobclass.h"
#include "resource_server_api.h"

using namespace std;

// -----------------------------------------------------------------------------

class Daemon : public DaemonConfig
      {
       public:

               Daemon( string TheConfigFile, int SlowCycleTime = 600, int FastCycleTime = 120, bool Strict = true );
               ~Daemon( void );

               int RunSchedular( void );
               int IsSchedularReady( void );
               void StopScheduling( void );
               void ReloadConfigFile( void );
               void ResetcURLHandle( void );
               void ClosecURLHandle( void );

       protected:

               int CycleThroughJobs( void );
               int RequestWorkCycle( void );
               
               int ScanDirectoryForJobs( string Directory );
               int AddJobToDaemonLists( DaemonJob Job );
               int RemoveJobFromDaemonLists( DaemonJob Job );

               int IsOwnerDenied( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application );
               int IsOwnerRunningToMuch( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application );

       map<string,list<DaemonJob> > Jobs;
       map<string,int>              Accounting;
       set<string>                  ListOfActiveOwners;
       int                          JobsFinished,
                                    JobsObtained,
                                    ReadyForScheduling,
                                    ReloadConfig,
                                    TheFastCycleTime, 
                                    TheSlowCycleTime;
       CURL                        *MycURLHandle;
       string                       ConfigFile;
       bool                         CheckHostname;
      };

// -----------------------------------------------------------------------------

string EscapeSpacesInString( string Command );

#endif
