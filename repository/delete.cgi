#!/usr/bin/perl

# Very simple DELETE handler. Read the Apache Week article before attempting
# to use this script. You are responsible for ensure that this script is
# used securely.

# A simple log file, must be writable by the user that this program runs as.
# Should not be within the document tree.
$putlog = "delete.log";

# Check we are using DELETE method
if ($ENV{'REQUEST_METHOD'} ne "DELETE") { &reply(500, "Request method is not DELETE"); }

# Note: should also check we are an authentication user by checking
# REMOTE_USER

# Check we got a destination filename
$filename = $ENV{'PATH_TRANSLATED'};
if (!$filename) { &reply(500, "No PATH_TRANSLATED"); }

# Delete the file

# Everything seemed to work, reply with 204 (or 200). Should reply with 201
# if content was created, not updated.
&reply(204);

exit(0);

#
# Send back reply to client for a given status.
#

sub reply
{
    local($status, $message) = @_;
    local($remuser, $remhost, $logline) = ();

    print "Status: $status\n";
    print "Content-Type: text/html\n\n";

    if ($status == 200) {
        print "<HEAD><TITLE>OK</TITLE></HEAD><H1>Delete Accepted</H1>\n";
    } elsif ($status == 500) {
        print "<HEAD><TITLE>Error</TITLE></HEAD><H1>Error Deleting File</H1>\n";
	print "An error occurred deleting this file ($message).\n";
    }
    # Note: status 204 and 201 gives have content part

    # Create a simple log
    $remuser = $ENV{'REMOTE_USER'} || "-";
    $remhost = $ENV{'REMOTE_HOST'} || $ENV{'REMOTE_ADDR'} || "-";
    
    $logline = "$remhost $remuser $filename status $status";
    $logline .= " ($message)" if ($status == 500);
    &log($logline);
    exit(0);
}

sub log
{
    local($msg) = @_;
    open (LOG, ">> $putlog") || return;
    print LOG "$msg\n";
    close(LOG);
}
