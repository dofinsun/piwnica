#!/usr/bin/perl

use strict;
use warnings;

my $exists = `ps h -C thegame.pl`;
if ( $exists ) {
  print "Process is running\n";
} else {
  print "Process is missing\n"
}
