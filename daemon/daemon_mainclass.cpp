/* []--------------------------------------------------------[]
    |                 daemon_mainclass.cpp                   |
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

#include "daemon_mainclass.h"

// -----------------------------------------------------------------------------

Daemon::Daemon( string ConfigFile ) : DaemonConfig( ConfigFile )
{
 Jobs.clear(); Accounting.clear();
 if( !IsValidConfigured() ) CRITICAL_LOG( "Daemon::Daemon; Configuration in file " << ConfigFile << " invalid" );
}

// -----------------------------------------------------------------------------

int Daemon::ScanDirectoryForJobs( string Directory, bool CleanListsFirst )
{
 struct dirent *Entry;
 DIR *Dir = opendir( Directory.c_str() );
 unsigned int FileMask = 0;

 if( CleanListsFirst && Jobs.empty() ) CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Daemon job list empty" );

 if( Dir == NULL ) CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Could not scan directory " << Directory );

 if( CleanListsFirst )
 { 
  Jobs.clear();
  Accounting.clear();
 }
 
 while( ( Entry = readdir( Dir ) ) != NULL )
 {
  if( !strcmp( Entry -> d_name, "." ) ) continue;
  if( !strcmp( Entry -> d_name, ".." ) ) continue;

  if( Entry -> d_type & DT_DIR )
  {
   if( !ScanDirectoryForJobs( Directory + "/" + string( Entry -> d_name ), false ) ) 
   {
    closedir( Dir );
    return( 0 );
   }
  }
  else
  {
   // mask the files we find...

   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_PROJECT_FILE ) ) FileMask |= LGI_JOBDAEMON_PROJECT_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE ) ) FileMask |= LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE ) ) FileMask |= LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_APPLICATION_FILE ) ) FileMask |= LGI_JOBDAEMON_APPLICATION_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_ID_FILE ) ) FileMask |= LGI_JOBDAEMON_JOB_ID_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_OWNERS_FILE ) ) FileMask |= LGI_JOBDAEMON_OWNERS_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_READ_ACCESS_FILE ) ) FileMask |= LGI_JOBDAEMON_READ_ACCESS_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_SPECIFICS_FILE ) ) FileMask |= LGI_JOBDAEMON_JOB_SPECIFICS_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_TARGET_RESOURCES_FILE ) ) FileMask |= LGI_JOBDAEMON_TARGET_RESOURCES_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_INPUT_FILE ) ) FileMask |= LGI_JOBDAEMON_INPUT_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_OUTPUT_FILE ) ) FileMask |= LGI_JOBDAEMON_OUTPUT_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_STATE_FILE ) ) FileMask |= LGI_JOBDAEMON_STATE_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ) ) FileMask |= LGI_JOBDAEMON_STATE_TIME_STAMP_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_RUN_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_RUN_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_ABORT_SCRIPT ) ) FileMask |= LGI_JOBDAEMON_JOB_ABORT_SCRIPT_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_KEY_FILE ) ) FileMask |= LGI_JOBDAEMON_KEY_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_CERTIFICATE_FILE ) ) FileMask |= LGI_JOBDAEMON_CERTIFICATE_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_CA_CERTIFICATE_FILE ) ) FileMask |= LGI_JOBDAEMON_CA_CERTIFICATE_FILE_BIT_VALUE;

   // if we have all the needed files...
   if( FileMask == LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER )
   {
    DEBUG_LOG( "Daemon::ScanDirectoryForJobs; Found job directory " << Directory );

    FileMask = ( LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER << 1 ) + 1;      // make sure we won't include it again...

    if( !AddJobToDaemonLists( DaemonJob( Directory ) ) ) CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Could not add job with directory " << Directory << " into daemin lists" );
   }

  }
 }

 closedir( Dir );
 return( 1 ); 
}

// -----------------------------------------------------------------------------

int Daemon::AddJobToDaemonLists( DaemonJob Job )
{
 string Project = Job.GetProject();

 string Ref = Project + " @ " + Job.GetThisProjectServer(); 
 
 if( !Jobs[ Ref ].empty() )          // try and find job first
  for( list<DaemonJob>::iterator i = Jobs[ Ref ].begin(); i != Jobs[ Ref ].end(); ++i )
   if( i -> GetJobDirectory() == Job.GetJobDirectory() )
    CRITICAL_LOG_RETURN( 0, "Daemon::AddJobToDaemonLists; Job was already added into the daemon lists" );
 
 Jobs[ Ref ].insert( Jobs[ Ref ].end(), Job );

 string Application = Job.GetApplication();
 vector<string> Owners = CommaSeparatedValues2Array( Job.GetOwners() );

 for( int i = 0; i < Owners.size(); ++i )
 {
  Accounting[ Owners[ i ] ]++;
  Accounting[ Owners[ i ] + ", " + Project ]++;
  Accounting[ Owners[ i ] + ", " + Project + ", " + Application ]++;
 }

 DEBUG_LOG_RETURN( 1, "Daemon::AddJobToDaemonLists; Job added and accounted for" );
}

// -----------------------------------------------------------------------------

int Daemon::RemoveJobFromDaemonLists( DaemonJob Job )
{
}

// -----------------------------------------------------------------------------
