#!/usr/bin/perl

# A simple log file, must be writable by the user that this program runs as...
$deletelog = "delete.log";

# Set repository access file name...
$accessfile = ".LGI_repository_write_access";

# Set resource list file name...
$resourcesfile = ".LGI_resource_list";

# Check we are using DELETE method...
if ($ENV{'REQUEST_METHOD'} ne "DELETE") { &reply(500, "Request method is not DELETE"); }

# Check use of SSL client certificate...
if ($ENV{'SSL_CLIENT_VERIFY'} ne "SUCCESS") { &reply(500, "Client certificate failed"); }

# Check we got a destination filename...
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

# Do access checks if needed...
if ($accessfile || $resourcesfile) {

 # Default deny access...
 $allowed = 0;

 # Get certificate common name fields...
 @certificate = &CSV2Array($ENV{'SSL_CLIENT_S_DN_CN'}, ";");
 if($#certificate >= 3) {
  @certificate = &CSV2Array( $certificate[ 0 ].", ".$certificate[ 1 ], ",");
 }
 else {
  @certificate = ( $certificate[ 0 ] );
 }

 # If file specified, check write access of repository...
 if ($accessfile) {

  # Get write access fields from file...
  ($dir,$file) = $filename =~ m|^(.*[/\\])([^/\\]+?)$|;
  $accessfile = $dir.$accessfile;
  open(ACCESS, $accessfile) || &reply(500, "Cannot open $accessfile file");
  $access = <ACCESS> || &reply(500, "Cannot read from $accessfile file");
  close(ACCESS);
  @access = &CSV2Array($access, ",");

  # Now check if allowed combination is found...
  foreach $a (@access) {
   chomp($a);
   foreach $c (@certificate) {
    chomp($c);
    if($a =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^$a$/i) { $allowed = 1; }
   }
  }
 }

 # If file specified, check resources of project...
 if ($resourcesfile) {

  # Get fields from file...
  open(RESOURCES, $resourcesfile) || &reply(500, "Cannot open $resourcesfile file");
  $resources = <RESOURCES> || &reply(500, "Cannot read from $resourcesfile file");
  close(RESOURCES);
  @resources = &CSV2Array($resources, ",");

  # Now check if allowed combination is found...
  foreach $r (@resources) {
   chomp($r);
   foreach $c (@certificate) {
    chomp($c);
    if($r =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^$r$/i) { $allowed = 1; }
   }
  }
 }

 # Check if we have access...
 if(!$allowed) { &reply(500, "Access to repository is denied"); }
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
 print "Status: $status $message\n";
 print "Content-Type: text/html\n\n";

 # Response...
 print "<status> <number> $status </number> <message> $message </message> </status>\n";

 # Create a simple log entry...
 $remuser = $ENV{'REMOTE_USER'} || "-";
 $remhost = $ENV{'REMOTE_HOST'} || $ENV{'REMOTE_ADDR'} || "-";
    
 $logline = "$remhost $remuser status $status ($message)";
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
 

