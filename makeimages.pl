#!/usr/bin/perl

use strict;

my @IN = (
    "funky",
    "gasket",
#    "if",
#    "line",
    "polygon",
    "ring",
    "spiral",
    "star",
    "maze1"
    );

my $PROG = "turtle";
die "'$PROG' doesn't exist!\n" unless -x $PROG;

sub EXE {
    my $CMD = shift;
    print "$CMD\n";
    system($CMD) == 0 or die "$!\n";
}

for (@IN) {
    EXE("./$PROG examples/$_.turtle | ./turtle.pl | convert - $_.png");
}

print "passed\n";
exit 0;
