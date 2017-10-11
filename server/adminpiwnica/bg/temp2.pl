#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;

print "$0\n";


my $script_path = $0;
$script_path =~ s/\/[\w \d]+\.pl$//;
chdir $script_path;


print "$script_path\n";
if (-e "temp2.pl") {
	print "Ok\n";
} else {
	print "Cant find temp2.pl\n";
}
