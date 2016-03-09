#!/usr/bin/perl

#
# Turtle Command Interpretter.
# Reads commands from STDIN and writes
# a PGM image to STDOUT
# Wayne O. Cochran wcochran@vancouver.wsu.edu
#

#
# Fetch turtle commands and store in buffer.
#
my @commands;  # array of turtle commands

for (<>) {
    chop;
    push @commands, $_;
}

#
# Creates PGM image.
#
sub pgm_create {
    my ($w, $h) = @_;
    my $pixels;
    for (my $r = 0; $r < $w; $r++) {
	for (my $c = 0; $c < $h; $c++) {
	    $pixels->[$r][$c] = 0;
	}
    }
    return {"width" => $w, "height" => $h, "pixels" => $pixels};
}

#
# Writes PGM image to file.
#
sub pgm_write {
    my ($image, $fname) = @_;
    my $w = $image->{width};
    my $h = $image->{height};

    my $fh;
    if ($fname eq "-") {
	$fh = *STDOUT;
    } else {
	open($fh, ">$fname") or return -1;
    }
    print $fh "P2\n$w $h\n255\n";
    my $pixels = $image->{pixels};
    for (my $r = 0; $r < $h; $r++) {
	for (my $c = 0; $c < $w; $c++) {
	    print $fh "$pixels->[$r][$c] ";
	}
	print $fh "\n";
    }    
    close $fh unless $fh == *STDOUT;

    return 0;
}

#
# Rasterize a line into a PGM image.
#
sub pgm_drawline {
    my ($image, $x0, $y0, $x1, $y1) = @_;
    my $w = $image->{width};
    my $h = $image->{height};
    my $pixels = $image->{pixels};
    my $dx = $x1 - $x0;
    my $dy = $y1 - $y0;
    if (abs($dx) >= abs($dy)) {
	return if $dx == 0;
	my $dydx = $dy/$dx;
	my $di = 1;
	if ($dx < 0) {
	    $di = -1;
	    $dydx = -$dydx;
	}
	my $i = int($x0 + 0.5);
	my $iend = int($x1 + 0.5);
	$y0 += 0.5;
	while ($i != $iend) {
	    my $j = int($y0); 
	    $pixels->[$j][$i] = 255 
		unless $j < 0 || $j >= $h || $i < 0 || $i >= $w;
	    $i += $di;
	    $y0 += $dydx;
	}
    } else {
	my $dxdy = $dx/$dy;
	my $dj = 1;
	if ($dy < 0) {
	    $dj = -1;
	    $dxdy = -$dxdy;
	}
	my $j = int($y0 + 0.5);
	my $jend = int($y1 + 0.5);
	$x0 += 0.5;
	while ($j != $jend) {
	    my $i = int($x0);
	    $pixels->[$j][$i] = 255
		unless $j < 0 || $j >= $h || $i < 0 || $i >= $w;
	    $j += $dj;
	    $x0 += $dxdy;
	}
    }   
}

my $DTOR = 3.141592653589/180;

#
# Subroutine that executes turtle commands.
# The subroutine for drawing lines is passed
# as an argument.
#
sub turtle {
    my $drawline = shift;
    my $x = 0, $y = 0, $dir = 0;
    my $pendown = 1;
    my @stack;
    for (@commands) {
	if (/H/) {
	    $x = $y = $dir = 0;
	} elsif (/U/) {
	    $pendown = 0;
	} elsif (/D/) {
	    $pendown = 1;
	} elsif (/M\s+(\S+)/) {
	    my $dist = $1;
	    my $xnew = $x + $dist*cos($dir*$DTOR);
	    my $ynew = $y + $dist*sin($dir*$DTOR);
	    &{$drawline}($x,$y,$xnew,$ynew) if $pendown;
	    $x = $xnew;
	    $y = $ynew;
	} elsif (/R\s+(\S+)/) {
	    my $angle = $1;
	    $dir += $angle;
	} elsif (/\[/) {
	    push @stack, [$x, $y, $dir];
	} elsif (/\]/) {
	    my $e = pop @stack;
	    $x = $e->[0];
	    $y = $e->[1];
	    $dir = $e->[2];
	}
    }
}

#
# Get extent of turtles drawing
#
my $minx = 10000, $miny = 10000, $maxx = -10000, $maxy = -10000;

sub drawline_extent {
    my ($x, $y, $x1, $y1) = @_;
    $minx = $x if $x < $minx;
    $minx = $x1 if $x1 < $minx;
    $maxx = $x if $x > $maxx;
    $maxx = $x1 if $x1 > $maxx;
    $miny = $y if $y < $miny;
    $miny = $y1 if $y1 < $miny;
    $maxy = $y if $y > $maxy;
    $maxy = $y1 if $y1 > $maxy;
}

turtle(\&drawline_extent);

print STDERR "bounds = ($minx, $miny) -- ($maxx, $maxy)\n";

#
# Create and draw image
#
my $W = 600;
my $H = 600;

sub toPixel {
    my ($x, $y) = @_;
    my $c = ($x - $minx)*($W-1) / ($maxx - $minx);
    my $r = ($maxy - $y)*($H-1) / ($maxy - $miny);
    return [$c, $r];
}

$image = pgm_create($W, $H);

sub drawline_image {
    my ($x0, $y0, $x1, $y1) = @_;
    my $pix0 = toPixel($x0, $y0);
    my $pix1 = toPixel($x1, $y1);
    pgm_drawline($image, 
		 $pix0->[0], $pix0->[1],
		 $pix1->[0], $pix1->[1]);
}

turtle(\&drawline_image);

pgm_write($image, "-") == 0 or die "Unable to create image!\n";

exit 0;
