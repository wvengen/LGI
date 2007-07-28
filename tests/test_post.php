<?php

 echo "Hello world from test_post.php!\n\n";

 $ClientCertificate = "../certificates/CLIENT_1.crt";
 $ClientKey = "../certificates/CLIENT.key";
 $CACertificate = "../certificates/LGI+CA.crt";

 $ch = curl_init("https://fwnc7003.leidenuniv.nl/LGI/tests/test_client_certificate.php");

 curl_setopt( $ch, CURLOPT_SSLCERT, $ClientCertificate );
 curl_setopt( $ch, CURLOPT_SSLKEY, $ClientKey );
 curl_setopt( $ch, CURLOPT_CAINFO, $CACertificate );
 curl_setopt( $ch, CURLOPT_SSL_VERIFYPEER, 1 );
 curl_setopt( $ch, CURLOPT_SSL_VERIFYHOST, 1 );
 curl_setopt( $ch, CURLOPT_POSTFIELDS, "field=SSL_CLIENT_S_DN_CN" );
 curl_setopt( $ch, CURLOPT_RETURNTRANSFER, 1 );

 $data = curl_exec( $ch );
 curl_close( $ch );

 echo $data."\n\n";
?>
