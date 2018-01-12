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

my %Reply = ();
my $GameStep;
my $Move;
selectfromDB(qq(SELECT Value FROM GameStat WHERE Param = 'GameLevel';), \$GameStep);
selectMovefromDB(qq(SELECT SensorStatus FROM RmUnitStatus WHERE SensorName LIKE 'PIR_';), \$Move);
$dbh->disconnect();

$Reply{currentstep} = $GameStep;
$Reply{movement} = $Move;

my $json = encode_json \%Reply;
print "Content-Type:text/html\n\n";
print "$json\n";

sub selectfromDB {
	my $stmt = shift;
	my $harray = shift;
	my $sth = $dbh->prepare( $stmt );
	my $rv = $sth->execute() or die $DBI::errstr;
	if($rv < 0){
		die $DBI::errstr ;
	}
	while(my @row = $sth->fetchrow_array()) {
		$$harray = $row[0];
	}
}

sub selectMovefromDB {
	my $stmt = shift;
	my $harray = shift;
	my $sth = $dbh->prepare( $stmt );
	my $rv = $sth->execute() or die $DBI::errstr;
	if($rv < 0){
		die $DBI::errstr ;
	}
	$$harray = "Clear";
	while(my @row = $sth->fetchrow_array()) {
		if ($row[0] eq "Move") {
			$$harray = "Move";
		}
	}
}
