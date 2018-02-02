#!/usr/bin/perl
use strict;
use warnings;

my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;

my $audiofile;
if (defined($ARGV[0])) {
	$audiofile = $ARGV[0];
} else {
	die "No variable defined \$Players "
}

my $pid;
unless ($pid = fork) {
  unless (fork) {
    close STDOUT;
    close STDERR;
    while (1) {
      system "mpg123", "-q", "./sound/$audiofile";
    }
    exit 0;
  }
  exit 0;
}
waitpid($pid,0);
