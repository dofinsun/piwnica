#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use DBI;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $debug = 0;
print "Content-type:text/html\r\n\r\n";

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

#$FORM{FNS} = 1;
if (defined($FORM{FNS}) && $FORM{FNS} =~ /^\d$/) {
  my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
    undef, undef, { RaiseError => 1 }) or die $DBI::errstr;
  my $stmt = qq(UPDATE GameStat SET Value = '$FORM{FNS}' WHERE Param = 'ForceNextLevel';);
  my $rv = $dbh->do($stmt) or die $DBI::errstr;
  if ($rv <0 ){
    die $DBI::errstr;
  } else {
    print "Updated $rv rows: $stmt\n" if $debug;
  }
}
