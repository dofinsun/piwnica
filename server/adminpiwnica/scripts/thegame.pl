#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();
use DBI;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

# telnet and DBI initialisation
my $tel = new Net::Telnet (Timeout => 10, Port => 404, Errmode => "return");
my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;

my $debug = 0;	
my $GameStep = 6;
my $ForceNextLevel = 0;

print "Content-type:text/html\r\n\r\n";
print "<html>";
print "<head>";
print "</head>";
print "<body>";
print "ok.";
print "</body>";
print "</html>\n";


my %RuIps = load_val_dbi('RuName', 'RuIp', 'RmUnits');						#RemoteUnit IP`s
my %RuStat = load_val_dbi('RuName', 'Status', 'RmUnits');					#RemoteUnit status 
my %RuSenVal = load_val_dbi('SensorName', 'SensorStatus', 'RmUnitStatus');	#RemoteUnits values
my %GameStatus = load_val_dbi('Param', 'Value', 'GameStat');

# Print loaded values for check
if ($debug) {
	print "\%RuIps:-------------------------\n";
	while (my ($k,$v)=each %RuIps){print ":$k:$v:\n"}
	print "\%RuStat:------------------------\n";
	while (my ($k,$v)=each %RuStat){print ":$k:$v:\n"}
	print "\%RuSenVal:----------------------\n";
	while (my ($k,$v)=each %RuSenVal){print ":$k:$v:\n"}
	print "\%GameStatus:----------------------\n";
	while (my ($k,$v)=each %GameStatus){print ":$k:$v:\n"}
}

# Main loop
set_val_dbi('GameStat', 'Value', 'Run', 'Param', 'GameStat');
set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');

until ($GameStep == 0) {
	print localtime . "\n" if $debug;
	update_val_dbi();
	$ForceNextLevel = load_FNL();
	if ($ForceNextLevel == 2) {
		print "Get signal END_of_the_Game\n";
	}
# Game logic
	if ($GameStep == 3) {
		$GameStep--;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
		print "Step 3\n" if $debug;
	} elsif ($GameStep == 2) {
		$GameStep--;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
		print "Step 2\n" if $debug;
	} elsif ($GameStep == 1) {
		$GameStep--;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
		print "Step 1\n" if $debug;
	} else {
		$GameStep--;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
		print "Step default case\n" if $debug;
	}
}
set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
set_val_dbi('GameStat', 'Value', 'Stop', 'Param', 'GameStat');
$dbh->disconnect();
exit 0;

sub load_val_dbi {
	if ((defined $_[0]) && (defined $_[1]) && (defined $_[2])) {
		my $stmt = qq(SELECT $_[0], $_[1] from $_[2];);
		my $sth = $dbh->prepare($stmt);
		my $rv = $sth->execute() or die $DBI::errstr;
		if ($rv < 0){
			print $DBI::errstr;
		} else {			
			my %returned_val;
			while(my @row = $sth->fetchrow_array()) {
			$returned_val{$row[0]} = $row[1];
			}
			return %returned_val;
		}
	}
}

sub load_FNL {
	my $stmt = qq(SELECT 'Value' from 'GameStat' where 'Param' = 'ForceNextLevel';);
	my $sth = $dbh->prepare($stmt);
	my $rv = $sth->execute() or die $DBI::errstr;
	if ($rv < 0){
		print $DBI::errstr;
	} else {		
		my @row = $sth->fetchrow_array();
		print "ForceNextLevel = $row[0]\n" if $debug;
		return $row[0];
	}
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
					print "Updated $rv rows: $stmt\n" if $debug;
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
							print "Updated $rv rows: $stmt\n" if $debug;
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
					print "Updated $rv rows: $stmt\n" if $debug;
					$RuStat{$RuName} = 'Down';
				}
			}
		}
	}
}

sub set_val_dbi {
	if ((defined $_[0]) && (defined $_[1]) && (defined $_[2]) && (defined $_[3]) && (defined $_[4])) {
		my $stmt =qq(UPDATE $_[0] set $_[1] = '$_[2]' where $_[3] = '$_[4]';);
		my $rv = $dbh->do($stmt) or die $DBI::errstr;
		if ($rv <0 ){
			print $DBI::errstr;
		} else {
			print "Updated $rv rows: $stmt\n" if $debug;
		}
	}
}
