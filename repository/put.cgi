#!/usr/bin/perl

# A simple log file, must be writable by the user that this program runs as...
$putlog = "put.log";

# Check we are using PUT method...
if ($ENV{'REQUEST_METHOD'} ne "PUT") { &reply(500, "Request method is not PUT"); }

# Check we got a destination filename...
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

# Check we got some content...
$clength = $ENV{'CONTENT_LENGTH'};
if (!$clength) { &reply(500, "Content-Length missing or zero ($clength)"); }

# Open output file...
open(OUT, "> $filename") || &reply(500, "Cannot write to $filename");

# Read the content itself from stdin...
$toread = $clength;
while ($toread > 0)
{
    $nread = read(STDIN, $data, $clength);
    &reply(500, "Error reading content") if !defined($nread);
    $toread -= $nread;
    print OUT $data;
}

# Close output file...
close(OUT);

# Reply all OK...
&reply(200, "Uploaded $filename");

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
 open (LOG, ">> $putlog") || return;
 print LOG "$msg\n";
 close(LOG);
}

