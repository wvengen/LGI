/* []--------------------------------------------------------[]
    |                 daemon_mainclass.h                     |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

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

               Daemon( string ConfigFile );

       protected:

               map<string,list<DaemonJob> > Jobs;
               map<string,int> Accounting;
      };

// -----------------------------------------------------------------------------

#endif
