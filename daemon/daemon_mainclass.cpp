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
     if( TempJob.GetState() == "aborting" )                       // do we need to abort the job...
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

int Daemon::RequestWorkCycle( void )
{
 DEBUG_LOG( "Daemon::RequestWorkCycle; Starting with request work cycle" );

 Resource_Server_API ServerAPI( Resource_Key_File(), Resource_Certificate_File(), CA_Certificate_File() );

 for( int nP = 1; nP <= Number_Of_Projects(); ++nP )     // cycle through all projects...
 {
  DaemonConfigProject TheProject;

  TheProject = Project( nP );

  // first signup to registered master server of this project to get all slaves... 

  list<string> ServerList;
  string Response, Attributes;
  int    StartPos;

  if( ServerAPI.Resource_SignUp_Resource( Response, TheProject.Project_Master_Server(), TheProject.Project_Name() ) != CURLE_OK ) continue;

  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

  if( Response.empty() ) continue;

  if( !Parse_XML( Response, "error" ).empty() ) continue;

  int NumberOfServers = atoi( NormalizeString( Parse_XML( Response, "number_of_slave_servers" ) ).c_str() );

  for( int nS = StartPos = 0; nS < NumberOfServers; nS++ )
  {
   string SlaveServer = Parse_XML( Response, "project_server", Attributes, StartPos );
   if( SlaveServer.empty() ) continue; 
   ServerList.insert( ServerList.begin(), SlaveServer );
  }

  // now add also master server reported in the response, and the one we just asked...
 
  ServerList.insert( ServerList.end(), Parse_XML( Response, "project_master_server" ) ); 

  ServerList.insert( ServerList.begin(), TheProject.Project_Master_Server() ); 

  list<string>::iterator ServerPointer = ServerList.begin();

  // the list is complete and we are signed up to the first (master) server...

  do
  {
   if( !Response.empty() )
   {
    string ExtraJobDetailsTags = "<project> " + TheProject.Project_Name()  + " </project> <this_project_server> " +
                                 (*ServerPointer) + " </this_project_server> <project_master_server> " +
                                 Parse_XML( Response, "project_master_server" ) + " </project_master_server> ";

    // now check all applications for this project on this server...

    for( int nA = 1; nA <= TheProject.Number_Of_Applications(); ++nA ) 
    {
     DaemonConfigProjectApplication TheApplication;

     TheApplication = TheProject.Application( nA );

     // check if there is a system wide limit reached for this application...

     if( system( TheApplication.Check_System_Limits_Script().c_str() ) == 0 ) 
     {
      DEBUG_LOG( "Daemon::RequestWorkCycle; Requesting work for application " << TheApplication.Application_Name() << " of project " << TheProject.Project_Name() << " at server " << (*ServerPointer) );

      if( ServerAPI.Resource_Request_Work( Response, (*ServerPointer), TheProject.Project_Name(), TheApplication.Application_Name() ) != CURLE_OK ) continue;

      string JobResponse = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

      if( JobResponse.empty() ) continue;

      if( !Parse_XML( JobResponse, "error" ).empty() ) continue;

      int NrOfJobs = atoi( NormalizeString( Parse_XML( JobResponse, "number_of_jobs" ) ).c_str() );

      for( int JobIndex = StartPos = 0; JobIndex < NrOfJobs; ++JobIndex )        // loop over jobs in response...
      {
       string JobData = NormalizeString( Parse_XML( JobResponse, "job", Attributes, StartPos ) );
       if( JobData.empty() ) continue;

       string Job_Id = NormalizeString( Parse_XML( JobData, "job_id" ) );
       if( Job_Id.empty() ) continue;

       // now check if any of the owners is denied serving...

       int OwnerIndex;
       vector<string> Owners = CommaSeparatedValues2Array( Parse_XML( JobResponse, "owners" ) );

       for( OwnerIndex = 0; OwnerIndex < Owners.size(); ++OwnerIndex )
       {
        if( IsOwnerDenied( Owners[ OwnerIndex ], TheProject, TheApplication ) ) break;
        if( IsOwnerRunningToMuch( Owners[ OwnerIndex ], TheProject, TheApplication ) ) break;
       }

       if( OwnerIndex == Owners.size() )      // no denials or limits reached for any of the owners...
       {
        // create temporary job directory with the jobs response data...
        DaemonJob TempJob( ExtraJobDetailsTags + "<job> " + JobData + " </job>", (*this), nP, nA );

        // see if job has limits from job limits script somehow...
        if( TempJob.RunJobCheckLimitsScript() == 0 )
        {
         // there are no limits and job is ready to be included for running...
         AddJobToDaemonLists( TempJob );

         // set job into running state on server and through this we now also get input...
         TempJob.UpdateJob( "running", Resource_Name(), "", "", "" );
        }
       }

       if( ServerAPI.Resource_UnLock_Job( Response, (*ServerPointer), TheProject.Project_Name(), Job_Id ) != CURLE_OK ) JobIndex = NrOfJobs;
      }

     }
     else
      VERBOSE_DEBUG_LOG( "Daemon::RequestWorkCycle; System limit reached for application " << TheApplication.Application_Name() << " of project " << TheProject.Project_Name() );
    }

    // sign off from this server...
    if( ServerAPI.Resource_SignOff_Resource( Response, (*ServerPointer), TheProject.Project_Name() ) != CURLE_OK )
     Response.clear();
   }

   if( (++ServerPointer) != ServerList.end() )         // go to next server in list and sign up there...
   {
    if( ServerAPI.Resource_SignUp_Resource( Response, (*ServerPointer), TheProject.Project_Name() ) != CURLE_OK )
     Response.clear();
    else
    {
     Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
     if( !Parse_XML( Response, "error" ).empty() ) Response.clear();
    }
   }
   else
    Response.clear();
  }
  while( ServerPointer != ServerList.end() );

 } 

 NORMAL_LOG_RETURN( 1, "Daemon::RequestWorkCycle; Request for work cycle done" ); 
}

// -----------------------------------------------------------------------------

int Daemon::IsOwnerDenied( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application )
{
 vector<string> ConfigOwners = CommaSeparatedValues2Array( Owner_Deny() );

 for( int i = 0; i < ConfigOwners.size(); ++i )
 {
  if( Owner == ConfigOwners[ i ] ) return( 1 );
  if( ConfigOwners[ i ] == "any' ) return( 1 );
 }

 vector<string> ProjectOwners = CommaSeparatedValues2Array( Project.Owner_Deny() );

 for( int i = 0; i < ProjectOwners.size(); ++i )
 {
  if( Owner == ProjectOwners[ i ] ) return( 1 );
  if( ProjectOwners[ i ] == "any' ) return( 1 );
 }

 vector<string> ApplicationOwners = CommaSeparatedValues2Array( Application.Owner_Deny() );

 for( int i = 0; i < ApplicationOwners.size(); ++i )
 {
  if( Owner == ApplicationOwners[ i ] ) return( 1 );
  if( ApplicationOwners[ i ] == "any' ) return( 1 );
 }

 return( 0 );
}

// -----------------------------------------------------------------------------

int Daemon::IsOwnerRunningToMuch( string Owner, DaemonConfigProject &Project, DaemonConfigProjectApplication &Application )
{
 string ConfigLimit = NormalizeString( Parse_XML( Owner_Allow(), Owner ) );
 if( ConfigLimit.empty() ) ConfigLimit = NormalizeString( Parse_XML( Owner_Allow(), "any" ) );
 
 string ProjectLimit = NormalizeString( Parse_XML( Project.Owner_Allow(), Owner ) );
 if( ProjectLimit.empty() ) ProjectLimit = NormalizeString( Parse_XML( Project.Owner_Allow(), "any" ) );

 string ApplicationLimit = NormalizeString( Parse_XML( Application.Owner_Allow(), Owner ) );
 if( ApplicationLimit.empty() ) ApplicationLimit = NormalizeString( Parse_XML( Application.Owner_Allow(), "any" ) );

 if( ConfigLimit.empty() && ProjectLimit.empty() && ApplicationLimit.empty() ) return( 1 );

 if( !ConfigLimit.empty() ) 
  if( atoi( ConfigLimit.c_str() ) < Accounting[ Owner ] ) return( 1 );

 if( !ProjectLimit.empty() ) 
  if( atoi( ProjectLimit.c_str() ) < Accounting[ Owner + ", " + Project.Project_Name() ] ) return( 1 );

 if( !ApplicationLimit.empty() ) 
  if( atoi( ApplicationLimit.c_str() ) < Accounting[ Owner + ", " + Project.Project_Name() + ", " + Application.Application_Name() ] ) return( 1 );

 return( 0 );
}

// -----------------------------------------------------------------------------

