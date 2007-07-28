<?php

// Copyright (C) 2007 M.F. Somers, Theoretical Chemistry Group, Leiden University
//
// This is free software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software Foundation.
//
// http://www.gnu.org/licenses/gpl.txt

require_once( '../inc/Config.inc' );
require_once( '../inc/Errors.inc' );

// -------------------------------------------------------------------- 

function LGI_Response( $DataTags, $ExtraInfoTags )
{
 global $Config;

 $ExtraInfoTags = "<CA_certificate> ".$Config["SERVER_SSL_CA_CERTIFICATE_URL"]." </CA_certificate> ".$ExtraInfoTags;

 exit( "<LGI> ".$ExtraInfoTags." <response> ".$DataTags." </response> </LGI>" ); 

 return( 0 );
}

// -------------------------------------------------------------------- 

function LGI_Error_Response( $ErrorNo, $ErrorMessage, $ExtraInfoTags )
{
 $ErrorTags = "<number> ".$ErrorNo." </number> <message> ".$ErrorMessage." </message> ";

 LGI_Response( "<error> ".$ErrorTags." </error>", $ExtraInfoTags ); 

 return( $ErrorNo );
}

// --------------------------------------------------------------------

function LGI_BackOff_Response( )
{
 global $Config;
 global $ErrorMsgs;

 $ErrorMessage =  "<error> <number> 1 </number> <message> ".$ErrorMsgs[ 1 ]." </message>";
 $ErrorMessage .= " <timeout> ".$Config["LGI_TIMEOUT_VALUE"]." </timeout> </error> ";

 LGI_Response( $ErrorMessage, "" );

 return( 1 );
}

// -------------------------------------------------------------------- 

?>
