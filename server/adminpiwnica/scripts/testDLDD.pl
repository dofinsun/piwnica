#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use DBI;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $debug = 0;

my ($buffer, @pairs, $pair, $name, $value, %FORM);
# Read in text
$ENV{'REQUEST_METHOD'} =~ tr/a-z/A-Z/;
if ($ENV{'REQUEST_METHOD'} eq "GET") {
   $buffer = $ENV{'QUERY_STRING'};
}
# Split information into name/value pairs
@pairs = split(/&/, $buffer);
foreach $pair (@pairs) {
   ($name, $value) = split(/=/, $pair);
   $value =~ tr/+/ /;
   $value =~ s/%(..)/pack("C", hex($1))/eg;
   $FORM{$name} = $value;
}
my $dbh = DBI->connect("DBI:SQLite:dbname=pigame.db",
    undef, undef, { RaiseError => 1 }) or die $DBI::errstr;

my %GameStatus = load_val_dbi('Param', 'Value', 'GameStat');
#For test only
#$FORM{times} = 1;

if (defined($FORM{times}) && $FORM{times} =~ /^\d$/) {
  print "Run $FORM{times} \n" if $debug;

  my $pid = fork();
  if (defined($pid)) {
  	if ($pid == 0) {
  #Child
      unless (fork) {
  #GrandChild
        close STDOUT;
        for (my $laps = 0; $laps < $FORM{times}; $laps++) {
          foreach my $ParamVal (keys %GameStatus) {
            if ($ParamVal =~/^DL/) {
              set_val_dbi('GameStat', 'Value', 'Open', 'Param', $ParamVal);
              sleep 5;
              set_val_dbi('GameStat', 'Value', 'Close', 'Param', $ParamVal);
              sleep 5;
            }
          }
        }
        exit 0;
      }
  #Exiting from Child
      exit 0;
  	} else {
  #Parent process get GIG{CHLD}
  		my $finished = waitpid($pid,0);
  		print "Content-type:text/html\r\n\r\n";
  		print "<html>";
  		print "<head>";
  		print "<title></title>";
  		print "</head>";
  		print "<body>";
  		print "Testung DL and DD.";
  		print "</body>";
  		print "</html>";
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
