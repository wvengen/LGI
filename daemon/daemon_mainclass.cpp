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

 if( !IsValidConfigured() ) { CRITICAL_LOG( "Daemon::Daemon; Configuration in file " << ConfigFile << " invalid" ); return;}

 if( !ScanDirectoryForJobs( RunDirectory() ) ) { CRITICAL_LOG( "Daemon::Daemon; Error during scan of run directory " << RunDirectory() ); return; }
}

// -----------------------------------------------------------------------------

int Daemon::ReadyToSchedule( void )
{
 return( Jobs.empty() ? 0 : 1 );
}

// -----------------------------------------------------------------------------

int Daemon::ScanDirectoryForJobs( string Directory )
{
 struct dirent *Entry;
 DIR *Dir = opendir( Directory.c_str() );
 unsigned int FileMask = 0;

 if( Dir == NULL ) CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Could not scan directory " << Directory );

 VERBOSE_DEBUG_LOG( "Daemon::ScanDirectoryForJobs; Scanning directory " << Directory << " for jobs" );

 while( ( Entry = readdir( Dir ) ) != NULL )
 {
  if( !strcmp( Entry -> d_name, "." ) ) continue;
  if( !strcmp( Entry -> d_name, ".." ) ) continue;

  if( Entry -> d_type & DT_DIR )
  {
   if( !ScanDirectoryForJobs( Directory + "/" + string( Entry -> d_name ) ) ) 
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
    NORMAL_LOG( "Daemon::ScanDirectoryForJobs; Found job directory " << Directory );

    FileMask = ( LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER << 1 ) + 1;      // make sure we won't include it again...

    DaemonJob TheScannedJob( Directory );

    if( TheScannedJob.GetJobDirectory().empty() )
     CRITICAL_LOG( "Daemon::ScanDirectoryForJobs; Job with directory " << Directory << " seems corrupt, it will be ignored" )
    else
     if( !AddJobToDaemonLists( TheScannedJob ) ) { closedir( Dir ); CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Could not add job with directory " << Directory << " into daemon lists" ); }
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
 
 if( !Jobs[ Ref ].empty() )          // try and find job first...
  for( list<DaemonJob>::iterator i = Jobs[ Ref ].begin(); i != Jobs[ Ref ].end(); ++i )
   if( i -> GetJobDirectory() == Job.GetJobDirectory() )
    CRITICAL_LOG_RETURN( 0, "Daemon::AddJobToDaemonLists; Job with directory " << Job.GetJobDirectory() << " was already added into the daemon lists" );
 
 Jobs[ Ref ].insert( Jobs[ Ref ].end(), Job );  // if not found, add and account it...

 string Application = Job.GetApplication();
 vector<string> Owners = CommaSeparatedValues2Array( Job.GetOwners() );

 for( int i = 0; i < Owners.size(); ++i )
 {
  Accounting[ Owners[ i ] ]++;
  Accounting[ Owners[ i ] + ", " + Project ]++;
  Accounting[ Owners[ i ] + ", " + Project + ", " + Application ]++;
 }

 DEBUG_LOG_RETURN( 1, "Daemon::AddJobToDaemonLists; Job with directory " << Job.GetJobDirectory() << " added and accounted for" );
}

// -----------------------------------------------------------------------------

int Daemon::RemoveJobFromDaemonLists( DaemonJob Job )
{
 if( Jobs.empty() ) CRITICAL_LOG_RETURN( 0, "Daemon::RemoveJobFromDaemonLists; Daemon lists empty" ); 

 string Project = Job.GetProject();
 string Ref = Project + " @ " + Job.GetThisProjectServer(); 
 bool Found = false;
 list<DaemonJob>::iterator Location;

 // try and find the job first...
 if( !Jobs[ Ref ].empty() )
  for( Location = Jobs[ Ref ].begin(); Location != Jobs[ Ref ].end(); ++Location )
   if( Location -> GetJobDirectory() == Job.GetJobDirectory() )
   {
    Found = true;
    break;
   }

 if( !Found ) CRITICAL_LOG_RETURN( 0, "Daemon::RemoveJobFromDaemonLists; Job with directory " << Job.GetJobDirectory() << " not found in deamon lists" ); 

 // update the accounting tables... 
 string Application = Job.GetApplication();
 vector<string> Owners = CommaSeparatedValues2Array( Job.GetOwners() );

 for( int i = 0; i < Owners.size(); ++i )
 {
  Accounting[ Owners[ i ] ]--;
  Accounting[ Owners[ i ] + ", " + Project ]--;
  Accounting[ Owners[ i ] + ", " + Project + ", " + Application ]--;
 }

 // and now remove the job from the daemon lists...
 Jobs[ Ref ].erase( Location );

 DEBUG_LOG_RETURN( 1, "Daemon::RemoveJobFromDaemonLists; Job with directory " << Job.GetJobDirectory() << " removed and unaccounted for" );
}

// -----------------------------------------------------------------------------

int Daemon::CycleThroughJobs( void )
{
 if( Jobs.empty() ) CRITICAL_LOG_RETURN( 0, "Daemon::CycleThroughJobs; Daemon lists empty" ); 



 DEBUG_LOG( "Daemon::CycleThroughJobs; Starting with update from server cycle" );

 for( map<string,list<DaemonJob> >::iterator Server = Jobs.begin(); Server != Jobs.end(); ++Server )
  if( !Server -> second.empty() )
  {
   VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signing up to " << Server -> first );

   if( !( ( Server -> second.begin() ) -> SignUp() ) ) continue;      // signup to project and server...
   
   VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed up to " << Server -> first );

   for( list<DaemonJob>::iterator Job = Server -> second.begin(); Job != Server -> second.end(); ++Job )
   {
    VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Get time stamp for job with directory " << Job -> GetJobDirectory() );

    string TimeStamp = Job -> GetStateTimeStampFromServer();          // get state time stamp on server of job...

    if( TimeStamp.empty() ) continue;
    
    VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Obtained time stamp for job with directory " << Job -> GetJobDirectory() );

    if( TimeStamp != Job -> GetStateTimeStamp() )                     // server and job not synchronized...
    {
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Synchronizing job with directory " << Job -> GetJobDirectory() );
     if( !( Job -> LockJob() ) ) continue;                            // lock, update and unlock job...
     if( !( Job -> UpdateJobFromServer() ) ) continue;
     if( !( Job -> UnLockJob() ) ) continue;
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Synchronised job with directory " << Job -> GetJobDirectory() );
    }
    else
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << Job -> GetJobDirectory() << " was up to date" );
   }

   VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signing off from " << Server -> first );
   if( !( ( Server -> second.begin() ) -> SignOff() ) ) continue;                      // signoff from server...
   VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed off from " << Server -> first );
  }




 DEBUG_LOG( "Daemon::CycleThroughJobs; Starting with job scripts cycle" );

 for( map<string,list<DaemonJob> >::iterator Server = Jobs.begin(); Server != Jobs.end(); ++Server )
  if( !Server -> second.empty() )
  {
   for( list<DaemonJob>::iterator JobPointer = Server -> second.begin(); JobPointer != Server -> second.end(); )
   {
    DaemonJob TempJob;
 
    TempJob = (*(JobPointer++));                                  // get copy and point to next for iterator...

    if( TempJob.RunJobCheckRunningScript() == 0 )                 // we are currently running this job...
    {
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was found running" );
     if( TempJob.GetState() == "abort" )                          // do we need to abort the job...
     {
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Aborting job with directory " << TempJob.GetJobDirectory() );
      if( TempJob.RunJobAbortScript() ) continue;                 // run abort script...
      if( !TempJob.SignUp() ) continue;                           // update job to server...
      if( !TempJob.LockJob() ) continue;
      if( !TempJob.UpdateJob( "aborted", "", "", TempJob.GetOutput(), "" ) ) continue;
      if( !TempJob.UnLockJob() ) continue;
      if( !TempJob.SignOff() ) continue;
      RemoveJobFromDaemonLists( TempJob );                        // remove job from lists and cleanup directory..
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Aborted job with directory " << TempJob.GetJobDirectory() );
      TempJob.CleanUpJobDirectory();
     }
    }
    else
    {
     if( TempJob.RunJobCheckFinishedScript() == 0 )               // the job is finished...
     {
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was found to be finished" );
      if( TempJob.RunJobEpilogueScript() ) continue;              // run jobs epilogue script...
      if( !TempJob.SignUp() ) continue;                           // update job to server...
      if( !TempJob.LockJob() ) continue;
      if( !TempJob.UpdateJob( "finished", "", "", TempJob.GetOutput(), "" ) ) continue;
      if( !TempJob.UnLockJob() ) continue;
      if( !TempJob.SignOff() ) continue;
      RemoveJobFromDaemonLists( TempJob );                        // remove job from lists and cleanup directory..
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Finished job with directory " << TempJob.GetJobDirectory() );
      TempJob.CleanUpJobDirectory();
     }
     else                                                         // the job was not running and was not finished...
     {
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was found inactive" );

      if( TempJob.RunJobPrologueScript() == 0 )                   // then run job after prologue finished...
      {
       if( TempJob.RunJobRunScript() ) continue;
       VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " started running" );
      }
     }
    } 
   }
  }

 NORMAL_LOG_RETURN( 1, "Daemon::CycleThroughJobs; Scheduling cycle through jobs done" ); 
}

// -----------------------------------------------------------------------------

