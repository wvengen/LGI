<?php

// []--------------------------------------------------------[]
//  |                      Response.inc                      |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, August 2007.                         |
//  | USE:        Generate LGI responses...                  |
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

require_once( '../inc/Config.inc' );
require_once( '../inc/Errors.inc' );

// -------------------------------------------------------------------- 

function LGI_Response( $DataTags, $Exit = true, $ExtraInfoTags = "" )
{
 global $Config;

 $ExtraInfoTags = "<server_max_field_size> ".$Config["MAX_FIELD_SIZE"]." </server_max_field_size> ".$ExtraInfoTags;
 $ExtraInfoTags = "<CA_certificate> ".$Config["SERVER_SSL_CA_CERTIFICATE_URL"]." </CA_certificate> ".$ExtraInfoTags;
 $Response = "<LGI> ".$ExtraInfoTags." <response> ".$DataTags." </response> </LGI>";

 if( $Exit )
  exit( $Response ); 
 else
  return( $Response );
}

// -------------------------------------------------------------------- 

function LGI_Error_Response( $ErrorNo, $ErrorMessage, $Exit = true, $ExtraInfoTags = "" )
{
 $ErrorTags = "<error> <number> ".$ErrorNo." </number> <message> ".$ErrorMessage." </message> </error>";

 LGI_Response( $ErrorTags, $Exit, $ExtraInfoTags ); 

 return( $ErrorNo );
}

// --------------------------------------------------------------------

function LGI_BackOff_Response( $Exit = true )
{
 global $Config;
 global $ErrorMsgs;

 $ErrorMessage =  "<error> <number> 1 </number> <message> ".$ErrorMsgs[ 1 ]." </message>  <timeout> ".$Config["LGI_TIMEOUT_VALUE"]." </timeout> </error> ";

 LGI_Response( $ErrorMessage, $Exit );

 return( 1 );
}

// -------------------------------------------------------------------- 

?>
