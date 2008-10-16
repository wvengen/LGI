#!/usr/bin/perl

# A simple log file, must be writable by the user that this program runs as...
$deletelog = "delete.log";

# Set repository owners file name...
$ownerfile = ".LGI_repository_owners";
$ownerfile = ""; 

# Check we are using DELETE method...
if ($ENV{'REQUEST_METHOD'} ne "DELETE") { &reply(500, "Request method is not DELETE"); }

# Check use of SSL client certificate...
if ($ENV{'SSL_CLIENT_VERIFY'} ne "SUCCESS") { &reply(500, "Client certificate failed"); }

# Check we got a destination filename...
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

# If file specified, check owners of repository...
if ($ownerfile) {

 # Get owners fields from file...
 ($dir,$file) = $filename =~ m|^(.*[/\\])([^/\\]+?)$|;
 $ownersfile = $dir.$ownerfile;
 open(OWNERS, $ownersfile) || &reply(500, "Cannot open $ownersfile file");
 $owners = <OWNERS> || &reply(500, "Cannot read from $ownersfile file");
 close(OWNERS);
 @owners = &CSV2Array($owners, ",");

 # Get certificate common name fields...
 @certificate = &CSV2Array($ENV{'SSL_CLIENT_S_DN_CN'}, ";");
 if($#certificate >= 3) {
  @certificate = &CSV2Array( $certificate[ 0 ].", ".$certificate[ 1 ], ",");
 }
 else {
  @certificate = ( $certificate[ 0 ] );
 }

 # Now check of allowed combination is found...
 $allowed = 0;
 foreach $o (@owners) {
  chomp($o);
  foreach $c (@certificate) {
   chomp($c);
   if($o =~ /^any$/i) { $allowed = 1; }
   if($c =~ /^any$/i) { $allowed = 1; }
   if($c =~ /^$o$/i) { $allowed = 1; }
  }
 }

 if(!$allowed) { &reply(500, "Access to repository is denied, you are no owner"); }
}

# Delete the file if possible...
if( !unlink($filename) ) { &reply(500, "Error deleting $filename"); }

# Reply all okay...
&reply(200, "Deleted $filename");

exit(0);

#
# Send back reply to client for a given status...
#
sub reply
{
 local($status, $message) = @_;
 local($remuser, $remhost, $logline) = ();

 # Create header...
 print "Status: $status\n";
 print "Content-Type: text/html\n\n";

 # Response...
 print "<status> <number> $status </number> <message> $message </message> </status>\n";

 # Create a simple log entry...
 $remuser = $ENV{'REMOTE_USER'} || "-";
 $remhost = $ENV{'REMOTE_HOST'} || $ENV{'REMOTE_ADDR'} || "-";
    
 $logline = "$remhost $remuser $filename status $status";
 $logline .= " ($message)" if ($status == 500);
 &log($logline);
 exit(0);
}

#
# Log messages...
#
sub log
{
 local($msg) = @_;
 local($stamp) = &timestamp();
 open (LOG, ">> $deletelog") || return;
 print LOG "[$stamp] $msg\n";
 close(LOG);
}

#
# Timestamps...
#
sub timestamp
{
 local($y, $m, $d, $ss, $mm, $hh) = (localtime())[5,4,3,0,1,2];
 $y += 1900;
 $m += 1;
 sprintf("%d/%02d/%02d %02d:%02d:%02d", $d, $m, $y, $hh, $mm, $ss);
}

#
# CSV2Array...
#
sub CSV2Array
{
 local($CSV, $S) = @_;
 split(/\s*$S\s*/, $CSV)
}
 

