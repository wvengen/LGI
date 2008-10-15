#!/usr/bin/perl

# A simple log file, must be writable by the user that this program runs as...
$deletelog = "delete.log";

# Check we are using DELETE method...
if ($ENV{'REQUEST_METHOD'} ne "DELETE") { &reply(500, "Request method is not DELETE"); }

# Check we got a destination filename...
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

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
 my ($y, $m, $d, $ss, $mm, $hh) = (localtime())[5,4,3,0,1,2];
 $y += 1900;
 $m += 1;
 sprintf("%d/%02d/%02d %02d:%02d:%02d", $d, $m, $y, $hh, $mm, $ss);
}

