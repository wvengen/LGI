#!/usr/bin/perl

# A simple log file, must be writable by the user that this program runs as...
$putlog = "put.log";

# Set block size for file IO...
$blocksize = 16384;

# Set repository owners file name...
$ownersfile = ".LGI_repository_owners";

# Set resource list file name...
$resourcesfile = ".LGI_resource_list";

# Check we are using PUT method...
if ($ENV{'REQUEST_METHOD'} ne "PUT") { &reply(500, "Request method is not PUT"); }

# Check use of SSL client certificate...
if ($ENV{'SSL_CLIENT_VERIFY'} ne "SUCCESS") { &reply(500, "Client certificate failed"); }

# Check we got a destination filename...
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

# Check we got some content...
$clength = $ENV{'CONTENT_LENGTH'};
if (!$clength) { &reply(500, "Content-Length missing or zero ($clength bytes)"); }

# Do access checks if needed...
if ($ownersfile || $resourcesfile) {

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

 # If file specified, check owners of repository...
 if ($ownersfile) {

  # Get owners fields from file...
  ($dir,$file) = $filename =~ m|^(.*[/\\])([^/\\]+?)$|;
  $ownersfile = $dir.$ownersfile;
  open(OWNERS, $ownersfile) || &reply(500, "Cannot open $ownersfile file");
  $owners = <OWNERS> || &reply(500, "Cannot read from $ownersfile file");
  close(OWNERS);
  @owners = &CSV2Array($owners, ",");

  # Now check if allowed combination is found...
  foreach $o (@owners) {
   chomp($o);
   foreach $c (@certificate) {
    chomp($c);
    if($o =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^any$/i) { $allowed = 1; }
    if($c =~ /^$o$/i) { $allowed = 1; }
   }
  }
 }

 # If file specified, check resources of project...
 if ($resourcesfile) {

  # Get owners fields from file...
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
 if(!$allowed) { &reply(500, "Access to repository is denied, you are no owner or resource"); }
}

# Open output file...
open(OUT, "> $filename") || &reply(500, "Cannot write to $filename");

# Read the content itself from stdin...
$toread = $clength;
while ($toread > 0) {
    $nread = read(STDIN, $data, &min($toread, $blocksize) );
    &reply(500, "Error reading content") if !defined($nread);
    $toread -= $nread;
    print OUT $data;
}

# Close output file...
close(OUT);

# Reply all OK...
&reply(200, "Uploaded $filename ($clength bytes)");

exit(0);

#
# Get minimum...
#
sub min
{
 if($_[0] > $_[1]) {
  $_[0];
 }
 else {
  $_[1];
 }
}

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
    
 $logline = "$remhost $remuser $filename status $status ($message)";
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
 open (LOG, ">> $putlog") || return;
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

