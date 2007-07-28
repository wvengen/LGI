<?php

 echo "Hello world from test_client_certificate.php!\n\n";

if( $_POST['field'] )
 {
  $field=$_POST['field'];
  $data=$GLOBALS[ 'HTTP_SERVER_VARS' ][ $field ];
  echo "A post was done for field '".$field."'\n";
  echo "Result: '".$data."'\n";
  echo "\n\n\n";
 }
 else
  var_dump( $GLOBALS[ 'HTTP_SERVER_VARS' ] );

?>
