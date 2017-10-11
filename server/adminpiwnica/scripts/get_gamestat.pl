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

my @GameStat;
my @RuStat;
my @SensorStat;
my %Reply = (GameStat => \@GameStat, RuStat => \@RuStat, SensorStat => \@SensorStat);

selectfromDB(qq(SELECT Param, Value from GameStat;), \@GameStat);
selectfromDB(qq(SELECT RuName, Status from RmUnits;), \@RuStat);
selectfromDB(qq(SELECT SensorName, SensorStatus from RmUnitStatus;), \@SensorStat);

$dbh->disconnect();

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
		push @$harray, {"Param" => $row[0], "Value" => $row[1]};
	}
}
