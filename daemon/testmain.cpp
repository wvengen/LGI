#include <cstdlib>

#include "logger.h"
#include "resource_server_api.h"
#include "xml.h"
#include "csv.h"
#include "binhex.h"
#include "hash.h"
#include "daemon_configclass.h"
#include "daemon_jobclass.h"

#define main_4 main

int main_5( int *argc, char *argv[] )
{ 
 string S, S2, S3;

 S.clear();
 for( int i = 0; i < 256; ++i )
  S.push_back( ( char )( i ) );

 S2 = Hash( S );
 BinHex( S2, S3 );

 cout << S3 << endl;
}

int main_4( int *argc, char *argv[] )
{
 DaemonConfigProject MyProject;
 DaemonConfigProjectApplication MyApplication;
 vector<DaemonJob> JobList, JobList2;
 string Response, Response2, Attributes, Job_Id;
 int StartStop, NrOfJobs, i;

 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING,"testmain.log");
 // InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING,"testmain.log");
 
 DaemonConfig Config( "LGI.cfg" );

 if( !Config.IsValidConfigured() ) CRITICAL_LOG_RETURN( 1, "Aborted because of bad config" );

 MyProject = Config.Project( 1 );
 MyApplication = MyProject.Application( 1 );

 Resource_Server_API ServerAPI( Config.Resource_Key_File(), Config.Resource_Certificate_File(), Config.CA_Certificate_File() );

 ServerAPI.Resource_SignUp_Resource( Response, MyProject.Project_Master_Server(), MyProject.Project_Name() );

 ServerAPI.Resource_Request_Work( Response, MyProject.Project_Master_Server(), MyProject.Project_Name(), MyApplication.Application_Name() );

 Response = Parse_XML( Parse_XML( Response, "LGI" ), "response" );

 NrOfJobs = atoi( NormalizeString( Parse_XML( Response, "number_of_jobs" ) ).c_str() );
 StartStop = 0;

 for( i = 0; i < NrOfJobs; ++i )
 {
  Job_Id = NormalizeString( Parse_XML( Parse_XML( Response, "job", Attributes, StartStop ), "job_id", Attributes ) );

  ServerAPI.Resource_Request_Job_Details( Response2, MyProject.Project_Master_Server(), MyProject.Project_Name(), Job_Id );
  cout << "Resource_Request_Job_Details " << Job_Id << " Response: " << Response2 << endl;

  Response2 = Parse_XML( Parse_XML( Response2, "LGI" ), "response" );

  JobList.push_back( DaemonJob( Response2, Config, 1, 1 ) );

  ServerAPI.Resource_UnLock_Job( Response2, MyProject.Project_Master_Server(), MyProject.Project_Name(), Job_Id );
 }
 
 ServerAPI.Resource_SignOff_Resource( Response, MyProject.Project_Master_Server(), MyProject.Project_Name() );
 
 for( i = 0; i < JobList.size(); ++i )
  JobList2.push_back( DaemonJob( JobList[ i ].GetJobDirectory() ) );

 for( i = 0; i < JobList.size(); ++i )
 {
  if( !JobList2[ i ].SignUp() ) cout << "Error SignUp: ";
  cout << "Cleaning up job in db " << JobList2[ i ].GetJobId() << " server state: " << JobList2[ i ].GetStateFromServer() << "  : ";
  if( !JobList2[ i ].LockJob() ) cout << "Error 1 ";
  if( !JobList2[ i ].UpdateJob( "queued", "any", "mark", "somers", "no specs" ) ) cout << "Error 2 ";
  if( !JobList2[ i ].UpdateJobFromServer() ) cout << "Error 3 ";
  if( !JobList2[ i ].UnLockJob() ) cout << "Error 4 ";
  if( !JobList2[ i ].SignOff() ) cout << "Error signup ";
  JobList2[ i ].CleanUpJobDirectory(); cout << endl;
 }

}


int main_3( int *argc, char *argv[] )
{
 string Values = "mark , is     , ";
 vector<string> Array = CommaSeparatedValues2Array( Values, ',' );

 for( int i = 0; i < Array.size(); ++i )
  cout << "VALUE: '" << Array[ i ] << "'" << endl;

 cout << NormalizeCommaSeparatedValues( Values, ',' ) << endl;
}


int main_2( int *argc, char *argv[] )
{
 // InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING,"testmain.log");
 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING,"testmain.log");

 DaemonConfig Config( "LGI.cfg" );
 DaemonConfigProject MyProject;
 DaemonConfigProjectApplication MyApplication;

 if( !Config.IsValidConfigured() ) 
 {
  cout << "Not valid configured! Aborted!" << endl << endl;
  return( 1 );
 }
 
 cout << "CA_Certificate_File: " << Config.CA_Certificate_File() << endl;
 cout << "Resource_Name: " << Config.Resource_Name() << endl;
 cout << "Resource_URL: " << Config.Resource_URL() << endl;
 cout << "Run directory: " << Config.RunDirectory() << endl;
 cout << "Number of projects: " << Config.Number_Of_Projects() << endl;

 for( int i = 1; i <= Config.Number_Of_Projects(); ++i )
 {
  MyProject = Config.Project( i );
  cout << " - ProjectName of project " << i << " is : " << MyProject.Project_Name() << endl;
  cout << " - ProjectMasterServer of project " << i << " is : " << MyProject.Project_Master_Server() << endl;
  cout << " - Number of applications of project " << i << " is : " << MyProject.Number_Of_Applications() << endl;
  for( int j = 1; j <= MyProject.Number_Of_Applications(); ++j )
  {
   MyApplication = MyProject.Application( j );
   cout << "  -- ApplicationName of application " << j << " is : " << MyApplication.Application_Name() << endl;
   cout << "  -- OwnerAllow of application " << j << " is : " << MyApplication.Owner_Allow() << endl;
   cout << "  -- OwnerDeny of application " << j << " is : " << MyApplication.Owner_Deny() << endl;
   cout << "  -- CheckSystemLimitsScript of application " << j << " is : " << MyApplication.Check_System_Limits_Script() << endl;
   cout << "  -- JobCheckLimitsScript of application " << j << " is : " << MyApplication.Job_Check_Limits_Script() << endl;
   cout << "  -- JobCheckRunningScript of application " << j << " is : " << MyApplication.Job_Check_Running_Script() << endl;
   cout << "  -- JobCheckFinishedScript of application " << j << " is : " << MyApplication.Job_Check_Finished_Script() << endl;
   cout << "  -- JobPrologueScript of application " << j << " is : " << MyApplication.Job_Prologue_Script() << endl;
   cout << "  -- JobRunScript of application " << j << " is : " << MyApplication.Job_Run_Script() << endl;
   cout << "  -- JobEpilogueScript of application " << j << " is : " << MyApplication.Job_Epilogue_Script() << endl;
   cout << "  -- JobAbortScript of application " << j << " is : " << MyApplication.Job_Abort_Script() << endl;
  }
 }

}


int main_1( int *argc, char *argv[] )
{
 string Response, Response2;
 string Attributes;
 string Job_Id;
 int NrOfJobs, StartStop;

 // InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING,"testmain.log");
 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING,"testmain.log");

 string ServerURL = "https://fwnc7003.leidenuniv.nl/LGI";
 string Project = "LGI";
 Resource_Server_API Daemon( "../certificates/CLIENT.key", "../certificates/CLIENT_1.crt", "../certificates/LGI+CA.crt" );

 Daemon.Resource_SignUp_Resource( Response, ServerURL, Project );
 cout << "Resource_SignUp_Resource Response: " << Response << endl << endl;

 Daemon.Resource_Request_Work( Response, ServerURL, Project, "testapp" );
 cout << "Resource_Request_Work Response: " << Response << endl << endl;
 
 cout << " ------------------------------------------------------------ " << endl << endl;

 Response = Parse_XML( Parse_XML( Response, "LGI", Attributes ), "response", Attributes ); 
 NrOfJobs = atoi( NormalizeString( Parse_XML( Response, "number_of_jobs" ) ).c_str() );
 StartStop = 0;

 for( int i = 0; i < NrOfJobs; ++i )
 {
  Job_Id = NormalizeString( Parse_XML( Parse_XML( Response, "job", Attributes, StartStop ), "job_id", Attributes ) );

  Daemon.Resource_Request_Job_Details( Response2, ServerURL, Project, Job_Id );
  cout << "Resource_Request_Job_Details " << Job_Id << " Response: " << Response2 << endl << endl;

  Daemon.Resource_UnLock_Job( Response2, ServerURL, Project, Job_Id );
  cout << "Resource_UnLock_Job " << Job_Id << " Response: " << Response2 << endl << endl;
 }


 cout << " ------------------------------------------------------------ " << endl << endl;

 Daemon.Resource_Submit_Job( Response, ServerURL, Project, "testapp", "queued", "LGI", "any", "any", "specific", "0000010203040506070809", "0000010203040506070809" );
 cout << "Resource_Submit_Job Response: " << Response << endl << endl;

 Response = Parse_XML( Parse_XML( Response, "LGI", Attributes ), "response", Attributes );
 Job_Id = NormalizeString( Parse_XML( Parse_XML( Response, "job", Attributes ), "job_id", Attributes ) );

 cout << "job_id: '" << Job_Id  << "'" << endl << endl;

 Daemon.Resource_Request_Job_Details( Response, ServerURL, Project, Job_Id );
 cout << "Resource_Request_Job_Details Response: " << Response << endl << endl;

 Daemon.Resource_Update_Job( Response, ServerURL, Project, Job_Id, "", "LGI,any", "00", "00", "nothing specific" );
 cout << "Resource_Update_Job Response: " << Response << endl << endl;

 Daemon.Resource_Request_Job_Details( Response, ServerURL, Project, Job_Id );
 cout << "Resource_Request_Job_Details Response: " << Response << endl << endl;

 Daemon.Resource_UnLock_Job( Response, ServerURL, Project, Job_Id );
 cout << "Resource_UnLock_Job Response: " << Response << endl << endl;

 Daemon.Resource_SignOff_Resource( Response, ServerURL, Project );
 cout << "Resource_SignOff_Resource Response: " << Response << endl << endl;
}
