#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my ($buffer, @pairs, $pair, $name, $value, %FORM);
# Read in text
$ENV{'REQUEST_METHOD'} =~ tr/a-z/A-Z/;
if ($ENV{'REQUEST_METHOD'} eq "GET") {
   $buffer = $ENV{'QUERY_STRING'};
}
# Split information into name/value pairs
@pairs = split(/&/, $buffer);
foreach $pair (@pairs) {
   ($name, $value) = split(/=/, $pair);
   $value =~ tr/+/ /;
   $value =~ s/%(..)/pack("C", hex($1))/eg;
   $FORM{$name} = $value;
}
# my %FORM = qw(Players 4);
#$SIG{CHLD} = 'IGNORE';
my $exists = `ps h -C thegame.pl`;
if ( $exists ) {
  print "Content-type:text/html\r\n\r\n";
  print "<html>";
  print "<head>";
  print "<title></title>";
  print "</head>";
  print "<body>";
  print "Already running!";
  print "</body>";
  print "</html>";
} else {
  my $pid = fork();
  if (defined($pid)) {
  	if ($pid == 0) {
  #Child
      unless (fork) {
  #GrandChild
        close STDOUT;
        exec("./thegame.pl $FORM{Players}") or die "Unable to start thegame.pl script";
        exit 0;
      }
  #Exiting from Child
      exit 0;
  	} else {
  #Parent process get GIG{CHLD}
  		my $finished = waitpid($pid,0);
  		print "Content-type:text/html\r\n\r\n";
  		print "<html>";
  		print "<head>";
  		print "<title></title>";
  		print "</head>";
  		print "<body>";
  		print "Started with $FORM{Players} players";
  		print "</body>";
  		print "</html>";
  	}
  } else {
  #fork return undefined
  	print "Content-type:text/html\r\n\r\n";
  	print "<html>";
  	print "<head>";
  	print "<title></title>";
  	print "</head>";
  	print "<body>";
  	print "Failed to start!";
  	print "</body>";
  	print "</html>";
  }
}
