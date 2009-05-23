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

require_once( '../inc/Interfaces.inc' );
require_once( '../inc/Html.inc' );

// check session...
session_start();
$SID = $_SESSION[ "sid" ];
if( !isset( $_GET[ "sid" ] ) || ( $SID != $_GET[ "sid" ] ) || ( $_GET[ "sid" ] == "" ) ) Exit_With_Text( "ERROR: ".$ErrorMsgs[ 66 ] );

Page_Head();

?>
<style>
.errorMessage {
  color: red;
  font-weight: bold;
}
</style>
<script type="text/javascript" src="java_scripts/validation.js"></script>
<script type="text/javascript" src="java_scripts/upload.js"></script>
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

// check if groups is set in request... or default to data from cert...
$Groups = Interface_Get_Groups_From_Common_Name( SSL_Get_Common_Name() );
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
echo '<form id="this_form" action="basic_interface_submit_job_action.php" method="POST" enctype="multipart/form-data">';
echo '<input type="hidden" name="user" value="'.$User.'">';
echo '<input type="hidden" name="groups" value="'.$Groups.'">';
echo '<input type="hidden" name="project" value="'.$Project.'">';
echo '<input type="hidden" name="sid" value="'.$SID.'">';
echo '<input type="hidden" name="number_of_uploaded_files" value="0" >';

Start_Table();
Row1( "<center><font color='green' size='4'><b>Leiden Grid Infrastructure basic interface at ".gmdate( "j M Y G:i", time() )." UTC</b></font></center>" );
Row2( "<b>Project:</b>", htmlentities( $Project ) ); 
Row2( "<b>This project server:</b>", Get_Server_URL() ); 
Row2( "<b>Project master server:</b>", "<a href='".Get_Master_Server_URL()."/basic_interface/index.php?project=$Project&groups=$Groups&sid=$SID'>".Get_Master_Server_URL()."</a>" ); 
Row2( "<b>User:</b>", htmlentities( $User ) ); 
Row2( "<b>Groups:</b>", htmlentities( $Groups ) ); 
Row1( "<center><font color='green' size='4'><b>Specify job details</b></font></center>" );
Row2( "<b>Application:</b>", '<input type="text" size="75" name="application" value="hello_world" maxlength="128" validation="required"/>' );
Row2( "<b>Extra owners:</b>", '<input type="text" size="75" name="owners" value="" maxlength="128" />' );
Row2( "<b>Extra read access:</b>", '<input type="text" size="75" name="read_access" value="" maxlength="128" />' );
Row2( "<b>Target resources:</b>", '<input type="text" size="75" name="target_resources" value="any" maxlength="128" validation="required"/>' );
Row2( "<b>Job specifics:</b>", '<input type="text" size="75" name="job_specifics" value="" maxlength="1024" />' );
Row2( "<b>File(s) to upload:</b>", '<div id="file_upload_entry_1"> <input name="uploaded_file_1" type="file" onchange="FileUploadChanged()" size="75" /> </div>' );
Row2( "<b>Input:</b>", '<textarea wrap="off" rows="20" cols="75" name="input"></textarea>' );
Row1( '<center><input type="submit" value="     Submit Job     " /></center>' );

End_Table();

echo "</form><br><a href='basic_interface_list.php?project=$Project&groups=$Groups&project_server=1&sid=$SID'>Show project server list</a>\n";
echo "<br><a href='basic_interface_list.php?project=$Project&groups=$Groups&project_server=0&sid=$SID'>Show project resource list</a>\n"; 
echo "<br><a href='basic_interface_job_state.php?project=$Project&groups=$Groups&sid=$SID'>Show job list</a>\n"; 
echo "<br><a href='index.php?project=$Project&groups=$Groups&sid=$SID'>Go to main menu</a>\n";

Page_Tail();
?>

