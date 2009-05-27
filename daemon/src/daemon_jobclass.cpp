/* []--------------------------------------------------------[]
    |                   daemon_jobclass.cpp                  |
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

#include "daemon_jobclass.h"

// -----------------------------------------------------------------------------

int UnlinkDirectoryRecursively( string Directory )
{
 string Name;
 struct dirent *Entry;
 struct stat FileStat;
 DIR *Dir = opendir( Directory.c_str() );

 if( Dir == NULL ) return( 0 );

 while( ( Entry = readdir( Dir ) ) != NULL )
 {
  if( !strcmp( Entry -> d_name, "." ) ) continue;
  if( !strcmp( Entry -> d_name, ".." ) ) continue;

  Name = Directory + "/" + string( Entry -> d_name );

  if( stat( Name.c_str(), &FileStat ) ) continue;

  if( S_ISDIR( FileStat.st_mode ) )                        // check if it is a directory...
  { 
   if( !UnlinkDirectoryRecursively( Name ) ) 
   {
    closedir( Dir );
    return( 0 );
   }
  }
  else
   if( unlink( Name.c_str() ) )
   {
    closedir( Dir );
    return( 0 );
   }
 }

 closedir( Dir );

 if( rmdir( Directory.c_str() ) ) return( 0 );

 return( 1 );
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( void )
{
 JobDirectory.empty();
 ErrorNumber = 0xFFFF;
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheJobDirectory )
{
 JobDirectory.clear();

 DIR *Entry = opendir( TheJobDirectory.c_str() );

 if( Entry == NULL )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not open and read from JobDirectory=" << TheJobDirectory );
  return;
 }
 else
  closedir( Entry );

 // now check if some critical files should be present, readable and untouched...

 string Data;

 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_APPLICATION_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_TARGET_RESOURCES_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ID_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_OWNERS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_READ_ACCESS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_TIME_STAMP_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_SPECIFICS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_INPUT_FILE << " seems corrupt in " << TheJobDirectory ); return; }

 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_RUN_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ABORT_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }

 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_KEY_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_CERTIFICATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_CA_CERTIFICATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( !ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE, Data ) ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE << " seems corrupt in " << TheJobDirectory ); return; }

 // all files are there and are okay... accept the job directory...

 JobDirectory = TheJobDirectory;
 ErrorNumber = 0;

 NORMAL_LOG( "DaemonJob::DaemonJob; Set up job with JobDirectory=" << TheJobDirectory );
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber )
{
 DaemonConfigProjectApplication Application;
 string TheHash, TheScript;

 DEBUG_LOG( "DaemonJob::DaemonJob; Setting up job with XML=" << TheXML << " for ProjectNumber=" << ProjectNumber << " and ApplicationNumber=" << ApplicationNumber );

 JobDirectory.clear();

 // first check if config matches the XML response...
 if( NormalizeString( Parse_XML( TheXML, "project" ) ) != TheConfig.Project( ProjectNumber ).Project_Name() )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; The XML response project does not match the one specified in the config" );
  return;
 }

 TheScript = NormalizeString( Parse_XML( TheXML, "application" ) );
 if( TheScript.empty() ) TheScript = NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "application" ) ); 
 if( TheScript != TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Application_Name() )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; The XML response application does not match the one specified in the config" );
  return;
 }

 // then check if run dir is readable...
 DIR *Entry = opendir( TheConfig.RunDirectory().c_str() );

 if( Entry == NULL )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not open and read from RunDirectory=" << TheConfig.RunDirectory() );
  return;
 } 
 else
  closedir( Entry );

 // now make subdirectories needed for this job...
 JobDirectory = TheConfig.RunDirectory() + "/" + TheConfig.Project( ProjectNumber ).Project_Name();
 mkdir( JobDirectory.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
 if( !getuid() ) chmod( JobDirectory.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );

 Application = TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ); 
 JobDirectory = JobDirectory + "/" + Application.Application_Name();
 mkdir( JobDirectory.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
 if( !getuid() ) chmod( JobDirectory.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );

 // now set sandbox uid if possible...
 int UID = getuid();
 if( UID == 0 )                                      // if we are running daemon as root...
 {
  UID = Application.Job_Sandbox_UID();               // get sandbox uid from config...
  if( UID <= 0 ) UID = rand() % 10000 + 5000;        // if none given or invalid, take a random one...
  DEBUG_LOG( "DaemonJob::DaemonJob; Using uid " << UID << " for sandboxing job" );
 }
 else
  DEBUG_LOG( "DaemonJob::DaemonJob; Deamon not running as root, no sandboxing possible" );

 // create job slot directory...
 BinHex( Hash( TheXML ), TheHash );
 JobDirectory = JobDirectory + "/JOB_" + TheHash;
 if( mkdir( JobDirectory.c_str(), S_IRWXU ) )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not create job directory for JobDirectory=" << JobDirectory );
  JobDirectory.clear();
  return;
 }
 chown( JobDirectory.c_str(), UID, UID ); 

 // then start writing stuff to disk from the response...
 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "project" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "this_project_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "project_master_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = NormalizeString( Parse_XML( TheXML, "application" ) );
 if( TheScript.empty() ) TheScript = NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "application" ) ); 
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE ).c_str(),  UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_id" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "owners" ) ), JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "read_access" ) ), JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = NormalizeString( Parse_XML( TheXML, "state" ) );
 if( TheScript.empty() ) TheScript = NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state" ) ); 
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 // we specifically do NOT write the input and output files yet because they are not present in the current
 // data and it allows the dameon detect the temporary job dir as being corrupted the next time it is 
 // restarted. if the job is accepted, the update cyle will generate these files with correct content.

 // then dump the scripts there...
 TheScript = ReadStringFromFile( Application.Job_Check_Limits_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Check_Running_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Check_Finished_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Prologue_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Run_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Epilogue_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 TheScript = ReadStringFromFile( Application.Job_Abort_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).c_str(), S_IRUSR | S_IXUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT + HASHFILE_EXTENTION ).c_str(), UID, UID );

 // write the SSL config too...
 WriteStringToHashedFile( TheConfig.Resource_Key_File(), JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( TheConfig.Resource_Certificate_File(), JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 WriteStringToHashedFile( TheConfig.CA_Certificate_File(), JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 // dump max output file size too..
 char TempBuffer[ 64 ];
 int ServerMaxFieldSize = atoi( NormalizeString( Parse_XML( TheXML, "server_max_field_size" ) ).c_str() );
 sprintf( TempBuffer, "%d", ( ServerMaxFieldSize > 1 ? MIN( ServerMaxFieldSize, Application.Max_Output_Size() ) : Application.Max_Output_Size() ) );
 WriteStringToHashedFile( string( TempBuffer ), JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 // write sandbox uid file...
 sprintf( TempBuffer, "%d", UID );
 WriteStringToHashedFile( string( TempBuffer ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 NORMAL_LOG( "DaemonJob::DaemonJob; Job with JobDirectory=" << JobDirectory << " has been setup" );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobDirectory( void )
{ 
 VERBOSE_DEBUG_LOG_RETURN( JobDirectory, "DaemonJob::GetJobDirectory; Returned " << JobDirectory );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetProject( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetProject; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetProject; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetThisProjectServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetThisProjectServer; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetThisProjectServer; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetProjectMasterServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetProjectMasterServer; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetProjectMasterServer; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetApplication( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetApplication; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetApplication; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetTargetResources( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetTargetResources; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetTargetResources; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobId( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetJobId; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetJobId; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetOwners( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetOwners; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetOwners; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetReadAccess( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetReadAccess; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetReadAccess; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobSpecifics( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetJobSpecifics; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetJobSpecifics; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetInput( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetInput; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetInput; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetOutput( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetOutput; JobDirectory empty" );
 string Data = ReadStringFromFile( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE, GetMaxOutputSize() );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetOutput; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetKeyFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetKeyFile; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetKeyFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

int DaemonJob::GetMaxOutputSize( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::GetMaxOutputSize; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_MAX_OUTPUT_SIZE_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( atoi( NormalizeString( Data ).c_str() ), "DaemonJob::GetMaxOutputSize; Returned " << Data );
}

// -----------------------------------------------------------------------------

int DaemonJob::GetErrorNumber( void )
{
 return( ErrorNumber );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetCertificateFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetCertificateFile; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetCertificateFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetCACertificateFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetCACertificateFile; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetCACertificateFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetState( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetState; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetState; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetStateTimeStamp( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetStateTimeStamp; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetStateTimeStamp; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobSandboxUID( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetJobSandboxUID; JobDirectory empty" );
 string Data; ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SANDBOX_UID_FILE, Data );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetJobSandboxUID; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetStateFromServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetStateFromServer; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_Job_State( Response, GetThisProjectServer(), GetProject(), GetJobId() ) )
   CRITICAL_LOG_RETURN( Response, "DaemonJob::GetStateFromServer; Could not read from server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );
 
 if( ErrorNumber ) CRITICAL_LOG_RETURN( "", "DaemonJob::GetStateFromServer; Error from server Response=" << Response );

 Response = Parse_XML( Parse_XML( Response, "job" ), "state" );

 VERBOSE_DEBUG_LOG_RETURN( Response, "DaemonJob::GetStateFromServer; Returned " << Response );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetStateTimeStampFromServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetStateTimeStampFromServer; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_Job_State( Response, GetThisProjectServer(), GetProject(), GetJobId() ) )
   CRITICAL_LOG_RETURN( Response, "DaemonJob::GetStateTimeStampFromServer; Could not read from server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( "", "DaemonJob::GetStateTimeStampFromServer; Error from server Response=" << Response );

 Response = Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" );

 VERBOSE_DEBUG_LOG_RETURN( Response, "DaemonJob::GetStateTimeStampFromServer; Returned " << Response );
}

// -----------------------------------------------------------------------------

void DaemonJob::CleanUpJobDirectory( void )
{
 if( JobDirectory.empty() ) { CRITICAL_LOG( "DaemonJob::CleanUpJobDirectory; JobDirectory empty" ); return; }

 if( !UnlinkDirectoryRecursively( JobDirectory ) )
  CRITICAL_LOG( "DaemonJob::CleanUpJobDirectory; Error during cleanup of JobDirectory=" << JobDirectory )
 else
  NORMAL_LOG( "DaemonJob::CleanUpJobDirectory; Cleaned up job from directory JobDirectory=" << JobDirectory );

 JobDirectory.empty();
}

// -----------------------------------------------------------------------------

int DaemonJob::UpdateJob( string State, string Resources, string Input, string Output, string Specs )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; JobDirectory empty" );
 if( SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; SessionID empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response, Data, HexedInput, HexedOutput;

 if( !Input.empty() ) BinHex( Input, HexedInput );
 if( !Output.empty() )                                    // check the size of the output here and cut it off if needed...
 {
  if( Output.size() > GetMaxOutputSize() )
  { 
   Output.erase( GetMaxOutputSize() );
   CRITICAL_LOG( "DaemonJob::UpdateJob; Output truncated to " << GetMaxOutputSize() << " bytes" );
  }
  BinHex( Output, HexedOutput );
 }

 do
 {
  if( Server.Resource_Update_Job( Response, GetThisProjectServer(), GetProject(), SessionID, GetJobId(), State, Resources, HexedInput, HexedOutput, Specs ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; Error from server " << GetThisProjectServer() << " Response=" << Response );

 // Dump obtained details of job into correct files...
 int UID = atoi( GetJobSandboxUID().c_str() );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), Data );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 if( !HexedOutput.empty() )      // only update output file if we posted output to server...
 {
  HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "output" ) ), Data );
  chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), S_IRUSR | S_IWUSR );
  WriteStringToFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );
  chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), S_IRUSR | S_IWUSR );
  chown( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), UID, UID );
 }

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::UpdateJob; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::SetState( string State )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SetState; JobDirectory empty" );
 int Value = UpdateJob( State, "", "", "", "" ); 
 VERBOSE_DEBUG_LOG_RETURN( Value, "DaemonJob::SetState; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob:: SetJobSpecifics( string Specs )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SetJobSpecifics; JobDirectory empty" );
 int Value = UpdateJob( "", "", "", "", Specs ); 
 VERBOSE_DEBUG_LOG_RETURN( Value, "DaemonJob::SetJobSpecifics; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::SetTargetResources( string Resources )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SetTargetResources; JobDirectory empty" );
 int Value = UpdateJob( "", Resources, "", "", "" ); 
 VERBOSE_DEBUG_LOG_RETURN( Value, "DaemonJob::SetTargetResources; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::SetInput( string Input )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SetInput; JobDirectory empty" );
 int Value = UpdateJob( "", "", Input, "", "" ); 
 VERBOSE_DEBUG_LOG_RETURN( Value, "DaemonJob::SetInput; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::SetOutput( string Output )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SetOutput; JobDirectory empty" );
 int Value = UpdateJob( "", "", "", Output, "" ); 
 VERBOSE_DEBUG_LOG_RETURN( Value, "DaemonJob::SetOutput; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::LockJob( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; JobDirectory empty" );
 if( SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; SessionID empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_Lock_Job( Response, GetThisProjectServer(), GetProject(), SessionID, GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::LockJob; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::UnLockJob( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; JobDirectory empty" );
 if( SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; SessionID empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_UnLock_Job( Response, GetThisProjectServer(), GetProject(), SessionID, GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::UnLockJob; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::SignUp( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; JobDirectory empty" );
 if( !SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; SessionID not empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_SignUp_Resource( Response, GetThisProjectServer(), GetProject() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; Error from server Response=" << Response );

 SessionID = NormalizeString( Parse_XML( Response, "session_id" ) );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::SignUp; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::SignOff( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; JobDirectory empty" );
 if( SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; SessionID empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_SignOff_Resource( Response, GetThisProjectServer(), GetProject(), SessionID ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; Error from server Response=" << Response );

 SessionID.clear();

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::SignOff; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::UpdateJobFromServer( bool UpdateOutputToo )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; JobDirectory empty" );
 if( SessionID.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; SessionID empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response, Data;

 do
 {
  if( Server.Resource_Request_Job_Details( Response, GetThisProjectServer(), GetProject(), SessionID, GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
  ErrorNumber = atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() );
 } while( ErrorNumber == LGI_SERVER_BACKOFF_ERROR_NR );

 if( ErrorNumber ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; Error from server Response=" << Response );

 // Dump obtained details of job into correct files...
 int UID = atoi( GetJobSandboxUID().c_str() );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), Data );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), S_IWUSR );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), S_IWUSR );
 WriteStringToHashedFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).c_str(), UID, UID );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), S_IRUSR );
 chown( ( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE + HASHFILE_EXTENTION ).c_str(), UID, UID );

 // we do not update the output from server here by default...
 if( UpdateOutputToo )
 {
  HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "output" ) ), Data );
  chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), S_IRUSR | S_IWUSR );
  WriteStringToFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );
  chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), S_IRUSR | S_IWUSR );
  chown( ( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE ).c_str(), UID, UID );
 }

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::UpDateJobFromServer; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckLimitsScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckLimitsScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckLimitsScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT );

 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckLimitsScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckRunningScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckRunningScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckRunningScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT );

 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckRunningScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckFinishedScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckFinishedScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckFinishedScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT );

 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckFinishedScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobPrologueScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobPrologueScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobPrologueScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT );

 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobPrologueScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobEpilogueScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobEpilogueScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobEpilogueScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT );

 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobEpilogueScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobRunScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobRunScript; JobDirectory empty" );

 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobRunScript; Script seems corrupt, refusing to run it" );

 int first_pid, status;

 if( ( first_pid = fork() ) == 0 )   // fork and let child fork again to exec script
 {                                   // so that first forked child can exit and parent  
  setsid();                          // can wait on it to avoid zombies...
  setpgid( 0, 0 );

  if( fork() == 0 )                      // this child will become a child of 'init' cause its parent quitted...
  {
   int UID = atoi( GetJobSandboxUID().c_str() );      // set real and effective uid and gid correctly of child...

   setregid( UID, UID );
   setreuid( UID, UID );
 
   chdir( JobDirectory.c_str() );             // get into right directory and exec the run script...

   _exit( execl( "/bin/sh", "sh", "-c", ( "./" LGI_JOBDAEMON_JOB_RUN_SCRIPT ), NULL ) );
  }

  _exit( 0 );                           // here we quit the parent of the above child...
 }

 waitpid( first_pid, &status, 0 );      // and here we wait on the first forked child that we quited above...

 NORMAL_LOG_RETURN( 0, "DamonJob::RunJobRunScript; Script started on background for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetSessionID( void )
{
 return( SessionID );
}

// -----------------------------------------------------------------------------

void DaemonJob::SetSessionID( string ID )
{
 SessionID = ID;
}
 
// -----------------------------------------------------------------------------

int DaemonJob::RunJobAbortScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobAbortScript; JobDirectory empty" );
 string Data; if( !ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT, Data ) ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobAbortScript; Script seems corrupt, refusing to run it" );

 int Value = RunAScript( LGI_JOBDAEMON_JOB_ABORT_SCRIPT );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobAbortScript; Returned " << Value << " for job with directory " << JobDirectory );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunAScript( string TheScriptFile )
{
 int pid, status;

 if( ( pid = fork() ) == 0 )                         // fork and let child run script as needed...
 {                                   
  int UID = atoi( GetJobSandboxUID().c_str() );      // set real and effective uid and gid correctly of child...

  setregid( UID, UID );
  setreuid( UID, UID );

  chdir( JobDirectory.c_str() );                    // get into right directory and exec the run script...

  _exit( execl( "/bin/sh", "sh", "-c", ( "./" + TheScriptFile ).c_str(), NULL ) );
 }

 waitpid( pid, &status, 0 );                        // and here we wait on the forked child...

 return( status );
}

// -----------------------------------------------------------------------------

int ReadStringFromHashedFile( string FileName, string &Data )
{
 fstream HashFile( ( FileName + HASHFILE_EXTENTION ).c_str(), fstream::in );
 string Line, TheHash;

 Data = ReadStringFromFile( FileName );

 BinHex( Hash( Data ), TheHash );

 getline( HashFile, Line );

 if( Line != TheHash )
 {
  Data.clear();
  CRITICAL_LOG_RETURN( 0, "ReadStringFromHashedFile; Data read from file " << FileName << " does not match hash" );
 }

 VERBOSE_DEBUG_LOG_RETURN( 1, "ReadStringFromHashedFile; Data returned from file " << FileName << ": '" << Data << "'" );
}

// -----------------------------------------------------------------------------

void WriteStringToHashedFile( string String, string FileName )
{
 fstream File( FileName.c_str(), fstream::out );
 fstream HashFile( ( FileName + HASHFILE_EXTENTION ).c_str(), fstream::out );
 string  TheHash;

 BinHex( Hash( String ), TheHash );

 File << String;
 HashFile << TheHash;

 VERBOSE_DEBUG_LOG( "WriteStringToHashedFile; Wrote file " << FileName << " with String=" << String );
}

// -----------------------------------------------------------------------------

