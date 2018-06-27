#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use DBI;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $debug = 0;

my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
    undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;

my %GameStatus = load_val_dbi('Param', 'Value', 'GameStat', 'Param', 'GameLevel');
my %GameCondition = load_val_dbi('Param', 'Value', 'GameCondition', 'GameStep', $GameStatus{GameLevel});

while (my ($Param,$Value)=each %GameCondition) {
  set_val_dbi($Param,$Value);
}
set_FNS_dbi();

print "Content-type:text/html\r\n\r\n";
print "<html>";
print "<head>";
print "<title></title>";
print "</head>";
print "<body>";
print "Perform step";
print "</body>";
print "</html>";

sub set_FNS_dbi {
  my $stmt = qq(UPDATE GameStat SET Value = '4' WHERE Param = 'ForceNextLevel';);
  print "$stmt\n" if $debug;
  my $rv = $dbh->do($stmt) or die $DBI::errstr;
  if ($rv <0 ){
    die $DBI::errstr;
  } else {
    print "Updated $rv rows: $stmt\n" if $debug;
  }
}

sub set_val_dbi {
	if ((defined $_[0]) && (defined $_[1])) {
		my $stmt =qq(UPDATE RmUnitStatus set SensorStatus = '$_[1]' where SensorName = '$_[0]';);
    print "$stmt\n" if $debug;
		my $rv = $dbh->do($stmt) or die $DBI::errstr;
		if ($rv <0 ){
			die $DBI::errstr;
		} else {
			print "Updated $rv rows: $stmt\n" if $debug;
		}
	}
}

sub load_val_dbi {
	if ((defined $_[0]) && (defined $_[1]) && (defined $_[2]) && (defined $_[3]) && (defined $_[4])) {
		my $stmt = qq(SELECT $_[0], $_[1] from $_[2] where $_[3] = '$_[4]';);
    print "$stmt\n" if $debug;
		my $sth = $dbh->prepare($stmt);
		my $rv = $sth->execute() or die $DBI::errstr;
		if ($rv < 0){
			die $DBI::errstr;
		} else {
			my %returned_val;
			while(my @row = $sth->fetchrow_array()) {
			$returned_val{$row[0]} = $row[1];
			}
      print join ":", %returned_val, "\n" if $debug;
			return %returned_val;
		}
	}
}
