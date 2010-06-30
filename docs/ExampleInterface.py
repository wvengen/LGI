#!/usr/bin/python
#
# This is a simple test code to demonstrate the use of the LGI interface class. All 
# methods are being used and briefly explained. Methods that return a response of
# the project server, return a dictionary with the XML content of the response. This
# allows for easy access to parameters, as shown below. Tags transfered in binhex
# format within LGI (input and output) are automatically converted. File uploads and
# downloads from a repository are transfered streaming and the LGI_Client class can be
# be setup to run as an interface client or as a repository client as shown below.

import LGI;

# create a client instance with defaults read in from "~/.LGI"
Client = LGI.LGI_Client();

# submit a job with some input for the hello_world application at the same time upload a file
JOB = Client.SubmitJob( Application = "hello_world", Input = "haydihay", FileList = [ "/etc/resolv.conf" ] );

# extract job_id from the response dictionary
job_id = JOB[ 'LGI' ][ 'response' ][ 'job' ][ 'job_id' ];

# extract the repository url from the response dictionary
repository_url = JOB[ 'LGI' ][ 'response' ][ 'job' ][ 'job_specifics' ][ 'repository_url' ];

# setup an instance to access the repository
FileClient = LGI.LGI_Client( URL = repository_url );

# print some stuff and run some tests
print JOB;
print
print Client.GetJobList( State = '!finished' );
print
print Client.GetJobState( job_id );
print
print Client.GetResourceList();
print
print Client.GetServerList();
print

# now play with the repository for a bit
print FileClient.ListRepository();
FileClient.DownLoadFiles( [ "resolv.conf" ], "/tmp" );
FileClient.UpLoadFiles( [ "/etc/hosts", "/etc/fstab" ] );
print
print FileClient.ListRepository();
FileClient.DeleteFiles( [ "hosts" ] );
print
print FileClient.ListRepository();
print

# finally just delete the job to clean up
print Client.DeleteJob( job_id );


