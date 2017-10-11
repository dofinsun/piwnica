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

my %RuStat;
my @DbRows;

my $stmt = qq(SELECT RemoteUnitNum, SensorName, SensorStatus from RmUnitStatus;);
my $sth = $dbh->prepare( $stmt );
my $rv = $sth->execute() or die $DBI::errstr;
if($rv < 0){
	print $DBI::errstr;
}
while(my @row = $sth->fetchrow_array()) {
	push @DbRows, {"RemoteUnitNum" => $row[0], 
					"SensorName" => $row[1], 
					"SensorStatus" => $row[2]};
}
$RuStat{"total"} = $sth->rows;
$RuStat{"rows"} = \@DbRows;
	
$dbh->disconnect();

my $json = encode_json \%RuStat;
print "Content-Type:text/html\n\n";
print "$json\n";
