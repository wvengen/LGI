<?php

  if( isset( $_POST[ "repository" ] ) )
   $Respository = $_POST[ "repository" ];

  if( isset( $_GET[ "repository" ] ) )
   $Repository = $_GET[ "repository" ];

  if( isset( $Repository ) && is_dir( escapeshellcmd( $Repository ) ) )
  {
   $Find = "find ./".escapeshellcmd( $Repository )." -type f -printf '<file name=\"%P\"> <size> %s </size> <date> %T@ </date> </file> '";
   $Content = shell_exec( $Find ); 
  }
  else
   $Content = "";

  echo "<repository_content name=\"$Repository\"> $Content </repository_content>";
?>
