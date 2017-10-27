#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();
use DBI;

my $debug = 0;
my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my %RU_orders = ("RU_01" => {"status", "s", "DL1_unlock", "q", "DL1_lock", "w", "DL12_unlock", "e", "DL12_lock", "r"},
"RU_02" => {"status", "s", "DL23_unlock", "q", "DL23_lock", "w", "DLdpk_unlock", "e", "DLdpk_lock", "r", "DLLightAlarm_unlock", "t", "DLLightAlarm_lock", "y"},
"RU_03" => {"status", "s", "DL34a_unlock", "q", "DL34a_lock", "w", "DL34b_unlock", "e", "DL34b_lock", "r", "DLComBox_unlock", "t", "DLComBox_lock", "y", "DLTruncLed_on", "a", "DLTruncLed_off", "d"},
"RU_04" => {"status", "s", "DLW_unlock", "q", "DLW_lock", "w", "DLT_unlock", "e", "DLT_lock", "r", "key_decrase", "a", "key_reset", "d"},
"RU_05" => {"status", "s", "DLL_unlock", "q", "DLL_lock", "w", "DLS_unlock", "e", "DLS_lock", "r", "Grate_open", "a", "Grate_close", "d"},
"RU_06" => {"status", "s", "DLp1_unlock", "q", "DLp1_lock", "w", "DLp2_unlock", "e", "DLp2_lock", "r", "DLven_unlock", "t",
"DLven_lock", "y", "DLL0_unlock", "u", "DLL0_lock", "i", "DLL1_unlock", "a", "DLL1_lock", "d", "DLL2_unlock", "f", "DLL2_lock", "g"});


my $tel = new Net::Telnet (Timeout => 5, Port => 404, Errmode => "return");

my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
	undef, undef, { RaiseError => 1, AutoCommit => 1 }) or die $DBI::errstr;
my %RuIps = load_val_dbi('RuName', 'RuIp', 'RmUnits');


foreach my $RU_name (keys %RU_orders) {
  foreach my $RU_comm (keys %{$RU_orders{$RU_name}}){
    if ($RU_comm =~ /_unlock/) {
      tell_order($RuIps{$RU_name}, $RU_orders{$RU_name}->{$RU_comm});
			my $RU_hasp = $RU_comm;
			$RU_hasp =~ s/_unlock//;
			set_val_dbi('GameStat', 'Value', 'Open', 'Param', $RU_hasp);
    }
  }
}
tell_order($RuIps{RU_06}, $RU_orders{RU_06}->{DLp1_lock});
set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLp1');
tell_order($RuIps{RU_06}, $RU_orders{RU_06}->{DLp2_lock});
set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLp2');
tell_order($RuIps{RU_04}, $RU_orders{RU_04}->{DLT_lock});
set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLT');
tell_order($RuIps{RU_02}, $RU_orders{RU_02}->{DLdpk_lock});
set_val_dbi('GameStat', 'Value', 'Close', 'Param', 'DLdpk');

$dbh->disconnect();

print "Content-type:text/html\r\n\r\n";
print "<html>";
print "<head>";
print "<title></title>";
print "</head>";
print "<body>";
print "MK-3 in service state.";
print "</body>";
print "</html>";

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
			print join "=", %returned_val, "\n" if $debug;
			return %returned_val;
		}
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
			print "tell_order($ip, $order);\n" if $debug;
	  } else {
	    die $tel->errmsg;
	  }
	}
}
