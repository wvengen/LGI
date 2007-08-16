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
#include <cstring>
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
#include "hash.h"
#include "daemon_configclass.h"

using namespace std;

// ------------ some handy defines -------------------------------------------

#define HASHFILE_EXTENTION                         ".hash"

#define LGI_JOBDAEMON_PROJECT_FILE                 "LGI_project"
#define LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE     "LGI_this_project_server"
#define LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE   "LGI_project_master_server"
#define LGI_JOBDAEMON_APPLICATION_FILE             "LGI_application"
#define LGI_JOBDAEMON_JOB_ID_FILE                  "LGI_job_id"
#define LGI_JOBDAEMON_OWNERS_FILE                  "LGI_owners"
#define LGI_JOBDAEMON_READ_ACCESS_FILE             "LGI_read_access"
#define LGI_JOBDAEMON_JOB_SPECIFICS_FILE           "LGI_job_specifics"
#define LGI_JOBDAEMON_TARGET_RESOURCES_FILE        "LGI_target_resources"
#define LGI_JOBDAEMON_INPUT_FILE                   "LGI_input"
#define LGI_JOBDAEMON_OUTPUT_FILE                  "LGI_output"
#define LGI_JOBDAEMON_STATE_FILE                   "LGI_state"
#define LGI_JOBDAEMON_STATE_TIME_STAMP_FILE        "LGI_state_time_stamp"

#define LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT      "LGI_job_check_limits_script"
#define LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT     "LGI_job_check_running_script"
#define LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT    "LGI_job_check_finished_script"
#define LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT          "LGI_job_prologue_script"
#define LGI_JOBDAEMON_JOB_RUN_SCRIPT               "LGI_job_run_script"
#define LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT          "LGI_job_epilogue_script"
#define LGI_JOBDAEMON_JOB_ABORT_SCRIPT             "LGI_job_abort_script"

#define LGI_JOBDAEMON_KEY_FILE                     "LGI_key_file"
#define LGI_JOBDAEMON_CERTIFICATE_FILE             "LGI_certificate_file"
#define LGI_JOBDAEMON_CA_CERTIFICATE_FILE          "LGI_ca_certificate_file"

// -----------------------------------------------------------------------------

class DaemonJob
      {
       public:

              DaemonJob();
              DaemonJob( string TheJobDirectory );
              DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber );

              void CleanUpJobDirectory( void );

              string GetJobDirectory( void );
              string GetProject( void );
              string GetThisProjectServer( void );
              string GetProjectMasterServer( void );
              string GetApplication( void );
              string GetJobId( void );
              string GetOwners( void );
              string GetReadAccess( void );
              string GetJobSpecifics( void );          
              string GetTargetResources( void );       
              string GetInput( void );                 
              string GetOutput( void );                
              string GetState( void );                 
              string GetStateTimeStamp( void );        
              string GetKeyFile( void );
              string GetCertificateFile( void );
              string GetCACertificateFile( void );

              void   SetJobSpecifics( string Specs );
              void   SetTargetResources( string Resources ); 
              void   SetInput( string Input );
              void   SetOutput( string Output );
              void   SetState( string State );

       protected:

       string JobDirectory;
      };

// -----------------------------------------------------------------------------

int    UnlinkDirectoryRecursively( string Directory );
string ReadStringFromHashedFile( string FileName );
void   WriteStringToHashedFile( string String, string FileName );

// -----------------------------------------------------------------------------



#endif
