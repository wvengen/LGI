/* []--------------------------------------------------------[]
    |                   daemon_jobclass.cpp                  |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
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
 DIR *Dir = opendir( Directory.c_str() );

 if( Dir == NULL ) return( 0 );

 while( ( Entry = readdir( Dir ) ) != NULL )
 {
  if( !strcmp( Entry -> d_name, "." ) ) continue;
  if( !strcmp( Entry -> d_name, ".." ) ) continue;

  Name = Directory + "/" + string( Entry -> d_name );

  if( Entry -> d_type & DT_DIR )
  { 
   if( UnlinkDirectoryRecursively( Name ) ) 
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

 // now check if some critical files should be present and untouched...

 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_APPLICATION_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_TARGET_RESOURCES_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ID_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_OWNERS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_READ_ACCESS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_TIME_STAMP_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_INPUT_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_SPECIFICS_FILE << " seems corrupt in " << TheJobDirectory ); return; }

 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_RUN_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ABORT_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }

 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_KEY_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_CERTIFICATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromHashedFile( TheJobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_CA_CERTIFICATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }

 // all files are there and are okay... accept the job directory...

 JobDirectory = TheJobDirectory;

 NORMAL_LOG( "DaemonJob::DaemonJob; Set up job with JobDirectory=" << TheJobDirectory );
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber )
{
 DaemonConfigProjectApplication Application;
 string TheHash, TheScript;

 DEBUG_LOG( "DaemonJob::DaemonJob; Setting up job with XML=" << TheXML << " for ProjectNumber=" << ProjectNumber << " and ApplicationNumber=" << ApplicationNumber );

 JobDirectory.clear();

 // first check if config matches the XML response

 if( NormalizeString( Parse_XML( TheXML, "project" ) ) != TheConfig.Project( ProjectNumber ).Project_Name() )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; The XML response project does not match the one specified in the config" );
  return;
 }

 if( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "application" ) ) != TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Application_Name() )
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
 mkdir( JobDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

 Application = TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ); 

 JobDirectory = JobDirectory + "/" + Application.Application_Name();
 mkdir( JobDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

 BinHex( Hash( TheXML ), TheHash );

 JobDirectory = JobDirectory + "/JOB_" + TheHash;
 if( mkdir( JobDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not create job directory for JobDirectory=" << JobDirectory );
  JobDirectory.clear();
  return;
 }

 // then start writing stuff to disk from the response...

 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "project" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "this_project_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( TheXML, "project_master_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "application" ) ), JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_id" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "owners" ) ), JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "read_access" ) ), JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );

 HexBin( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "input" ) ), TheScript );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );

 HexBin( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "output" ) ), TheScript );
 WriteStringToFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );

 // finally also dump the scripts there...

 TheScript = ReadStringFromFile( Application.Job_Check_Limits_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Check_Running_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Check_Finished_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Prologue_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Run_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Epilogue_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 TheScript = ReadStringFromFile( Application.Job_Abort_Script() );
 WriteStringToHashedFile( TheScript, JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT );
 chmod( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).c_str(), S_IRWXU | S_IRWXG );

 // and finally write the SSL config too...
 WriteStringToHashedFile( TheConfig.Resource_Key_File(), JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE );
 WriteStringToHashedFile( TheConfig.Resource_Certificate_File(), JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE );
 WriteStringToHashedFile( TheConfig.CA_Certificate_File(), JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE );

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
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetProject; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetThisProjectServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetThisProjectServer; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetThisProjectServer; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetProjectMasterServer( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetProjectMasterServer; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetProjectMasterServer; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetApplication( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetApplication; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetApplication; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetTargetResources( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetTargetResources; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetTargetResources; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobId( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetJobId; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetJobId; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetOwners( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetOwners; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetOwners; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetReadAccess( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetReadAccess; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetReadAccess; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobSpecifics( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetJobSpecifics; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetJobSpecifics; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetInput( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetInput; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetInput; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetOutput( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetOutput; JobDirectory empty" );
 string Data = ReadStringFromFile( JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetOutput; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetKeyFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetKeyFile; JobDirectory empty" );
 string Data = ReadStringFromFile( JobDirectory + "/" + LGI_JOBDAEMON_KEY_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetKeyFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetCertificateFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetCertificateFile; JobDirectory empty" );
 string Data = ReadStringFromFile( JobDirectory + "/" + LGI_JOBDAEMON_CERTIFICATE_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetCertificateFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetCACertificateFile( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetCACertificateFile; JobDirectory empty" );
 string Data = ReadStringFromFile( JobDirectory + "/" + LGI_JOBDAEMON_CA_CERTIFICATE_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetCACertificateFile; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetState( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetState; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetState; Returned " << Data );
}

// -----------------------------------------------------------------------------

string DaemonJob::GetStateTimeStamp( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( JobDirectory, "DaemonJob::GetStateTimeStamp; JobDirectory empty" );
 string Data = ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );
 VERBOSE_DEBUG_LOG_RETURN( Data, "DaemonJob::GetStateTimeStamp; Returned " << Data );
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
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );
 
 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( "", "DaemonJob::GetStateFromServer; Error from server Response=" << Response );

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
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( "", "DaemonJob::GetStateTimeStampFromServer; Error from server Response=" << Response );

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

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response, Data, HexedInput, HexedOutput;

 if( !Input.empty() ) BinHex( Input, HexedInput );
 if( !Output.empty() ) BinHex( Output, HexedOutput );

 do
 {
  if( Server.Resource_Update_Job( Response, GetThisProjectServer(), GetProject(), GetJobId(), State, Resources, HexedInput, HexedOutput, Specs ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJob; Error from server " << GetThisProjectServer() << " Response=" << Response );

 // Dump obtained details of job into correct files...

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );

 HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), Data );
 WriteStringToHashedFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );

 if( !HexedOutput.empty() )      // only update output file if we posted output to server...
 {
  HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "output" ) ), Data );
  WriteStringToFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );
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

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_Lock_Job( Response, GetThisProjectServer(), GetProject(), GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::LockJob; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::LockJob; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::UnLockJob( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_UnLock_Job( Response, GetThisProjectServer(), GetProject(), GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UnLockJob; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::UnLockJob; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::SignUp( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_SignUp_Resource( Response, GetThisProjectServer(), GetProject() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignUp; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::SignUp; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::SignOff( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response;

 do
 {
  if( Server.Resource_SignOff_Resource( Response, GetThisProjectServer(), GetProject() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::SignOff; Error from server Response=" << Response );

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::SignOff; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::UpdateJobFromServer( bool UpdateOutputToo )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; JobDirectory empty" );

 Resource_Server_API Server( GetKeyFile(), GetCertificateFile(), GetCACertificateFile() );
 string Response, Data;

 do
 {
  if( Server.Resource_Request_Job_Details( Response, GetThisProjectServer(), GetProject(), GetJobId() ) )
   CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; Could not post to server " << GetThisProjectServer() );
  Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );
 } while( atoi( NormalizeString( Parse_XML( Parse_XML( Response, "error" ), "number" ) ).c_str() ) == SERVER_BACK_OF_ERROR_NR );

 if( !Parse_XML( Response, "error" ).empty() ) CRITICAL_LOG_RETURN( 0, "DaemonJob::UpdateJobFromServer; Error from server Response=" << Response );

 // Dump obtained details of job into correct files...

 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "target_resources" ) ), JobDirectory + "/" + LGI_JOBDAEMON_TARGET_RESOURCES_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 WriteStringToHashedFile( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );

 HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "input" ) ), Data );
 WriteStringToHashedFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );

 // we do not update the output from server here by default...
 if( UpdateOutputToo )
 {
  HexBin( NormalizeString( Parse_XML( Parse_XML( Response, "job" ), "output" ) ), Data );
  WriteStringToFile( Data, JobDirectory + "/" + LGI_JOBDAEMON_OUTPUT_FILE );
 }

 VERBOSE_DEBUG_LOG_RETURN( 1, "DaemonJob::UpDateJobFromServer; Response=" << Response << " returned 1" );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckLimitsScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckLimitsScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckLimitsScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckLimitsScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckRunningScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckRunningScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckRunningScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckRunningScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobCheckFinishedScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckFinishedScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobCheckFinishedScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobCheckFinishedScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobPrologueScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobPrologueScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobPrologueScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobPrologueScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobEpilogueScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobEpilogueScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobEpilogueScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobEpilogueScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobRunScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobRunScript; JobDirectory empty" );

 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobRunScript; Script seems corrupt, refusing to run it" );

 if( fork() == 0 )   // fork and child is going to start the script...
 {
  // int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).c_str() );
  int Value = execl( "/bin/sh", "sh", "-c", ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).c_str(), NULL );
  _exit( Value );
 }

 NORMAL_LOG_RETURN( 0, "DamonJob::RunJobRunScript; Script started on background" );
}

// -----------------------------------------------------------------------------

int DaemonJob::RunJobAbortScript( void )
{
 if( JobDirectory.empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobAbortScript; JobDirectory empty" );
 if( ReadStringFromHashedFile( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).empty() ) CRITICAL_LOG_RETURN( 1, "DaemonJob::RunJobAbortScript; Script seems corrupt, refusing to run it" );
 int Value = system( ( JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).c_str() );
 NORMAL_LOG_RETURN( Value, "DamonJob::RunJobAbortScript; Returned " << Value );
}

// -----------------------------------------------------------------------------

string ReadStringFromHashedFile( string FileName )
{
 fstream HashFile( ( FileName + HASHFILE_EXTENTION ).c_str(), fstream::in );
 string Buffer, Line, TheHash;

 Buffer = ReadStringFromFile( FileName );

 BinHex( Hash( Buffer ), TheHash );

 getline( HashFile, Line );

 if( Line != TheHash )
 {
  Buffer.clear();
  CRITICAL_LOG_RETURN( Buffer, "ReadStringFromHashedFile; Data read from file " << FileName << " does not match hash" );
 }

 VERBOSE_DEBUG_LOG_RETURN( Buffer, "ReadStringFromHashedFile; Data returned from file " << FileName << ": " << Buffer );
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

