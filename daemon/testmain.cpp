#include <cstdlib>
#include "logger.h"
#include "resource_server_api.h"
#include "xml.h"
#include "binhex.h"
#include "config.h"

int main( int *argc, char *argv[] )
{
 // InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING|DEBUG_LOGGING,"testmain.log");
 InitializeLogger(CRITICAL_LOGGING|NORMAL_LOGGING,"testmain.log");

 DaemonConfig Config( "LGI.cfg" );
 DaemonConfigProject MyProject;
 DaemonConfigProjectApplication MyApplication;

 cout << "CA_Certificate_File: " << Config.CA_Certificate_File() << endl;
 cout << "Resource_Name: " << Config.Resource_Name() << endl;
 cout << "Resource_URL: " << Config.Resource_URL() << endl;
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
   cout << "  -- JobPrologueScript of application " << j << " is : " << MyApplication.Job_Prologue_Script() << endl;
   cout << "  -- JobRunScript of application " << j << " is : " << MyApplication.Job_Run_Script() << endl;
   cout << "  -- JobEpilogueScript of application " << j << " is : " << MyApplication.Job_Epilogue_Script() << endl;
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
