/* []--------------------------------------------------------[]
    |                    daemon_jobclass.h                   |
   []--------------------------------------------------------[]
    |                                                        |
    | AUTHOR:     M.F.Somers                                 |
    | VERSION:    1.00, 14 August 2007.                      |
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
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

#include "logger.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "resource_server_api.h"

#define MIN( A, B ) ( A > B ? B : A )
#define MAX( A, B ) ( A > B ? A : B )

using namespace std;

// ------------ some handy defines -------------------------------------------

#define HASHFILE_EXTENTION                         ".hash"

// all files for job handeling in job directory...

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
#define LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE         "LGI_max_output_size"

// check values, a bit per file...

#define LGI_JOBDAEMON_PROJECT_FILE_BIT_VALUE                1
#define LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE_BIT_VALUE    2
#define LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE_BIT_VALUE  4
#define LGI_JOBDAEMON_APPLICATION_FILE_BIT_VALUE            8
#define LGI_JOBDAEMON_JOB_ID_FILE_BIT_VALUE                 16
#define LGI_JOBDAEMON_OWNERS_FILE_BIT_VALUE                 32
#define LGI_JOBDAEMON_READ_ACCESS_FILE_BIT_VALUE            64
#define LGI_JOBDAEMON_JOB_SPECIFICS_FILE_BIT_VALUE          128
#define LGI_JOBDAEMON_TARGET_RESOURCES_FILE_BIT_VALUE       256
#define LGI_JOBDAEMON_INPUT_FILE_BIT_VALUE                  512
#define LGI_JOBDAEMON_OUTPUT_FILE_BIT_VALUE                 1024
#define LGI_JOBDAEMON_STATE_FILE_BIT_VALUE                  2048
#define LGI_JOBDAEMON_STATE_TIME_STAMP_FILE_BIT_VALUE       4096
#define LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT_BIT_VALUE     8192
#define LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT_BIT_VALUE    16384
#define LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT_BIT_VALUE   32768
#define LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT_BIT_VALUE         65536
#define LGI_JOBDAEMON_JOB_RUN_SCRIPT_BIT_VALUE              131072
#define LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT_BIT_VALUE         262144
#define LGI_JOBDAEMON_JOB_ABORT_SCRIPT_BIT_VALUE            524288
#define LGI_JOBDAEMON_KEY_FILE_BIT_VALUE                    1048576
#define LGI_JOBDAEMON_CERTIFICATE_FILE_BIT_VALUE            2097152
#define LGI_JOBDAEMON_CA_CERTIFICATE_FILE_BIT_VALUE         4194304
#define LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE_BIT_VALUE        8388608

#define LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER               16777215

// -----------------------------------------------------------------------------

class DaemonJob
      {
       public:

              DaemonJob();
              DaemonJob( string TheJobDirectory );
              DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber );

              void CleanUpJobDirectory( void );             // remove job directory

              string GetJobDirectory( void );               // retrieve data from job directory
              string GetProject( void );
              string GetThisProjectServer( void );
              string GetProjectMasterServer( void );
              string GetApplication( void );
              string GetJobId( void );
              string GetOwners( void );
              string GetReadAccess( void );
              string GetJobSpecifics( void );               // volatile data
              string GetTargetResources( void );            // volatile data
              string GetInput( void );                      // volatile data
              string GetOutput( void );                     // volatile data
              string GetState( void );                      // volatile data
              string GetStateFromServer( void );            // volatile data
              string GetStateTimeStamp( void );             // volatile data
              string GetStateTimeStampFromServer( void );   // volatile data
              string GetKeyFile( void );
              string GetCertificateFile( void );
              string GetCACertificateFile( void );

              string GetSessionID( void );
              void   SetSessionID( string ID );

              int  SetJobSpecifics( string Specs );         // set data in directory and post to server
              int  SetTargetResources( string Resources ); 
              int  SetInput( string Input );
              int  SetOutput( string Output );
              int  SetState( string State );
              int  UpdateJob( string State, string Resources, string Input, string Output, string Specs );           

              int  UpdateJobFromServer( bool UpdateOutputToo = false ); // reset the above volatile marked data from server

              int  LockJob( void );                        // handy for scheduling per job
              int  UnLockJob( void );
              int  SignUp( void );
              int  SignOff( void );

              int  RunJobCheckLimitsScript( void );         // the check scripts
              int  RunJobCheckRunningScript( void );
              int  RunJobCheckFinishedScript( void );
              int  RunJobPrologueScript( void );            // the run scripts
              int  RunJobEpilogueScript( void );
              int  RunJobAbortScript( void );
              int  RunJobRunScript( void );
              
              int  GetMaxOutputSize( void );

       protected:

              int RunAScript( string TheScriptFile );

       string JobDirectory, SessionID;
      };

// -----------------------------------------------------------------------------

int  UnlinkDirectoryRecursively( string Directory );
int  ReadStringFromHashedFile( string FileName, string &Data );
void WriteStringToHashedFile( string String, string FileName );

// -----------------------------------------------------------------------------

#endif
