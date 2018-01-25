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
"RU_03" => {"status", "s", "DL34a_unlock", "q", "DL34a_lock", "w", "DL34b_unlock", "e", "DL34b_lock", "r", "DLComBox_unlock", "t", "DLComBox_lock", "y", "DLTruncLed_on", "a", "DLTruncLed_off", "d", "RedAlert_unlock", "f", "RedAlert_lock", "g"},
"RU_04" => {"status", "s", "DLW_unlock", "q", "DLW_lock", "w", "DLT_unlock", "e", "DLT_lock", "r", "key_decrase", "a", "key_reset", "d"},
"RU_05" => {"status", "s", "DLL_unlock", "q", "DLL_lock", "w", "DLS_unlock", "e", "DLS_lock", "r", "DLGH_unlock", "f", "DLGH_lock", "g", "DLFAC_unlock", "h", "DLFAC_lock", "j", "Grate_open", "a", "Grate_close", "d"},
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
		case 0		{ print "DD1=$RuSenVal{DD1}\n" if $debug;
								if ($RuSenVal{DD1} eq "Close") {
									print "Button DD1 was pushed. Go to next level to start game.\n" if $debug;
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 1		{ print "USB=$RuSenVal{USB}\n" if $debug;
								if ($RuSenVal{USB} eq "Open"){
									print "USBbox was open. Open door12\n" if $debug;
									tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL12_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL12');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 2		{ print "KEYBOX=$RuSenVal{KEYBOX}\n" if $debug;
								if ($RuSenVal{KEYBOX} eq "Open"){
									print "KeyBox was open. Enable LightAlarm.\n" if $debug;
									system "./playsiren.pl";
									tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DLLightAlarm_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLLightAlarm');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 3		{ print "JACK=$RuSenVal{JACK} KEY=$RuSenVal{KEY}\n" if $debug;
								if (($RuSenVal{JACK} eq "Open") && ($RuSenVal{KEY} eq "Open")){
									print "JACK is solved and KEY pushed. Open D23.\n" if $debug;
									tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DL23_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL23');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 4		{ print "PowerCable=$RuSenVal{PowerCable}\n" if $debug;
								if ($RuSenVal{PowerCable} eq "Open"){
									print "Power cable was pluged. Svitch LightAlarm to light.\n" if $debug;
									system "killall", "playsiren.pl";
									system "killall", "mpg123";
									tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DLLightAlarm_lock});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLLightAlarm');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 5		{ print "Table KEYS=$RuSenVal{KEY0} $RuSenVal{KEY1} $RuSenVal{KEY2} $RuSenVal{KEY3}\n" if $debug;
								if ($GameStatus{Players} == 3) {
									if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open")){
										print "KEY0 and KEY1 has been turned on. Light TruncLed.\n" if $debug;
										TruncLed(1);
									}
								}
								if ($GameStatus{Players} == 4) {
									if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open")){
										print "KEY0,1,2 has been turned on. Light TruncLed.\n" if $debug;
										TruncLed(1);
									}
								}
								if ($GameStatus{Players} == 5) {
									if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open") && ($RuSenVal{KEY3} eq "Open")){
										print "KEY0,1,2,3 has been turned on. Light TruncLed.\n" if $debug;
										TruncLed(1);
									}
								}
							}
		case 6		{ print "TruncButton=$RuSenVal{TruncButton} \n" if $debug;
								if ($RuSenVal{TruncButton} eq "Open"){
									if ($GameStatus{Players} == 3) {
										if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open")){
											print "TruncButton has been pushed and KEY0, KEY1 still turned on. Open DL34a.\n" if $debug;
											tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_unlock});
											set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34a');
											$GameStep++;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											TruncLed(0);
											tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL12_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL12');
											tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DL23_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
										}else{
											TruncLed(0);
											$GameStep--;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
										}
									}
									if ($GameStatus{Players} == 4) {
										if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open")){
											print "TruncButton has been pushed and KEY0,1,2 still turned on. Open DL34a.\n" if $debug;
											tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_unlock});
											set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34a');
											$GameStep++;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											TruncLed(0);
											tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL12_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL12');
											tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DL23_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
										}else{
											TruncLed(0);
											$GameStep--;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
										}
									}
									if ($GameStatus{Players} == 5) {
										if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open") && ($RuSenVal{KEY3} eq "Open")){
											print "TruncButton has been pushed and KEY0,1,2,3 still turned on. Open DL34a.\n" if $debug;
											tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_unlock});
											set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34a');
											$GameStep++;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											TruncLed(0);
											tell_order($RuIps{RU_01}, $RU_orders{RU_01}->{DL12_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL12');
											tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DL23_lock});
											set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
										}else{
											TruncLed(0);
											$GameStep--;
											set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
										}
									}
								}
							}
		case 7		{ print "DD34a=$RuSenVal{DD34a}\n UZ0=$RuSenVal{UZ0}" if $debug;
								if ($RuSenVal{UZ0} eq "Detect"){
									if ($RuSenVal{DD34a} eq "Close"){
										if ($GameStatus{Players} == 3) {
											if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open")){
												print "UZ0 detected, DD34a closed, KEY0,1 turned. Transpond.\n" if $debug;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_lock});
												set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
												sleep 3;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34b_unlock});
												set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34b');
												$GameStep++;
												set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											}
										}
										if ($GameStatus{Players} == 4) {
											if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open")){
												print "UZ0 detected, DD34a closed, KEY0,1,2 turned. Transpond.\n" if $debug;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_lock});
												set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
												sleep 3;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34b_unlock});
												set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34b');
												$GameStep++;
												set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											}
										}
										if ($GameStatus{Players} == 5) {
											if (($RuSenVal{KEY0} eq "Open") && ($RuSenVal{KEY1} eq "Open") && ($RuSenVal{KEY2} eq "Open") && ($RuSenVal{KEY3} eq "Open")){
												print "UZ0 detected, DD34a closed, KEY0,1,2,3 turned. Transpond.\n" if $debug;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34a_lock});
												set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DL34a');
												sleep 3;
												tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DL34b_unlock});
												set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DL34b');
												$GameStep++;
												set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
											}
										}
									}
								}
							}
		case 8		{ print "PIR5=$RuSenVal{PIR5}\n" if $debug;
								if ($RuSenVal{PIR5} eq "Detect"){
									print "Intruder detected. Enable Sirene.\n" if $debug;
									system "./playsiren.pl";
									tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{RedAlert_unlock});
									set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'RedAlert');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
							}
		case 9		{ print "TOUCH $RuSenVal{TOUCHl} $RuSenVal{TOUCHr}\n" if $debug;
								if (($RuSenVal{TOUCHl} eq "Available") && ($RuSenVal{TOUCHr} eq "Available")) {
									print "Prisoner in the jail.\n" if $debug;
									tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{DLGH_lock});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLGH');
									tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{Grate_close});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLGrate');
									system "killall", "playsiren.pl";
									system "killall", "mpg123";
									tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{RedAlert_lock});
									set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'RedAlert');
									$GameStep++;
									set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
								}
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
	tell_order($RuIps{RU_06}, $RU_orders{RU_06}->{DLven_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLven');
	tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{DLGH_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLGH');
	sleep 1;
	tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{Grate_open});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLGrate');
	tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{DLL_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLL');
	tell_order($RuIps{RU_05}, $RU_orders{RU_05}->{DLS_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLS');
	tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DLComBox_unlock});
	set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLComBox');
}

sub TruncLed {
	if ($_[0] == 1){
		tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DLTruncLed_on});
		set_val_dbi('GameStat', 'Value', 'Open', 'Param', 'DLTruncLed');
		$GameStep++;
		set_val_dbi('GameStat', 'Value', $GameStep, 'Param', 'GameLevel');
	}else{
		tell_order($RuIps{RU_03}, $RU_orders{RU_03}->{DLTruncLed_off});
		set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLTruncLed');
	}
}
