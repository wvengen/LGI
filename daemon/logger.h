/* []--------------------------------------------------------[]
    |                      logger.h                          |
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

#if !defined(__LOGGER_INCLUDED__)
#define __LOGGER_INCLUDED__

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

// ------------------------------------------------------------------------------

#define DEFAULT_LOGGER_FILE "/dev/stdout"

#define CRITICAL_LOGGING 1
#define NORMAL_LOGGING   2
#define DEBUG_LOGGING    4

#define LOGGER(tag)    ( LoggerStream << LoggerTime() << " [" << tag << "] " << __FILE__ << ":" << __LINE__ << " :: " )

#define DEBUG_LOG( M )    { if( LoggerMode & DEBUG_LOGGING ) { LOGGER( "DEBUG" ) << M << endl; } }
#define NORMAL_LOG( M )   { if( LoggerMode & NORMAL_LOGGING ) { LOGGER( "NORMAL" ) << M << endl; } }
#define CRITICAL_LOG( M ) { if( LoggerMode & CRITICAL_LOGGING ) { LOGGER( "CRITICAL" ) << M << endl; } }

#define DEBUG_LOG_RETURN( X, M )    { if( LoggerMode & DEBUG_LOGGING ) { LOGGER( "DEBUG" ) << M << endl; } return( X ); }
#define NORMAL_LOG_RETURN( X, M )   { if( LoggerMode & NORMAL_LOGGING ) { LOGGER( "NORMAL" ) << M << endl; } return( X ); }
#define CRITICAL_LOG_RETURN( X, M ) { if( LoggerMode & CRITICAL_LOGGING ) { LOGGER( "CRITICAL" ) << M << endl; } return( X ); }

// ------------------------------------------------------------------------------

extern fstream LoggerStream;
extern int     LoggerMode;

// ------------------------------------------------------------------------------

void InitializeLogger( int Mode = NORMAL_LOGGING | CRITICAL_LOGGING, char *FileName = DEFAULT_LOGGER_FILE );
char *LoggerTime( void );

// ------------------------------------------------------------------------------

#endif
