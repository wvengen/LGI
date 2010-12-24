/* []--------------------------------------------------------[]
    |                 daemon_mainclass.cpp                   |
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

#include "daemon_mainclass.h"

// -----------------------------------------------------------------------------

Daemon::Daemon( string ConfigFile, int SlowCycleTime, int FastCycleTime ) : DaemonConfig( ConfigFile )
{
 ReadyForScheduling = 0; Jobs.clear(); Accounting.clear();

 if( !IsValidConfigured() ) { CRITICAL_LOG( "Daemon::Daemon; Configuration in file " << ConfigFile << " invalid" ); return; }

 if( !ScanDirectoryForJobs( RunDirectory() ) ) { CRITICAL_LOG( "Daemon::Daemon; Error during scan of run directory " << RunDirectory() ); return; }

 if( FastCycleTime >= SlowCycleTime ) { CRITICAL_LOG( "Daemon::Daemon; FastCycleTime >= SlowCycleTime" ); return; }

 if( FastCycleTime < 1 ) { CRITICAL_LOG( "Daemon::Daemon; FastCycleTime < 1" ); return; }

 TheFastCycleTime = FastCycleTime;
 TheSlowCycleTime = SlowCycleTime;
 ReadyForScheduling = 1;
 MycURLHandle = curl_easy_init();
 if( MycURLHandle == NULL ) CRITICAL_LOG( "Daemon::Daemon; Could not create cURL handle for all connections" );
}

// -----------------------------------------------------------------------------

Daemon::~Daemon( void )
{
 if( MycURLHandle != NULL ) curl_easy_cleanup( MycURLHandle );
}

// -----------------------------------------------------------------------------

int Daemon::ScanDirectoryForJobs( string Directory )
{
 struct stat FileStat;
 struct dirent *Entry;
 DIR *Dir = opendir( Directory.c_str() );
 unsigned int FileMask = 0;

 if( Dir == NULL ) CRITICAL_LOG_RETURN( 0, "Daemon::ScanDirectoryForJobs; Could not scan directory " << Directory );

 VERBOSE_DEBUG_LOG( "Daemon::ScanDirectoryForJobs; Scanning directory " << Directory << " for jobs" );

 while( ( Entry = readdir( Dir ) ) != NULL )
 {
  if( !strcmp( Entry -> d_name, "." ) ) continue;
  if( !strcmp( Entry -> d_name, ".." ) ) continue;

  string FilePath = Directory + "/" + string( Entry -> d_name );
  if( stat( FilePath.c_str(), &FileStat ) ) continue;

  if( S_ISDIR( FileStat.st_mode ) )              // check if it is a directory...
  {
   if( !ScanDirectoryForJobs( FilePath ) ) 
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
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_WRITE_ACCESS_FILE ) ) FileMask |= LGI_JOBDAEMON_WRITE_ACCESS_FILE_BIT_VALUE;
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
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE ) ) FileMask |= LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE ) ) FileMask |= LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_JOB_RUN_SCRIPT_PID_FILE ) ) FileMask |= LGI_JOBDAEMON_JOB_RUN_SCRIPT_PID_FILE_BIT_VALUE;
   if( !strcmp( Entry -> d_name, LGI_JOBDAEMON_DAEMON_REFERENCE_FILE ) ) FileMask |= LGI_JOBDAEMON_DAEMON_REFERENCE_FILE_BIT_VALUE;

   // if we have all the needed files...
   if( FileMask == LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER )
   {
    NORMAL_LOG( "Daemon::ScanDirectoryForJobs; Found job directory " << Directory );

    FileMask = ( LGI_JOBDAEMON_ALL_BIT_VALUES_TOGETHER << 1 ) + 1;      // make sure we won't include it again...

    DaemonJob TheScannedJob( Directory, MycURLHandle );                 // also make sure we use our curl handle...

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
 vector<string> AccountedFor;

 for( int i = 0, j = 0; i < Owners.size(); ++i )
 {
  for( j = 0; j < AccountedFor.size(); j++ )         // check if this owner has already been accounted for...
   if( AccountedFor[ j ] == Owners[ i ] ) break;
  if( j < AccountedFor.size() ) continue;

  AccountedFor.push_back( Owners[ i ] );    

  Accounting[ Owners[ i ] ]++;
  Accounting[ Owners[ i ] + ", " + Project ]++;
  Accounting[ Owners[ i ] + ", " + Project + ", " + Application ]++;

  ListOfActiveOwners.insert( Owners[ i ] );   // add this owner into set...
 }

 Accounting[ "; TOTALS;" ]++;
 Accounting[ "; TOTALS; " + Project ]++;
 Accounting[ "; TOTALS; " + Project + "; " + Application ]++;

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
 vector<string> AccountedFor;

 for( int i = 0, j = 0; i < Owners.size(); ++i )
 {
  for( j = 0; j < AccountedFor.size(); j++ )         // check if this owner has already been accounted for...
   if( AccountedFor[ j ] == Owners[ i ] ) break;
  if( j < AccountedFor.size() ) continue;

  AccountedFor.push_back( Owners[ i ] );

  Accounting[ Owners[ i ] ]--;
  Accounting[ Owners[ i ] + ", " + Project ]--;
  Accounting[ Owners[ i ] + ", " + Project + ", " + Application ]--;

  if( Accounting[ Owners[ i ] ] <= 0 ) ListOfActiveOwners.erase( Owners[ i ] );
 }

 Accounting[ "; TOTALS;" ]--;
 Accounting[ "; TOTALS; " + Project ]--;
 Accounting[ "; TOTALS; " + Project + "; " + Application ]--;

 // and now remove the job from the daemon lists...
 Jobs[ Ref ].erase( Location );

 DEBUG_LOG_RETURN( 1, "Daemon::RemoveJobFromDaemonLists; Job with directory " << Job.GetJobDirectory() << " removed and unaccounted for" );
}

// -----------------------------------------------------------------------------

int Daemon::CycleThroughJobs( void )
{
 if( Jobs.empty() ) CRITICAL_LOG_RETURN( 0, "Daemon::CycleThroughJobs; Daemon lists empty" ); 
 
 JobsFinished = 0;

 DEBUG_LOG( "Daemon::CycleThroughJobs; Starting with update from server cycle" );

 for( map<string,list<DaemonJob> >::iterator Server = Jobs.begin(); Server != Jobs.end() && ReadyForScheduling; ++Server )
  if( !Server -> second.empty() )
  {
   int SignedUp = 0;
   string SessionID;

   for( list<DaemonJob>::iterator JobPointer = Server -> second.begin(); JobPointer != Server -> second.end() && ReadyForScheduling; )
   {
    DaemonJob TempJob;
    TempJob = (*(JobPointer++));                 // get copy and point to next for iterator...

    VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Get time stamp for job with directory " << TempJob.GetJobDirectory() );

    string TimeStamp = TempJob.GetStateTimeStampFromServer();          // get state time stamp on server of job...

    if( TimeStamp.empty() )     // if this failed, check if job is a stale job that has been accepted by another daemon...
    {
     if( ( TempJob.GetErrorNumber() == 24 ) || ( TempJob.GetErrorNumber() == 15 ) )    // check if we weren't allowed to get job state or
     {                                                                                 // the job doesn't exist in the DB anymore...
      CRITICAL_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " seems stale, invalidating daemon reference hash" );
      TempJob.InvalidateDaemonReferenceHash();
     }
     continue;
    }
    
    VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Obtained time stamp for job with directory " << TempJob.GetJobDirectory() );

    if( TimeStamp != TempJob.GetStateTimeStamp() )                     // server and job not synchronized...
    {
     if( !SignedUp )
     {
      DEBUG_LOG( "Daemon::CycleThroughJobs; Signing up to " << Server -> first );    // signup to project and server...
      ( Server -> second.begin() ) -> SetSessionID( "" );
      if( !( ( Server -> second.begin() ) -> SignUp() ) ) continue;                     
      SignedUp = 1;
      SessionID = ( Server -> second.begin() ) -> GetSessionID();
      ( Server -> second.begin() ) -> SetSessionID( "" );
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed up to " << Server -> first << " with session id " << SessionID );
     }

     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Synchronizing job with directory " << TempJob.GetJobDirectory() << " using session id " << SessionID );
     TempJob.SetSessionID( SessionID );
     if( !( TempJob.LockJob() ) ) continue;                            // lock, update and unlock job...
     if( !( TempJob.UpdateJobFromServer() ) ) continue;
     if( !( TempJob.UnLockJob() ) ) continue;
     TempJob.SetSessionID( "" );
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Synchronised job with directory " << TempJob.GetJobDirectory() );
    }
    else
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was up to date" );
   }

   if( SignedUp )
   {
    DEBUG_LOG( "Daemon::CycleThroughJobs; Signing off from " << Server -> first );
    ( Server -> second.begin() ) -> SetSessionID( SessionID );
    if( !( ( Server -> second.begin() ) -> SignOff() ) ) continue;                      // signoff from server...
    ( Server -> second.begin() ) -> SetSessionID( "" );
    VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed off from " << Server -> first );
   }
  }

 DEBUG_LOG( "Daemon::CycleThroughJobs; Starting with job scripts cycle" );
 
 Resource_Server_API ServerAPI( Resource_Key_File(), Resource_Certificate_File(), CA_Certificate_File(), MycURLHandle );
 string ServerURL, Project, Response;

 for( map<string,list<DaemonJob> >::iterator Server = Jobs.begin(); Server != Jobs.end() && ReadyForScheduling; ++Server )
  if( !Server -> second.empty() )
  {
   int SignedUp = 0;
   string SessionID;

   for( list<DaemonJob>::iterator JobPointer = Server -> second.begin(); JobPointer != Server -> second.end() && ReadyForScheduling; )
   {
    DaemonJob TempJob;
 
    TempJob = (*(JobPointer++));                                  // get copy and point to next for iterator...

    // now check and see if the daemon reference in the job specs match the one in the job slot...
    if( TempJob.GetDaemonReferenceHash() != NormalizeString( Parse_XML( TempJob.GetJobSpecifics(), "daemon_reference" ) ) )
    {
     CRITICAL_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " seems stale, invalidating daemon reference hash and trying to abort" );
     TempJob.InvalidateDaemonReferenceHash();
     if( TempJob.RunJobCheckFinishedScript() == 0 )                               // if finished run epilogue...
     {
      if( TempJob.RunJobEpilogueScript() ) continue;
     }
     else
      if( TempJob.RunJobCheckRunningScript() == 0 ) 
       if( TempJob.RunJobAbortScript() ) continue;                                // or try to abort...
     NORMAL_LOG( "Daemon::CycleThroughJobs; Removing and cleaning up stale job with directory " << TempJob.GetJobDirectory() );
     RemoveJobFromDaemonLists( TempJob );                   // remove job from lists and cleanup directory..
     TempJob.KillJobRunScriptProcess();
     TempJob.CleanUpJobDirectory();
     continue;
    }

    if( TempJob.RunJobCheckRunningScript() == 0 )                 // we are currently running this job...
    {
     VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was found running" );

     if( TempJob.GetState() == "aborting" )                       // do we need to abort the job...
     {
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Aborting job with directory " << TempJob.GetJobDirectory() );
      if( TempJob.RunJobAbortScript() ) continue;                 // run abort script... and update job to server...
      if( !SignedUp )
      {
       DEBUG_LOG( "Daemon::CycleThroughJobs; Signing up to server " << ServerURL << " for project " << Project );
       TempJob.SetSessionID( "" );
       if( !TempJob.SignUp() ) continue;                          // sign up if needed...
       SignedUp = 1;
       SessionID = TempJob.GetSessionID();
       ServerURL = TempJob.GetThisProjectServer();
       Project = TempJob.GetProject();
       TempJob.SetSessionID( "" );
       VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed up to server " << ServerURL << " for project " << Project << " with session id " << SessionID );
      }
      TempJob.SetSessionID( SessionID );
      if( !TempJob.LockJob() ) continue;
      if( !TempJob.UpdateJob( "aborted", "", "", TempJob.GetOutput(), "" ) ) continue;
      if( !TempJob.UnLockJob() ) continue;
      TempJob.SetSessionID( "" );
      RemoveJobFromDaemonLists( TempJob );                        // remove job from lists and cleanup directory..
      NORMAL_LOG( "Daemon::CycleThroughJobs; Aborted job with directory " << TempJob.GetJobDirectory() );
      TempJob.KillJobRunScriptProcess();
      TempJob.CleanUpJobDirectory();
      JobsFinished = 1;
     }
     else                     // if not aborting this job...
     {
      if( TempJob.GetState() != "running" )        // if our local script said the job runs and the state in DB said not so, drop the job...
      {                                          
       CRITICAL_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " seems stale, invalidating daemon reference hash" );
       TempJob.InvalidateDaemonReferenceHash();
      }
     }

    }
    else
    {
     if( TempJob.RunJobCheckFinishedScript() == 0 )               // the job is finished...
     {
      VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Job with directory " << TempJob.GetJobDirectory() << " was found to be finished" );
      if( TempJob.RunJobEpilogueScript() ) continue;              // run jobs epilogue script... update job to server...
      if( !SignedUp ) 
      { 
       DEBUG_LOG( "Daemon::CycleThroughJobs; Signing up to server " << ServerURL << " for project " << Project );
       TempJob.SetSessionID( "" );
       if( !TempJob.SignUp() ) continue;                         // sign up if needed...
       SignedUp = 1;
       SessionID = TempJob.GetSessionID();
       ServerURL = TempJob.GetThisProjectServer();
       Project = TempJob.GetProject();
       TempJob.SetSessionID( "" );
       VERBOSE_DEBUG_LOG( "Daemon::CycleThroughJobs; Signed up to server " << ServerURL << " for project " << Project << " with session id " << SessionID );
      }
      TempJob.SetSessionID( SessionID );
      if( !TempJob.LockJob() ) continue;
      if( !TempJob.UpdateJob( "finished", "", "", TempJob.GetOutput(), "" ) ) continue;
      if( !TempJob.UnLockJob() ) continue;
      TempJob.SetSessionID( "" );
      RemoveJobFromDaemonLists( TempJob );                        // remove job from lists and cleanup directory..
      NORMAL_LOG( "Daemon::CycleThroughJobs; Finished job with directory " << TempJob.GetJobDirectory() );
      TempJob.KillJobRunScriptProcess();
      TempJob.CleanUpJobDirectory();
      JobsFinished = 1;
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

   if( SignedUp )      // sign off from server if needed...
   {
    DEBUG_LOG( "Daemon::CycleThroughJobs; Signing of from server " << ServerURL << " for project " << Project << " with session id " << SessionID );

    do
    {
     if( ServerAPI.Resource_SignOff_Resource( Response, ServerURL, Project, SessionID ) )
     {
      CRITICAL_LOG( "Daemon::CycleThroughJobs; Could not post to server " << ServerURL );
      break;
     }
     Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
     SignedUp = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
    } while( SignedUp == LGI_SERVER_BACKOFF_ERROR_NR );

    if( SignedUp ) CRITICAL_LOG( "Daemon::CycleThroughJobs; Error from server Response=" << Response );
   }

  }
 
 // now see which lists are empty, so we can remove them...
 for( map<string,list<DaemonJob> >::iterator Server = Jobs.begin(); Server != Jobs.end() && ReadyForScheduling; )
  if( Server -> second.empty() )
   Jobs.erase( Server++ );
  else
   ++Server;

 NORMAL_LOG_RETURN( 1, "Daemon::CycleThroughJobs; Scheduling cycle through jobs done" ); 
}

// -----------------------------------------------------------------------------

int Daemon::RequestWorkCycle( void )
{
 DEBUG_LOG( "Daemon::RequestWorkCycle; Starting with request work cycle" );

 JobsObtained = 0;

 if( Job_Limit() <= Accounting[ "; TOTALS;" ] ) NORMAL_LOG_RETURN( JobsObtained, "Daemon::RequestWorkCycle; Total job limit reached, not requesting any work" );

 Resource_Server_API ServerAPI( Resource_Key_File(), Resource_Certificate_File(), CA_Certificate_File(), MycURLHandle );

 for( int nP = 1; nP <= Number_Of_Projects() && ReadyForScheduling; ++nP )     // cycle through all projects...
 {
  DaemonConfigProject TheProject;

  TheProject = Project( nP );

  // check if limits are reached... 
  if( Job_Limit() <= Accounting[ "; TOTALS;" ] ) continue;
  if( TheProject.Job_Limit() <= Accounting[ "; TOTALS; " + TheProject.Project_Name() ] ) continue;

  // first signup to registered master server of this project to get all slaves... if this fails somehow, try the next...
  list<string> ServerList;
  string Response, Attributes, SessionID, ServerMaxFieldSize, Capabilities;
  int    StartPos;

  DEBUG_LOG( "Daemon::RequestWorkCycle; Signing up to project " << TheProject.Project_Name() << " at server " << TheProject.Project_Master_Server() );

  // setup capabilities of this resource for this project's application...
  for( int nA = 1; nA <= TheProject.Number_Of_Applications() && ReadyForScheduling; ++nA )
  {
   DaemonConfigProjectApplication TheApplication = TheProject.Application( nA );
   string AppName = TheApplication.Application_Name();
   Capabilities += "<" + AppName + "> " + TheApplication.Capabilities() + " </" + AppName + "> ";
  }

  VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Capabilities reported to server " << Capabilities );

  if( ServerAPI.Resource_SignUp_Resource( Response, TheProject.Project_Master_Server(), TheProject.Project_Name(), Capabilities ) != CURLE_OK ) continue;

  Response = Parse_XML( Response, "LGI" );
  ServerMaxFieldSize = NormalizeString( Parse_XML( Response, "server_max_field_size" ) );
  Response = Parse_XML( Response, "response" );

  if( Response.empty() ) continue;

  if( !Parse_XML( Response, "error" ).empty() ) { CRITICAL_LOG( "Daemon::RequestWorkCycle; Unable to sign up to project " << TheProject.Project_Name() << " at server " << TheProject.Project_Master_Server() << " : " << Parse_XML( Parse_XML( Response, "error" ), "message" ) ); continue; }

  SessionID = NormalizeString( Parse_XML( Response, "session_id" ) );
  
  VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Signed up to project " << TheProject.Project_Name() << " at server " << TheProject.Project_Master_Server() << " with session id " << SessionID );

  int NumberOfServers = atoi( NormalizeString( Parse_XML( Response, "number_of_slave_servers" ) ).c_str() );
  
  VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Received list of " << NumberOfServers << " servers from " << TheProject.Project_Master_Server() );

  string Server;

  for( int nS = StartPos = 0; nS < NumberOfServers; nS++ )
  {
   Server = NormalizeString( Parse_XML( Response, "project_server", Attributes, StartPos ) );
   if( Server.empty() ) continue; 
   if( Server != TheProject.Project_Master_Server() )
   {
    ServerList.insert( ServerList.begin(), Server );
    VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Added " << Server << " to server request list" )
   }
  }

  // now add also master server reported in the response, and the one we just asked...
  Server = NormalizeString( Parse_XML( Response, "project_master_server" ) ); 

  if( Server != TheProject.Project_Master_Server() ) 
  {
   ServerList.insert( ServerList.end(), Server ); 
   VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Added " << Server << " to server request list" );
  }

  Server = TheProject.Project_Master_Server();
  ServerList.insert( ServerList.begin(), Server ); 
  VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Added " << Server << " to server request list" );

  Server = NormalizeString( Parse_XML( Response, "project_master_server" ) ); 

  list<string>::iterator ServerPointer = ServerList.begin();

  // the list is complete and we are signed up to the first (master) server...

  do
  {
   if( !Response.empty() )
   {

    // now check all applications for this project on this server...
    for( int nA = 1; nA <= TheProject.Number_Of_Applications() && ReadyForScheduling; ++nA ) 
    {
     DaemonConfigProjectApplication TheApplication;

     TheApplication = TheProject.Application( nA );
  
     // check if limits are reached... 
     if( Job_Limit() <= Accounting[ "; TOTALS;" ] ) continue;
     if( TheProject.Job_Limit() <= Accounting[ "; TOTALS; " + TheProject.Project_Name() ] ) continue;
     if( TheApplication.Job_Limit() <= Accounting[ "; TOTALS; " + TheProject.Project_Name() + "; " + TheApplication.Application_Name() ] ) continue;

     string ExtraJobDetailsTags = "<project> " + TheProject.Project_Name()  + " </project> <this_project_server> " +
                                  (*ServerPointer) + " </this_project_server> <project_master_server> " + Server + " </project_master_server> " +
                                  "<application> " + TheApplication.Application_Name() + " </application> " +
                                  "<state> queued </state> <server_max_field_size> " + ServerMaxFieldSize +
                                  " </server_max_field_size>";

     VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Checking system limits for application " << TheApplication.Application_Name() );

     // check if there is a system wide limit reached for this application...
     if( system( TheApplication.Check_System_Limits_Script().c_str() ) == 0 ) 
     {
      DEBUG_LOG( "Daemon::RequestWorkCycle; Requesting work for application " << TheApplication.Application_Name() << " of project " << TheProject.Project_Name() << " at server " << (*ServerPointer) << " with session id " << SessionID );

      int NrOfJobs = 0;
      int Limit = MIN( MAX( ( TheApplication.Job_Limit() << 2 ), 32 ), 256 );
      int Offset = 0;
      int FoundJob = 0;
      char OffsetStr[ 64 ];
      char LimitStr[ 64 ];
      string OwnersStr = "";
      vector<string> LimitsList;
      set<string> OwnersAllowedList, OwnersDeniedList;

      do
      {
       sprintf( OffsetStr, "%d", Offset );
       sprintf( LimitStr, "%d", Limit );

       // start by collecting possible owners or denied owners for jobs...
       LimitsList.clear(); OwnersDeniedList.clear(); OwnersAllowedList.clear();     // clear all lists...

       Parse_XML_ListAllTags( Owner_Allow(), LimitsList );                       // get all listed owners from limits...
       Parse_XML_ListAllTags( TheProject.Owner_Allow(), LimitsList );
       Parse_XML_ListAllTags( TheApplication.Owner_Allow(), LimitsList );

       for( int i = 0; i < LimitsList.size(); ++i )      // check if any of these owners has hit a limit or is denied...
        if( ( IsOwnerDenied( LimitsList[ i ], TheProject, TheApplication ) ) || ( IsOwnerRunningToMuch( LimitsList[ i ], TheProject, TheApplication ) & 7 ) )
         OwnersDeniedList.insert( LimitsList[ i ] );  
        else
         OwnersAllowedList.insert( LimitsList[ i ] );  

       if( OwnersAllowedList.find( "any" ) != OwnersAllowedList.end() )   // just clear list if 'any' was also found and allowed...
        OwnersAllowedList.clear();

       for( set<string>::iterator i = ListOfActiveOwners.begin(); i != ListOfActiveOwners.end(); ++i )   // check all known owners so far...
        if( ( IsOwnerDenied( (*i), TheProject, TheApplication ) ) || ( IsOwnerRunningToMuch( (*i), TheProject, TheApplication ) & 7 ) )
         OwnersDeniedList.insert( (*i) );

       LimitsList.clear();                                     // and add all denied owners to the list...
       LimitsList = CommaSeparatedValues2Array( Owner_Deny() + ", " + TheProject.Owner_Deny() + ", " + TheApplication.Owner_Deny() );
       for( int i = 0; i < LimitsList.size(); ++i ) OwnersDeniedList.insert( LimitsList[ i ] );

       for( set<string>::iterator i = OwnersAllowedList.begin(); i != OwnersAllowedList.end(); ++i )   // finally build string to post...
        OwnersStr += ", " + (*i);
       for( set<string>::iterator i = OwnersDeniedList.begin(); i != OwnersDeniedList.end(); ++i )
        OwnersStr += ", !" + (*i);
       if( !OwnersStr.empty() ) OwnersStr[ 0 ] = ' ';

       if( !OwnersStr.empty() ) DEBUG_LOG( "Daemon::RequestWorkCycle; requesting work for owner list" << OwnersStr );   // log for testing and debugging...

       VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Performing work request with Offset=" << OffsetStr << " at server " << (*ServerPointer) );
       if( ServerAPI.Resource_Request_Work( Response, (*ServerPointer), TheProject.Project_Name(), SessionID, TheApplication.Application_Name(), OffsetStr, LimitStr, OwnersStr ) != CURLE_OK ) break;

       string JobResponse = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

       if( JobResponse.empty() ) break;

       if( !Parse_XML( JobResponse, "error" ).empty() )
       { 
        CRITICAL_LOG( "Daemon::RequestWorkCycle; Unable to request work at server " << (*ServerPointer) << " for project " << TheProject.Project_Name() << " for application " << TheApplication.Application_Name()<< ": " << Parse_XML( Parse_XML( JobResponse, "error" ), "message" ) ); 
        break;
       };

       NrOfJobs = atoi( NormalizeString( Parse_XML( JobResponse, "number_of_jobs" ) ).c_str() );
       
       VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Received " << NrOfJobs << " jobs from server " << (*ServerPointer) );

       for( int JobIndex = StartPos = 0; JobIndex < NrOfJobs; ++JobIndex )        // loop over jobs in response...
       {
        string JobData = NormalizeString( Parse_XML( JobResponse, "job", Attributes, StartPos ) );
        if( JobData.empty() ) continue;

        string Job_Id = NormalizeString( Parse_XML( JobData, "job_id" ) );
        if( Job_Id.empty() ) continue;

        // now check if any of the owners is denied serving...
        int OwnerIndex, OwnerFlag;
        vector<string> Owners = CommaSeparatedValues2Array( Parse_XML( JobData, "owners" ) );

        for( OwnerFlag = OwnerIndex = 0; OwnerIndex < Owners.size(); ++OwnerIndex )
        {
         // check if limits are reached... 
         if( Job_Limit() <= Accounting[ "; TOTALS;" ] ) break;
         if( TheProject.Job_Limit() <= Accounting[ "; TOTALS; " + TheProject.Project_Name() ] ) break;
         if( TheApplication.Job_Limit() <= Accounting[ "; TOTALS; " + TheProject.Project_Name() + "; " + TheApplication.Application_Name() ] ) break;

         // check if owner is denied... 
         if( IsOwnerDenied( Owners[ OwnerIndex ], TheProject, TheApplication ) ) break;

         // check if owner specific limits are reached... 
         OwnerFlag |= IsOwnerRunningToMuch( Owners[ OwnerIndex ], TheProject, TheApplication );
         if( OwnerFlag & 3 ) break; 
        }

        if( ( OwnerIndex == Owners.size() ) && ( OwnerFlag & 8 ) )    // no denials or limits reached for any of the owners...
        {
         VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; No owners were denied service for job " << Job_Id );

         // create temporary job directory with the jobs response data...
         DaemonJob TempJob( ExtraJobDetailsTags + "<job> " + JobData + " </job>", (*this), nP, nA, MycURLHandle );

         // see if job has limits from job limits script somehow... if so, delete temp job directory...
         if( TempJob.RunJobCheckLimitsScript() == 0 )
         {
          VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; No job limits were encountered for job " << Job_Id );

          // set job into running state on server and through this we now also get input... also set daemon reference into job specs...
          TempJob.SetSessionID( SessionID );
          if( TempJob.UpdateJob( "running", NormalizeString( Parse_XML( JobResponse, "resource" ) ), "", "", 
              "<daemon_reference> " + TempJob.GetDaemonReferenceHash() + " </daemon_reference> " + TempJob.GetJobSpecifics() ) )
          {

           // there are no limits and job was successfully accepted...
           AddJobToDaemonLists( TempJob );
           NORMAL_LOG( "Daemon::RequestWorkCycle; Job " << TempJob.GetJobId() << " accepted for " << TempJob.GetOwners() << " from " << TempJob.GetThisProjectServer() << " into " << TempJob.GetJobDirectory() );
           FoundJob = JobsObtained = 1;

          }
          else
          {
           CRITICAL_LOG( "Daemon::RequestWorkCycle; Job with directory " << TempJob.GetJobDirectory() << " could not be accepted" );
           UnlinkDirectoryRecursively( TempJob.GetJobDirectory() );
          }
          TempJob.SetSessionID( "" );

         }
         else
         {
          VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; A job limit was encountered for job " << Job_Id );
          UnlinkDirectoryRecursively( TempJob.GetJobDirectory() );
         }
          
        }
        else
         VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Denied service for job " << Job_Id << ", flag=" << OwnerFlag << ", index=" << OwnerIndex );

        if( ServerAPI.Resource_UnLock_Job( Response, (*ServerPointer), TheProject.Project_Name(), SessionID, Job_Id ) != CURLE_OK ) JobIndex = NrOfJobs;
       }

       Offset += Limit;
      } while( ( NrOfJobs > ( Limit >> 1 ) ) && ( !FoundJob ) && ReadyForScheduling && ( Offset < ( Limit << 4 ) ) );

     }
     else
      DEBUG_LOG( "Daemon::RequestWorkCycle; System limit reached for application " << TheApplication.Application_Name() << " of project " << TheProject.Project_Name() );
    }

    // sign off from this server...
    DEBUG_LOG( "Daemon::RequestWorkCycle; Signing off from server " << (*ServerPointer) << " with session id " << SessionID );

    if( ServerAPI.Resource_SignOff_Resource( Response, (*ServerPointer), TheProject.Project_Name(), SessionID ) != CURLE_OK )
     Response.clear();
    else
     VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Signed off from server " << (*ServerPointer) );
   }

   if( (++ServerPointer) != ServerList.end() && ReadyForScheduling )         // go to next server in list and sign up there...
   {
    DEBUG_LOG( "Daemon::RequestWorkCycle; Signing up to project " << TheProject.Project_Name() << " at server " << (*ServerPointer) );

    if( ServerAPI.Resource_SignUp_Resource( Response, (*ServerPointer), TheProject.Project_Name(), Capabilities ) != CURLE_OK )
    {
     Response.clear();
     SessionID.clear();
     Server.clear();
    }
    else
    {
     Response = Parse_XML( Response, "LGI" );
     ServerMaxFieldSize = NormalizeString( Parse_XML( Response, "server_max_field_size" ) );
     Response = Parse_XML( Response, "response" );
     Server = NormalizeString( Parse_XML( Response, "project_master_server" ) ); 

     if( !Parse_XML( Response, "error" ).empty() )
     {
      CRITICAL_LOG( "Daemon::RequestWorkCycle; Unable to sign up: " << Parse_XML( Parse_XML( Response, "error" ), "message" ) );
      Response.clear();
      ServerMaxFieldSize.clear();
      SessionID.clear();
      Server.clear();
     } 
     else
     {
      SessionID = NormalizeString( Parse_XML( Response, "session_id" ) );
      VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; Signed up to project " << TheProject.Project_Name() << " at server " << (*ServerPointer) << " with session id " << SessionID );
     }
    }
   }
   else
   {
    Response.clear();
    ServerMaxFieldSize.clear();
    SessionID.clear();
    Server.clear();
   }

  }
  while( ServerPointer != ServerList.end() && ReadyForScheduling );

 } 

 NORMAL_LOG_RETURN( JobsObtained, "Daemon::RequestWorkCycle; Request for work cycle done" ); 
}

// -----------------------------------------------------------------------------

int Daemon::IsOwnerDenied( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application )
{
 vector<string> ConfigOwners = CommaSeparatedValues2Array( Owner_Deny() );

 for( int i = 0; i < ConfigOwners.size(); ++i )
 {
  if( Owner == ConfigOwners[ i ] ) return( 1 );
  if( ConfigOwners[ i ] == "any" ) return( 1 );
 }

 vector<string> ProjectOwners = CommaSeparatedValues2Array( Project.Owner_Deny() );

 for( int i = 0; i < ProjectOwners.size(); ++i )
 {
  if( Owner == ProjectOwners[ i ] ) return( 1 );
  if( ProjectOwners[ i ] == "any" ) return( 1 );
 }

 vector<string> ApplicationOwners = CommaSeparatedValues2Array( Application.Owner_Deny() );

 for( int i = 0; i < ApplicationOwners.size(); ++i )
 {
  if( Owner == ApplicationOwners[ i ] ) return( 1 );
  if( ApplicationOwners[ i ] == "any" ) return( 1 );
 }

 return( 0 );
}

// -----------------------------------------------------------------------------

int Daemon::IsOwnerRunningToMuch( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application )
{
 // check total limits first...
 if( Job_Limit() <= Accounting[ "; TOTALS;" ] ) return( 1 );
 if( Project.Job_Limit() <= Accounting[ "; TOTALS; " + Project.Project_Name() ] ) return( 1 );
 if( Application.Job_Limit() <= Accounting[ "; TOTALS; " + Project.Project_Name() + "; " + Application.Application_Name() ] ) return( 1 );

 // now check also owner limits that might exists...
 string ConfigLimit = NormalizeString( Parse_XML( Owner_Allow(), Owner ) );
 if( ConfigLimit.empty() ) ConfigLimit = NormalizeString( Parse_XML( Owner_Allow(), "any" ) );
 
 string ProjectLimit = NormalizeString( Parse_XML( Project.Owner_Allow(), Owner ) );
 if( ProjectLimit.empty() ) ProjectLimit = NormalizeString( Parse_XML( Project.Owner_Allow(), "any" ) );

 string ApplicationLimit = NormalizeString( Parse_XML( Application.Owner_Allow(), Owner ) );
 if( ApplicationLimit.empty() ) ApplicationLimit = NormalizeString( Parse_XML( Application.Owner_Allow(), "any" ) );

 // if no limits for this owner defined, then signal this...
 if( ConfigLimit.empty() && ProjectLimit.empty() && ApplicationLimit.empty() ) return( 4 );

 // then check limits that have been found for this owner...
 if( !ConfigLimit.empty() ) 
  if( atoi( ConfigLimit.c_str() ) <= Accounting[ Owner ] ) return( 2 );

 if( !ProjectLimit.empty() ) 
  if( atoi( ProjectLimit.c_str() ) <= Accounting[ Owner + ", " + Project.Project_Name() ] ) return( 2 );

 if( !ApplicationLimit.empty() ) 
  if( atoi( ApplicationLimit.c_str() ) <= Accounting[ Owner + ", " + Project.Project_Name() + ", " + Application.Application_Name() ] ) return( 2 );

 // otherwise signal that no limit was encountered for this owner...
 return( 8 );
}

// -----------------------------------------------------------------------------

void Daemon::StopScheduling( void )
{
 NORMAL_LOG( "Daemon::StopScheduling; Received stop signal" );
 ReadyForScheduling = 0;
}

// -----------------------------------------------------------------------------

int Daemon::IsSchedularReady( void )
{
 return( ReadyForScheduling );
}

// -----------------------------------------------------------------------------

int Daemon::RunSchedular( void )
{
 if( !ReadyForScheduling ) CRITICAL_LOG_RETURN( ReadyForScheduling, "Daemon::RunSchedular; Daemon was not ready for schedulig" );

 time_t LastRequestTime = 0;
 time_t LastUpdateTime = 0;
 time_t RequestDelay = TheSlowCycleTime;

 do
 {

  if( time( NULL ) - LastRequestTime >= RequestDelay )        // check for work every slow cycle time seconds...
  {
   if( RequestWorkCycle() )                    // if we got some work, wait for fast cycle time now and ask for more... 
    RequestDelay = TheFastCycleTime;
   else
    RequestDelay = TheSlowCycleTime;
   LastRequestTime = time( NULL );
  }
  else
   JobsObtained = 0;

  if( ( time( NULL ) - LastUpdateTime >= TheFastCycleTime ) || ( JobsObtained ) )      // check our jobs every fast cycle time...
  {
   if( !Jobs.empty() )
   {
    CycleThroughJobs(); 

    if( JobsFinished )                            // if we have jobs finished, we can request new work now... 
     RequestDelay = TheFastCycleTime;
   }
   LastUpdateTime = time( NULL );
  }

  sleep( 1 );                // sleep for a second each time...

 } while( ReadyForScheduling );

 return( ReadyForScheduling );
}

// -----------------------------------------------------------------------------
