/* []--------------------------------------------------------[]
    |                      logger.cpp                        |
   []--------------------------------------------------------[]
    |                                                        |
    | ATHOR:      M.F.Somers                                 |
    | VERSION:    1.00, 10 July 2007.                        |
    | USE:        Implements a basic logging mechanism...    |
    |                                                        |
   []--------------------------------------------------------[]

*/

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

#include "logger.h" 

// ------------------------------------------------------------------------------

fstream LoggerStream( DEFAULT_LOGGER_FILE, fstream::out | fstream::app );
int     LoggerMode = NORMAL_LOGGING | CRITICAL_LOGGING;

// ------------------------------------------------------------------------------

void InitializeLogger( int Mode, char *FileName )
{
 LoggerMode = Mode;
 if( !LoggerStream.good() ) LoggerStream.clear();
 if( LoggerStream.is_open() ) LoggerStream.close();
 LoggerStream.open( FileName, fstream::out | fstream::app );
}

// ------------------------------------------------------------------------------

char *LoggerTime( void )
{
 char *TimeStr;
 time_t CurrentTime;

 time( &CurrentTime );
 TimeStr = ctime( &CurrentTime );   // get current time in string
 TimeStr[ 24 ] = '\0';              // remove the '\n'

 return( TimeStr );                 // and return pointer
}

// ------------------------------------------------------------------------------

