#!/usr/bin/perl
use strict;
use warnings;

my $pid;
unless ($pid = fork) {
  unless (fork) {
    close STDOUT;
    close STDERR;
    while (1) {
      system "mpg123", "-q", "./sound/ElectricEmergency.mp3";
    }
    exit 0;
  }
  exit 0;
}
waitpid($pid,0);
