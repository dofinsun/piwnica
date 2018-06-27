#!/usr/bin/perl

use strict;
use warnings;
use diagnostics;
use Net::Telnet ();

my $debug = 0;
my $IpAddr;
my $Req;
my $timestamp = 0;

if (defined($ARGV[0]) && defined($ARGV[1])) {
        $IpAddr = $ARGV[0];
        $Req = $ARGV[1];
        print "telnet $IpAddr $Req\n" if $debug;
} else {
        die "Usage: program $IpAddr $Req\n";
}

my $RmTel = new Net::Telnet (Timeout => 5, Port => 404, Errmode => "return");

while (1){
  my $tel_ok = $RmTel->open($IpAddr);
  if ($tel_ok) {
    $RmTel->put($Req);
    my @lines = $RmTel->getlines();
    foreach my $ParamSt (@lines) {
      chomp $ParamSt;
      print "$ParamSt" if $debug;
      my ($ParamName, $ParamVal) = split /=/, $ParamSt;
      $timestamp++;
      print "$timestamp   $ParamName=$ParamVal\n";
    }
  } else {
    print "Host unreachable\n";
  }
  $RmTel->close;
  sleep 1;
}

