<?php

// []--------------------------------------------------------[]
//  |                      Error.inc                         |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        All error messages and numbers...          |
//  |                                                        |
// []--------------------------------------------------------[]
//
//
// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

global $ErrorMsgs;

$ErrorMsgs[ 0 ] = "No Error"; 
$ErrorMsgs[ 1 ] = "Back-off"; 
$ErrorMsgs[ 2 ] = "Invalid client-certificate"; 
$ErrorMsgs[ 3 ] = "Cannot connect to MySQL"; 
$ErrorMsgs[ 4 ] = "Cannot select database in MySQL"; 
$ErrorMsgs[ 5 ] = "Resource identifier not found in database"; 
$ErrorMsgs[ 6 ] = "Incorrect number of master project servers in the database"; 
$ErrorMsgs[ 7 ] = "Trying to remove stale job locks of a resource that has no valid session running"; 
$ErrorMsgs[ 8 ] = "Compulsory session_id field not posted"; 
$ErrorMsgs[ 9 ] = "Checking for job locks of a resource that has no valid session running"; 
$ErrorMsgs[ 10 ] = "Trying to unlock jobs of a resource that has no valid session running"; 
$ErrorMsgs[ 11 ] = "Trying to lock jobs with a resource that has no valid session running"; 
$ErrorMsgs[ 12 ] = "Multiple locks were found in the database for the specific job"; 
$ErrorMsgs[ 13 ] = "No locks were found in the database for the specific job"; 
$ErrorMsgs[ 14 ] = "Lock was not set by current resource in current session"; 
$ErrorMsgs[ 15 ] = "Job was not found in the database"; 
$ErrorMsgs[ 16 ] = "Resource has no valid session running"; 
$ErrorMsgs[ 17 ] = "Resource still has locks on jobs in database"; 
$ErrorMsgs[ 18 ] = "Compulsory application field was not posted"; 
$ErrorMsgs[ 19 ] = "Compulsory job_id field was not posted"; 
$ErrorMsgs[ 20 ] = "Job was not locked by this resource"; 
$ErrorMsgs[ 21 ] = "Job may not be locked by this resource"; 
$ErrorMsgs[ 22 ] = "Job could not be locked"; 
$ErrorMsgs[ 23 ] = "Job could not be unlocked"; 
$ErrorMsgs[ 24 ] = "Resource is not allowed to get job state"; 
$ErrorMsgs[ 25 ] = "Compulsory state field was not posted"; 
$ErrorMsgs[ 26 ] = "Compulsory owners field was not posted"; 
$ErrorMsgs[ 27 ] = "Compulsory target_resources field was not posted"; 
$ErrorMsgs[ 28 ] = "None of the target_resources were found in the database"; 
$ErrorMsgs[ 29 ] = "None of the owners is allowed to submit a job"; 
$ErrorMsgs[ 30 ] = "Posted user field does not match the client-certificate"; 
$ErrorMsgs[ 31 ] = "Compulsory groups field was not posted"; 
$ErrorMsgs[ 32 ] = "Compulsory user field was not posted"; 
$ErrorMsgs[ 33 ] = "This user is not allowed to submit a job for any of the groups"; 
$ErrorMsgs[ 34 ] = "This user is not allowed to read this job for any of the groups"; 
$ErrorMsgs[ 35 ] = "This user is not allowed to modify this job for any of the groups"; 
$ErrorMsgs[ 36 ] = "Job was not found in the database or was not spin-locked"; 
$ErrorMsgs[ 37 ] = "The certificate does not allow for this project"; 
$ErrorMsgs[ 38 ] = "The certificate does not allow for any of the posted groups"; 
$ErrorMsgs[ 39 ] = "Resource is not listed as a project-server in the database"; 
$ErrorMsgs[ 40 ] = "Compulsory version field was not posted"; 
$ErrorMsgs[ 41 ] = "Update fields not correctly posted"; 
$ErrorMsgs[ 42 ] = "Trying to kill a non-existing session of a resource"; 
$ErrorMsgs[ 43 ] = "Compulsory resource_name field was not posted"; 
$ErrorMsgs[ 44 ] = "Could not setup a new session with resource"; 
$ErrorMsgs[ 45 ] = "State field posted too big"; 
$ErrorMsgs[ 46 ] = "Application field posted too big"; 
$ErrorMsgs[ 47 ] = "Job_id field posted too big"; 
$ErrorMsgs[ 48 ] = "Start field posted too big"; 
$ErrorMsgs[ 49 ] = "Limit field posted too big"; 
$ErrorMsgs[ 50 ] = "Target_resources field posted too big"; 
$ErrorMsgs[ 51 ] = "Read_access field posted too big"; 
$ErrorMsgs[ 52 ] = "Owners field posted too big"; 
$ErrorMsgs[ 53 ] = "Job_specifics field posted too big"; 
$ErrorMsgs[ 54 ] = "Input field posted too big"; 
$ErrorMsgs[ 55 ] = "Output field posted too big"; 
$ErrorMsgs[ 56 ] = "Groups field posted too big"; 
$ErrorMsgs[ 57 ] = "User field posted too big"; 
$ErrorMsgs[ 58 ] = "Project field posted too big"; 
$ErrorMsgs[ 59 ] = "Version field posted too big"; 
$ErrorMsgs[ 60 ] = "Servers field posted too big"; 
$ErrorMsgs[ 61 ] = "Update field posted too big"; 
$ErrorMsgs[ 62 ] = "Session_id field posted too big"; 
$ErrorMsgs[ 63 ] = "Resource_name field posted too big"; 
$ErrorMsgs[ 64 ] = "Uploaded file size too big for current server settings"; 
$ErrorMsgs[ 65 ] = "Total POST size too big for current server settings"; 
$ErrorMsgs[ 66 ] = "Suspected cross-site-request-forgery (CSRF) attack"; 
$ErrorMsgs[ 67 ] = "Suspected cross-site-scripting (XSS) attack"; 
$ErrorMsgs[ 68 ] = "Error in file upload to repository"; 

?>
