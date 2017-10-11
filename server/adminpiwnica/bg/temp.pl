#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();
use DBI;

# telnet and DBI initialisation
my $tel = new Net::Telnet (Timeout => 10, Port => 404, Errmode => "return");
my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;
	
my $GameStep = 3;

my %RuIps = load_val_dbi('RuName', 'RuIp', 'RmUnits');						#RemoteUnit IP`s
my %RuStat = load_val_dbi('RuName', 'Status', 'RmUnits');					#RemoteUnit status 
my %RuSenVal = load_val_dbi('SensorName', 'SensorStatus', 'RmUnitStatus');	#RemoteUnits values

# Print loaded values for check
print "\%RuIps:-------------------------\n";
while (my ($k,$v)=each %RuIps){print ":$k:$v:\n"}
print "\%RuStat:------------------------\n";
while (my ($k,$v)=each %RuStat){print ":$k:$v:\n"}
print "\%RuSenVal:----------------------\n";
while (my ($k,$v)=each %RuSenVal){print ":$k:$v:\n"}

# Main loop
until ($GameStep == 0) {
	print localtime . "\n";
	update_val_dbi();
	# Game logic
	if ($GameStep == 3) {
			print "Step 3\n";
			$GameStep--;
	} elsif ($GameStep == 2) {
			print "Step 2\n";
			$GameStep--;
	} elsif ($GameStep == 1) {
			print "Step 1\n";
			$GameStep--;
	} else {
			print "Step default case\n";
	}
}

$dbh->disconnect();
exit 0;

sub load_val_dbi {
	my $stmt = qq(SELECT $_[0], $_[1] from $_[2];);
	my $sth = $dbh->prepare($stmt);
	my $rv = $sth->execute() or die $DBI::errstr;
	if ($rv < 0){
		print $DBI::errstr;
	}
	my %returned_val;
	while(my @row = $sth->fetchrow_array()) {
	$returned_val{$row[0]} = $row[1];
	}
	return %returned_val;
}

sub update_val_dbi {
	for my $RuName (keys %RuIps) {
		my $tel_ok = $tel->open($RuIps{$RuName});
		if ($tel_ok) {
			if ($RuStat{$RuName} ne 'Up') {
				my $stmt =qq(UPDATE RmUnits set Status = 'Up' where RuName = '$RuName';);
				my $rv = $dbh->do($stmt) or die $DBI::errstr;
				if ($rv <0 ){
					print $DBI::errstr;
				} else {
					print "Updated $rv rows: $stmt\n";
					$RuStat{$RuName} = 'Up';
				}
			}
			$tel -> put("s");
			my @lines = $tel -> getlines();
			foreach my $ParamSt (@lines) {
				chomp $ParamSt;
				my ($ParamName, $ParamVal) = split /=/, $ParamSt;
				if (defined($ParamName) && $ParamName ne "") {
					if ($ParamVal ne $RuSenVal{$ParamName}) {
						my $stmt =qq(UPDATE RmUnitStatus set sensorstatus = '$ParamVal' where sensorname = '$ParamName';);
						my $rv = $dbh->do($stmt) or die $DBI::errstr;
						if ($rv <0 ){
							print $DBI::errstr;
						} else {
							print "Updated $rv rows: $stmt\n";
							$RuSenVal{$ParamName} = $ParamVal;
						}
					}
				}
			}
		} else {
			if ($RuStat{$RuName} ne 'Down') {
				my $stmt =qq(UPDATE RmUnits set Status = 'Down' where RuName = '$RuName';);
				my $rv = $dbh->do($stmt) or die $DBI::errstr;
				if ($rv <0 ){
					print $DBI::errstr;
				} else {
					print "Updated $rv rows: $stmt\n";
					$RuStat{$RuName} = 'Down';
				}
			}
		}
	}
}
