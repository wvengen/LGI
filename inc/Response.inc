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

function LGI_Response( $DataTags, $ExtraInfoTags, $Exit = true )
{
 global $Config;

 $ExtraInfoTags = "<CA_certificate> ".$Config["SERVER_SSL_CA_CERTIFICATE_URL"]." </CA_certificate> ".$ExtraInfoTags;

 if( $Exit )
  exit( "<LGI> ".$ExtraInfoTags." <response> ".$DataTags." </response> </LGI>" ); 
 else
  return( 0 );
}

// -------------------------------------------------------------------- 

function LGI_Error_Response( $ErrorNo, $ErrorMessage, $ExtraInfoTags, $Exit = true )
{
 $ErrorTags = "<number> ".$ErrorNo." </number> <message> ".$ErrorMessage." </message> ";

 LGI_Response( "<error> ".$ErrorTags." </error>", $ExtraInfoTags, $Exit ); 

 return( $ErrorNo );
}

// --------------------------------------------------------------------

function LGI_BackOff_Response( $Exit = true )
{
 global $Config;
 global $ErrorMsgs;

 $ErrorMessage =  "<error> <number> 1 </number> <message> ".$ErrorMsgs[ 1 ]." </message>";
 $ErrorMessage .= " <timeout> ".$Config["LGI_TIMEOUT_VALUE"]." </timeout> </error> ";

 LGI_Response( $ErrorMessage, "", $Exit );

 return( 1 );
}

// -------------------------------------------------------------------- 

?>
