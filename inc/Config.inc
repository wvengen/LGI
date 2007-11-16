<?php

// []--------------------------------------------------------[]
//  |                      Config.inc                        |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Configuration of server API...             |
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

global $Config;

$Config[ "SERVER_URL" ]            = "https://fwnc7003.leidenuniv.nl/LGI";     // the URL of this project server
$Config[ "SERVER_NAME" ]           = "LGI@fwnc7003.wks.gorlaeus.net";          // the unique name of this project server

$Config[ "SERVER_SSL_CERTIFICATE_FILE" ]     = "../certificates/LGI@fwnc7003.crt";      // the certificate of this server
$Config[ "SERVER_SSL_KEY" ]                  = "../certificates/LGI@fwnc7003.key";      // the certificate key file of this server
$Config[ "SERVER_SSL_CA_CERTIFICATE_URL" ]   = "https://fwnc7003.leidenuniv.nl/LGI/LGI+CA.crt";   // the certificate of the CA server
$Config[ "SERVER_SSL_CA_CERTIFICATE_FILE" ]  = "../certificates/LGI+CA.crt";           // the local certificate of the CA server

$Config[ "REPOSITORY_DIRECTORY" ] = "";                 // location on filesystem of basic interface repository
$Config[ "REPOSITORY_SERVER_NAME" ] = "";               // on what server and user the basic interface repository is 
$Config[ "REPOSITORY_URL" ] = "";                       // location on web of basic interface repository
$Config[ "REPOSITORY_SSH_IDENTITY_FILE" ] = "";         // the identity file to use to ssh RSA authorization to handle basic interface repository
$Config[ "REPOSITORY_SSH_COMMAND" ] = "/usr/bin/ssh";   // the default location of the ssh command

$Config[ "MYSQL_URL" ]                   = "localhost";            // where is the MySQL db and how to connect to it
$Config[ "MYSQL_USER" ]                  = "LGI"; 
$Config[ "MYSQL_PASSWD" ]                = "LGIpasswd";
$Config[ "MYSQL_DEFAULT_DATABASE" ]      = "LGI";                  // what DB to use by default

$Config[ "MAX_POST_SIZE_FOR_BLOB" ]      = 65355;                  // max post size for blob and text fields...
$Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ]  = 255;                    // max post size for tinytext fields...
$Config[ "MAX_POST_SIZE_FOR_INTEGER" ]   = 10;                     // max post size for integer fields...
$Config[ "MAX_FIELD_SIZE" ]              = $Config[ "MAX_POST_SIZE_FOR_BLOB" ];

$Config[ "LGI_TIMEOUT_VALUE" ]           = 10;                     // default timeout in seconds for back-off
$Config[ "LGI_SPIN_LOCK_ATTEMPTS" ]      = 30;                     // number of attempt to get a spin-lock before failing for interfaces
$Config[ "LGI_INACTIVITY_VALUE" ]        = 1800;                   // default timeout in seconds before resource session is considered inactive

$Config[ "DEFAULT_WORK_REQUEST_LIMIT" ]  = 10;                     // the default request limit for resources
$Config[ "DEFAULT_JOB_STATUS_LIMIT" ]    = 20;                     // the default request limit for interfaces

?>
