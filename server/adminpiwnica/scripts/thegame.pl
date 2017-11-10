#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();
use DBI;
use Switch;

my $debug = 1;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $Players;

if (defined($ARGV[0]) && $ARGV[0] =~ /^\d$/) {
	$Players = $ARGV[0];
} else {
	die "No variable defined \$Players "
}

# telnet and DBI initialisation
my $tel = new Net::Telnet (Timeout => 5, Port => 404, Errmode => "return");
my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;

set_val_dbi('GameStat', 'Value', $Players, 'Param', 'Players');

my $GameStep = 0;
my $LastGameStep = 11;
my $ForceNextLevel = 0;

my %RuIps = load_val_dbi('RuName', 'RuIp', 'RmUnits');						#RemoteUnit IP`s
my %RuStat = load_val_dbi('RuName', 'Status', 'RmUnits');					#RemoteUnit status (Up/Down)
my %RuSenVal = load_val_dbi('SensorName', 'SensorStatus', 'RmUnitStatus');	#RemoteUnits sensor values
my %GameStatus = load_val_dbi('Param', 'Value', 'GameStat');			#Game status values

my %RU_orders = ("RU_01" => {"status", "s", "DL1_unlock", "q", "DL1_lock", "w", "DL12_unlock", "e", "DL12_lock", "r"},
"RU_02" => {"status", "s", "DL23_unlock", "q", "DL23_lock", "w", "DLdpk_unlock", "e", "DLdpk_lock", "r", "DLLightAlarm_unlock", "t", "DLLightAlarm_lock", "y"},
"RU_03" => {"status", "s", "DL34a_unlock", "q", "DL34a_lock", "w", "DL34b_unlock", "e", "DL34b_lock", "r", "DLComBox_unlock", "t", "DLComBox_lock", "y", "DLTruncLed_on", "a", "DLTruncLed_off", "d"},
"RU_04" => {"status", "s", "DLW_unlock", "q", "DLW_lock", "w", "DLT_unlock", "e", "DLT_lock", "r", "key_decrase", "a", "key_reset", "d"},
"RU_05" => {"status", "s", "DLL_unlock", "q", "DLL_lock", "w", "DLS_unlock", "e", "DLS_lock", "r", "Grate_open", "a", "Grate_close", "d"},
"RU_06" => {"status", "s", "DLp1_unlock", "q", "DLp1_lock", "w", "DLp2_unlock", "e", "DLp2_lock", "r", "DLven_unlock", "t",
"DLven_lock", "y", "DLL0_unlock", "u", "DLL0_lock", "i", "DLL1_unlock", "a", "DLL1_lock", "d", "DLL2_unlock", "f", "DLL2_lock", "g"});

# Main loop
set_val_dbi('GameStat', 'Value', 'Run', 'Param', 'GameStat');
set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');

prepare_room();

until ($GameStep == $LastGameStep) {
	print localtime . "\n" if $debug;
	update_val_dbi();
	$ForceNextLevel = load_FNL();
	if ($ForceNextLevel == 2) {
		print "Recived signal END_of_the_Game\n" if $debug;
		$GameStep = $LastGameStep;
		$ForceNextLevel = 0;
		set_val_dbi('GameStat', 'Value', $ForceNextLevel, 'Param', 'ForceNextLevel');
	} elsif ($ForceNextLevel == 1) {
		print "Recived signal Next Step\n" if $debug;
		$GameStep++;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
		$ForceNextLevel = 0;
		set_val_dbi('GameStat', 'Value', $ForceNextLevel, 'Param', 'ForceNextLevel');
	}
# Game logic
	switch ($GameStep) {
		case 0		{ print "$GameStep step\n" if $debug;}
		case 1		{ print "DD1=$RuSenVal{DD1}\n" if $debug;
								if ($RuSenVal{DD1} eq "Close") {
									print "Run GameLevel_1 subrutime.\n" if $debug;
									tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL1_lock});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL1');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 2		{ print "USB=$RuSenVal{USB}\n" if $debug;
								if ($RuSenVal{USB} eq "Open"){
									print "Run GameLevel_2 subrutime.\n" if $debug;
									tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL12_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL12');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 3		{ print "KEYBOX=$RuSenVal{KEYBOX}\n" if $debug;
								if ($RuSenVal{KEYBOX} eq "Open"){
									print "Run GameLevel_3 subrutime.\n" if $debug;
									tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DLLightAlarm_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLLightAlarm');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 4		{ print "PowerCable=$RuSenVal{PowerCable}\n" if $debug;
								if ($RuSenVal{PowerCable} eq "Open"){
									print "Run GameLevel_4 subrutime.\n" if $debug;
									tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DLLightAlarm_lock});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLLightAlarm');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 5		{

							}
		case 6		{

							}
		case 7		{

							}
		case 8		{

							}
		case 9		{

							}
		case 10		{

							}
		case 11		{

							}

		else			{	print "Current step is $GameStep \n" if $debug;
								$GameStep = $LastGameStep;
							}
	}
	print "$GameStep step\n" if $debug;
	sleep 1;
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
			die $DBI::errstr;
		} else {
			my %returned_val;
			while(my @row = $sth->fetchrow_array()) {
			$returned_val{$row[0]} = $row[1];
			}
			if ($debug) {
				while (my ($k,$v)=each %returned_val) {
					print ":$k=$v:\n"
				}
			}
			return %returned_val;
		}
	}
}

sub load_FNL {
	my $stmt = qq(SELECT Value from GameStat where Param = 'ForceNextLevel';);
	my $sth = $dbh->prepare($stmt);
	my $rv = $sth->execute() or die $DBI::errstr;
	if ($rv < 0){
		die $DBI::errstr;
	} else {
		my @row = $sth->fetchrow_array();
		print "ForceNextLevel from DB = $row[0]\n" if $debug;
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
					die $DBI::errstr;
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
						print "Sensor status changed $ParamName=$ParamVal was $RuSenVal{$ParamName}.\n" if $debug;
						my $stmt =qq(UPDATE RmUnitStatus set SensorStatus = '$ParamVal' where SensorName = '$ParamName';);
						my $rv = $dbh->do($stmt) or die $DBI::errstr;
						if ($rv <0 ){
							die $DBI::errstr;
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
					die $DBI::errstr;
				} else {
					print "Updated $rv rows: $stmt\n" if $debug;
					$RuStat{$RuName} = 'Down';
				}
			}
		}
		$tel->close;
	}
}

sub set_val_dbi {
	if ((defined $_[0]) && (defined $_[1]) && (defined $_[2]) && (defined $_[3]) && (defined $_[4])) {
		my $stmt =qq(UPDATE $_[0] set $_[1] = '$_[2]' where $_[3] = '$_[4]';);
		my $rv = $dbh->do($stmt) or die $DBI::errstr;
		if ($rv <0 ){
			die $DBI::errstr;
		} else {
			print "Updated $rv rows: $stmt\n" if $debug;
		}
	}
}

sub tell_order {
  my ($ip, $order) = @_;
	if ((defined $ip) && (defined $order)) {
		my $tel_ok = $tel->open($ip);
	  if ($tel_ok) {
	    $tel -> put($order);
	    $tel->close;
	  } else {
	    die $tel->errmsg;
	  }
	}
	print "tell_order($ip, $order);\n" if $debug;
}

sub prepare_room {
	foreach my $RU_name (keys %RU_orders) {
	  foreach my $RU_comm (keys %{$RU_orders{$RU_name}}){
	    if ($RU_comm =~ /_lock/) {
	      tell_order($RuIps{$RU_name}, $RU_orders{$RU_name}->{$RU_comm});
				my $RU_hasp = $RU_comm;
				$RU_hasp =~ s/_lock//;
				set_val_dbi('GameStat', 'Value', 'Close', 'Param', $RU_hasp);
	    }
	  }
	}
	tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{DLGrate_open});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLGrate');
	tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL1_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL1');
}
