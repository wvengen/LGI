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
 RunDirectory.empty();
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheRunDirectory )
{
}

// -----------------------------------------------------------------------------

DaemonJob::DaemonJob( string TheXML, DaemonConfigProjectApplication TheApplicationConfig )
{
}

// -----------------------------------------------------------------------------

string DaemonJob::ReadStringFromFile( string FileName )
{
 string Buffer;

 return( Buffer );
}

// -----------------------------------------------------------------------------
