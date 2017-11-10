#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
my $pid;
print "$$ Parent\n";
unless ($pid = fork) {
  print "$$ Child\n";
  sleep 5;
  unless (fork) {
    print "$$ GrandChild\n";
    sleep 20;
    print "$$ GrandChild exiting\n";
    #die "We will die for a victory\n";
    exit 0;
  }
  print "$$ Child make GrandChild and exit\n";
  exit 0;
}
print "$$ Parent make child\n";
waitpid($pid,0);
print "$$ Parent get SIG Child exit\n"
