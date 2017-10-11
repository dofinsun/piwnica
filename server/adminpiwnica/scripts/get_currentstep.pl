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
my $Hint;
selectfromDB(qq(SELECT Value FROM GameStat WHERE Param = 'GameLevel';), \$GameStep);
selectfromDB(qq(SELECT HintText FROM GameHint WHERE CurrentStep = '$GameStep';), \$Hint);
$dbh->disconnect();

$Reply{currentstep} = $GameStep;
$Reply{reaction} = $Hint;

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
