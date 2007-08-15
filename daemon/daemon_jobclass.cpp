/* []--------------------------------------------------------[]
    |                   daemon_jobclass.cpp                  |
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

#include "daemon_jobclass.h"

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

 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_APPLICATION_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ID_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_OWNERS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_READ_ACCESS_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_FILE << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_STATE_TIME_STAMP_FILE << " seems corrupt in " << TheJobDirectory ); return; }

 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_RUN_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }
 if( ReadStringFromFile( TheJobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT ).empty() ) { CRITICAL_LOG( "DaemonJob::DaemonJob; File " << LGI_JOBDAEMON_JOB_ABORT_SCRIPT << " seems corrupt in " << TheJobDirectory ); return; }

 // all files are there and are okay... accept the job directory...

 JobDirectory = TheJobDirectory;

 NORMAL_LOG( "DaemonJob::DaemonJob; Set up job with JobDirectory=" << TheJobDirectory );
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber )
{
 string TheHash;

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

 JobDirectory = JobDirectory + "/" + TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Application_Name();
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

 WriteStringToFile( NormalizeString( Parse_XML( TheXML, "project" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( TheXML, "this_project_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_THIS_PROJECT_SERVER_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( TheXML, "project_master_server" ) ), JobDirectory + "/" + LGI_JOBDAEMON_PROJECT_MASTER_SERVER_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "application" ) ), JobDirectory + "/" + LGI_JOBDAEMON_APPLICATION_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_id" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_ID_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "owners" ) ), JobDirectory + "/" + LGI_JOBDAEMON_OWNERS_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "read_access" ) ), JobDirectory + "/" + LGI_JOBDAEMON_READ_ACCESS_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "job_specifics" ) ), JobDirectory + "/" + LGI_JOBDAEMON_JOB_SPECIFICS_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "input" ) ), JobDirectory + "/" + LGI_JOBDAEMON_INPUT_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_FILE );
 WriteStringToFile( NormalizeString( Parse_XML( Parse_XML( TheXML, "job" ), "state_time_stamp" ) ), JobDirectory + "/" + LGI_JOBDAEMON_STATE_TIME_STAMP_FILE );
 
 // finally also dump the scripts there...

 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Check_Limits_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_LIMITS_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Check_Running_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_RUNNING_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Check_Finished_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_CHECK_FINISHED_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Prologue_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_PROLOGUE_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Run_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_RUN_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Epilogue_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_EPILOGUE_SCRIPT );
 WriteStringToFile( TheConfig.Project( ProjectNumber ).Application( ApplicationNumber ).Job_Abort_Script() , JobDirectory + "/" + LGI_JOBDAEMON_JOB_ABORT_SCRIPT );

 NORMAL_LOG( "DaemonJob::DaemonJob; Job with JobDirectory=" << JobDirectory << " has been setup" );
}

// -----------------------------------------------------------------------------
#include <iostream>
string DaemonJob::ReadStringFromFile( string FileName )
{
 fstream File( FileName.c_str(), fstream::in );
 fstream HashFile( ( FileName + HASHFILE_EXTENTION ).c_str(), fstream::in );
 string Buffer, Line, TheHash;

 Buffer.reserve( 1024 );
 Buffer.clear();

 getline( File, Line );
 while( File )
 {
  Buffer.append( Line );
  Buffer.push_back( '\n' );
  getline( File, Line );
 }

 if( Buffer.length() >= 1 ) 
  Buffer = Buffer.substr( 0, Buffer.length() - 1 );
 else
  Buffer.clear();

 BinHex( Hash( Buffer ), TheHash );

 getline( HashFile, Line );

 if( Line != TheHash )
 {
  Buffer.clear();
  CRITICAL_LOG_RETURN( Buffer, "DaemonJob::ReadStringFromFile; Data read from file " << FileName << " does not match hash" );
 }
 
 DEBUG_LOG_RETURN( Buffer, "DaemonJob::ReadStringFromFile; Data returned from file " << FileName << ":  " << Buffer );
}

// -----------------------------------------------------------------------------
 
void DaemonJob::WriteStringToFile( string String, string FileName )
{
 fstream File( FileName.c_str(), fstream::out );
 fstream HashFile( ( FileName + HASHFILE_EXTENTION ).c_str(), fstream::out );
 string  TheHash;

 BinHex( Hash( String ), TheHash );

 File << String;
 HashFile << TheHash;
}

// -----------------------------------------------------------------------------

string DaemonJob::GetJobDirectory( void )
{ 
 return( JobDirectory );
}

// -----------------------------------------------------------------------------
