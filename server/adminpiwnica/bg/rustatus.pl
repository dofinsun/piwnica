#!/usr/bin/perl

# replly html page with RU status (Up/Down)

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();
use DBI;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $debug = 0;

# telnet and DBI initialisation
my $tel = new Net::Telnet (Timeout => 10, Port => 404, Errmode => "return");
my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;

my %RuIps = load_val_dbi('RuName', 'RuIp', 'RmUnits');						#RemoteUnit IP`s
my %RuStat = load_val_dbi('RuName', 'Status', 'RmUnits');					#RemoteUnit status
my %RuSenVal = load_val_dbi('SensorName', 'SensorStatus', 'RmUnitStatus');	#RemoteUnits values

if ($debug) {
	# Print loaded values for check
	print "\%RuIps:-------------------------\n";
	while (my ($k,$v)=each %RuIps){print ":$k:$v:\n"}
	print "\%RuStat:------------------------\n";
	while (my ($k,$v)=each %RuStat){print ":$k:$v:\n"}
	print "\%RuSenVal:----------------------\n";
	while (my ($k,$v)=each %RuSenVal){print ":$k:$v:\n"}
}

update_val_dbi();
$dbh->disconnect();

if ($debug) {
	# Print updated values
	print "\%RuIps:-------------------------\n";
	while (my ($k,$v)=each %RuIps){print ":$k:$v:\n"}
	print "\%RuStat:------------------------\n";
	while (my ($k,$v)=each %RuStat){print ":$k:$v:\n"}
	print "\%RuSenVal:----------------------\n";
	while (my ($k,$v)=each %RuSenVal){print ":$k:$v:\n"}
}

my $status_response = '<p style="color:green;">Ok</p>';
foreach my $key (keys %RuStat) {
	if ($RuStat{$key} eq "Down") {
		$status_response = '<p style="color:red;">Failed!</p>';
	}
}

print "Content-type:text/html\r\n\r\n";
print "<html>";
print "<head>";
print "</head>";
print "<body>";
print $status_response;
print "</body>";
print "</html>\n";

exit 0;

sub load_val_dbi {
	my $stmt = qq(SELECT $_[0], $_[1] from $_[2];);
	my $sth = $dbh->prepare($stmt);
	my $rv = $sth->execute() or die $DBI::errstr;
	if ($rv < 0){
		die $DBI::errstr;
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
					die $DBI::errstr;
				} else {
					print "Updated $rv rows: $stmt\n" if ($debug);
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
							die $DBI::errstr;
						} else {
							print "Updated $rv rows: $stmt\n" if ($debug);
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
					die $DBI::errstr;
				} else {
					print "Updated $rv rows: $stmt\n" if ($debug);
					$RuStat{$RuName} = 'Down';
				}
			}
		}
	}
}
