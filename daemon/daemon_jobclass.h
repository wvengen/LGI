/* []--------------------------------------------------------[]
    |                    daemon_jobclass.h                   |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 14 August 2007.                       |
    | USE:        Implements job class...                    |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#if !defined(__DAEMONJOBCLASS_INCLUDED__)
#define __DAEMONJOBCLASS_INCLUDED__

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "logger.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "daemon_config.h"

using namespace std;

// -----------------------------------------------------------------------------

class DaemonJob
      {
       public:

              DaemonJob();
              DaemonJob( string TheJobDirectory );
              DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber );

       private:

              string ReadStringFromFile( string FileName );
              void   WriteStringToFile( string String, string FileName );

       protected:

       string JobDirectory;
      };

// -----------------------------------------------------------------------------

#endif
