<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

global $Config;

$Config[ "SERVER_URL" ]            = "https://fwnc7003.leidenuniv.nl/LGI";     // the URL of this project server
$Config[ "SERVER_NAME" ]           = "mark@fwnc7003.leidenuniv.nl";            // the unique name of this project server

$Config[ "SERVER_SSL_CERTIFICATE_FILE" ]     = "../certificates/fwnc7003.crt";      // the certificate of this server
$Config[ "SERVER_SSL_KEY" ]                  = "../certificates/fwnc7003.key";      // the certificate key file of this server
$Config[ "SERVER_SSL_CA_CERTIFICATE_URL" ]   = "https://fwnc7003.leidenuniv.nl/LGI/LGI+CA.crt";   // the certificate of the CA server
$Config[ "SERVER_SSL_CA_CERTIFICATE_FILE" ]  = "../certificates/LGI+CA.crt";           // the local certificate of the CA server

$Config[ "MYSQL_URL" ]                   = "localhost";            // where is the MySQL db and how to connect to it
$Config[ "MYSQL_USER" ]                  = "LGI"; 
$Config[ "MYSQL_PASSWD" ]                = "LGIpasswd";
$Config[ "MYSQL_DEFAULT_DATABASE" ]      = "LGI";                  // what DB to use by default

$Config[ "LGI_TIMEOUT_VALUE" ]           = 10;                     // default timeout in seconds for back-off
$Config[ "LGI_SPIN_LOCK_ATTEMPTS" ]      = 30;                     // number of attempt to get a spin-lock before failing for interfaces
$Config[ "LGI_INACTIVITY_VALUE" ]        = 3600;                   // default timeout in seconds before resource is considered inactive

$Config[ "DEFAULT_WORK_REQUEST_LIMIT" ]  = 10;                     // the default request limit for resources
$Config[ "DEFAULT_JOB_STATUS_LIMIT" ]    = 20;                     // the default request limit for interfaces

?>
