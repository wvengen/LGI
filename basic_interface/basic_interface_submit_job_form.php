<?php

// []--------------------------------------------------------[]
//  |           basic_interface_submit_job_form.php          |
// []--------------------------------------------------------[]
//  |                                                        |
//  | AUTHOR:     M.F.Somers                                 |
//  | VERSION:    1.00, October 2007.                        |
//  | USE:        Generate a form to submit a job...         |
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
require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Utils.inc' );
require_once( '../inc/Html.inc' );

global $Config;
global $ErrorMsgs;

Page_Head();

?>
<style>
.errorMessage {
  color: red;
  font-weight: bold;
}
</style>
<script type="text/javascript" src="java_scripts/validation.js"></script>
<?php

// check if user is set in request... or use value from certificate...
$CommonNameArray = CommaSeparatedField2Array( SSL_Get_Common_Name(), ";" );
$User = $CommonNameArray[ 1 ];
if( isset( $_GET[ "user" ] ) )
 $User = $_GET[ "user" ];
else
 if( isset( $_POST[ "user" ] ) )
  $User = $_POST[ "user" ];
if( strlen( $User ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 57 ] );
$User = mysql_escape_string( $User );

// check if groups is set in request... or default to user's group...
$Groups = $User;
if( isset( $_GET[ "groups" ] ) )
 $Groups = $_GET[ "groups" ];
else
 if( isset( $_POST[ "groups" ] ) )
  $Groups = $_POST[ "groups" ];
if( strlen( $Groups ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 56 ] );
$Groups = mysql_escape_string( $Groups );

// check if project is set in request... or default to value set in config...
$Project = $Config[ "MYSQL_DEFAULT_DATABASE" ];
if( isset( $_GET[ "project" ] ) )
 $Project = $_GET[ "project" ];
else
 if( isset( $_POST[ "project" ] ) )
  $Project = $_POST[ "project" ];
if( strlen( $Project ) >= $Config[ "MAX_POST_SIZE_FOR_TINYTEXT" ] ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 58 ] );
$Project = mysql_escape_string( $Project );

// now verfiy the user using the basic browser interface... also make MySQL connection...
$ErrorCode = Interface_Verify( $Project, $User, $Groups, false );
if( $ErrorCode !== 0 ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ $ErrorCode ] );

// start building form to fill in...
$RepositoryName = "JOB_".md5(uniqid(time()));
echo '<form id="this_form" action="basic_interface_submit_job_action.php" method="POST" enctype="multipart/form-data">';
echo '<input type="hidden" name="user" value="'.$User.'">';
echo '<input type="hidden" name="groups" value="'.$Groups.'">';
echo '<input type="hidden" name="repository" value="'.$RepositoryName.'">';

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</b></font></center>" );
Row2( "<b>Project:</b>", $Project ); 
Row2( "<b>This project server:</b>", Get_Server_URL() ); 
Row2( "<b>Project master server:</b>", "<a href=".Get_Master_Server_URL()."/basic_interface>".Get_Master_Server_URL()."</a>" ); 
Row2( "<b>User:</b>", $User ); 
Row2( "<b>Groups:</b>", $Groups ); 
Row1( "<center><font color='green' size='4'><b>Specify job details</b></font></center>" );
Row2( "<b>Job repository:</b>", $RepositoryName );
Row2( "<b>Application:</b>", '<input type="text" size="75" name="application" value="hello_world" maxlength="128" validation="required"/>' );
Row2( "<b>Extra owners:</b>", '<input type="text" size="75" name="owners" value="" maxlength="128" />' );
Row2( "<b>Extra read access:</b>", '<input type="text" size="75" name="read_access" value="" maxlength="128" />' );
Row2( "<b>Target resources:</b>", '<input type="text" size="75" name="target_resources" value="any" maxlength="128" validation="required"/>' );
Row2( "<b>Job specifics:</b>", '<input type="text" size="75" name="job_specifics" value="" maxlength="1024" />' );

?>
<script type="text/javascript">

function FileUploadChanged()
{
 var this_form = document.getElementById( 'this_form' );
 var usedFields = 0;
 var fields = new Array();

 for( var i = 0; i < this_form.elements.length; i++ )
  if( ( this_form.elements[ i ].type == 'file'  ) && ( this_form.elements[ i ].name.substr( 0, 14 ) == 'uploaded_file_' ) )
   fields[ fields.length ] = this_form.elements[ i ];

 for( var i = 0; i < fields.length; i++ )
  if( fields[ i ].value.length > 0 ) usedFields++;

 if( usedFields == fields.length )
 {
  var lastEntry = document.getElementById( 'file_upload_entry_' + usedFields );

  if( lastEntry )
  {
   var newEntry = document.createElement( 'div' );
   newEntry.id = 'file_upload_entry_' + ( usedFields + 1 );

   var file = document.createElement( 'input' );
   file.type = 'file';
   file.name = 'uploaded_file_' + ( usedFields + 1 );
   file.onchange = function() { FileUploadChanged(); };
   file.size = 75;

   newEntry.appendChild( file );
   lastEntry.parentNode.insertBefore( newEntry, lastEntry.nextSibling );
  }
 }
}

</script>
<?php

Row2( "<b>File(s) to upload:</b>", '<div id="file_upload_entry_1"> <input name="uploaded_file_1" type="file" onchange="FileUploadChanged()" size="75" /> </div>' );
Row2( "<b>Input:</b>", '<textarea wrap="off" rows="20" cols="75" name="input"></textarea>' );
Row1( '<center><input type="submit" value="     Submit Job     " /></center></form>' );

End_Table();

echo "<br><a href=basic_interface_list.php?project_server=1>Show project server list</a>\n";
echo "<br><a href=basic_interface_list.php?project_server=0>Show project resource list</a>\n"; 
echo "<br><a href=basic_interface_job_state.php>Show job list</a>\n"; 
echo "<br><a href=index.php>Go to main menu</a>\n";

Page_Tail();
?>

