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

 if( Entry != NULL )
 {
  closedir( Entry );
  JobDirectory = TheJobDirectory;
  NORMAL_LOG( "DaemonJob::DaemonJob; Set up job with JobDirectory=" << TheJobDirectory );
 }
 else
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not open and read from JobDirectory=" << TheJobDirectory );
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheXML, DaemonConfig TheConfig, int ProjectNumber, int ApplicationNumber )
{
 DEBUG_LOG( "DaemonJob::DaemonJob; Setting up job with XML=" << TheXML << " for ProjectNumber=" << ProjectNumber << " and ApplicationNumber=" << ApplicationNumber );

 JobDirectory.clear();

 // first check if run dir is readable...

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
 if( mkdir( JobDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) )
 {
  CRITICAL_LOG( "DaemonJob::DaemonJob; Could not create job directory for JobDirectory=" << JobDirectory );
  JobDirectory.clear();
  return;
 }

 NORMAL_LOG( "DaemonJob::DaemonJob; Set up job with JobDirectory=" << JobDirectory );
}

// -----------------------------------------------------------------------------

string DaemonJob::ReadStringFromFile( string FileName )
{
 fstream File( FileName.c_str(), fstream::in );
 string Buffer, Line;

 Buffer.reserve( 1024 );

 while( File )
 {
  getline( File, Line );
  Buffer.append( Line );
  Buffer.push_back( '\n' );
 }

 if( Buffer.length() >= 2 ) 
  Buffer = Buffer.substr( 0, Buffer.length() - 2 );
 else
  Buffer.clear();
 
 DEBUG_LOG_RETURN( Buffer, "DaemonJob::ReadStringFromFile; Data returned from file " << FileName << ":  " << Buffer );
}

// -----------------------------------------------------------------------------
 
void DaemonJob::WriteStringToFile( string String, string FileName )
{
 fstream File( FileName.c_str(), fstream::out );

 File << String;
}

// -----------------------------------------------------------------------------
