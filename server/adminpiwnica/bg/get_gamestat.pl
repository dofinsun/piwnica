#!/usr/bin/perl

use strict;
use warnings;
use DBI;
use JSON;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1 }) or die $DBI::errstr;

my %GameStat;
my @DbRows;

my $stmt = qq(SELECT Param, Value from GameStat;);
my $sth = $dbh->prepare( $stmt );
my $rv = $sth->execute() or die $DBI::errstr;
if($rv < 0){
	die $DBI::errstr ;
}
while(my @row = $sth->fetchrow_array()) {
	push @DbRows, {"Param" => $row[0], "Value" => $row[1]};
}
$GameStat{"total"} = $sth->rows;
$GameStat{"rows"} = \@DbRows;

$dbh->disconnect();

my $json = encode_json \%GameStat;
print "Content-Type:text/html\n\n";
print "$json\n";
