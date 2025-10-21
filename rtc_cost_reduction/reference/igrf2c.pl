#!/usr/bin/env perl
#
# Usage:
#  $ perl igrf2c.pl < igrfcoeffs.txt > igrf_coeffs.cpp
#


# C\‘¢‘Ì‚Ì”z—ñ‚É’¼‚·B
print <<EOF;
//////////////////////////////////////////////////
// IGRF_coefficients
// converted by igrf2c.pl, from IGRF text.
// Copyright(C) 2005 Miyamoto Luisch.

namespace rtc { namespace model { namespace magnet {

typedef struct igrf_coeff_element
{
	enum coeffs_mode {g,h} mode;
	int  n,m;
	double year, data, sv;
} IGRF_COEFFICIENT_ELEMENT;

const static IGRF_COEFFICIENT_ELEMENT g_igrf_coeffs[] = {
EOF

##################################################
$_ = <STDIN>;
$_ =~ s/[\n\r]+//g;
my @params = split(/[ \t]+/, $_);

my $line_nums = @params;
my $element_nums  = 0;

while(<STDIN>) {

	$_ =~ s/[\n\r]+//g;
	my @line = split(/[ \t]+/, $_);

	my $elements = @line;
	if( $elements != $line_nums )
	{ next; }
	
	my $g_or_h   = "igrf_coeff_element::${line[0]}";
	my $n        = $line[1];
	my $m        = $line[2];

	for( my $i = 3; $i < $line_nums; ++$i )
	{
		my $year = $params[$i];
		my $coef = $line[$i];
		my $sv   = 0.0;
		
		if( $year eq "SV" )
		{
			last;
		}
		elsif( $params[$i+1] eq "SV" )
		{
			$sv = $line[$i+1];
		}
		else
		{
			$y_diff = $params[$i+1]-$year;
			$sv = ( $line[$i+1] - $coef )/$y_diff;
		}
		
		print "\t{ $g_or_h, $n, $m, $year, $coef, $sv },\n";
		++$element_nums;
	}
}

print <<EOF;
}; // IGRF_COEFFICIENT_ELEMENT

EOF

print "const static double g_igrf_coeffs_years[] = {\n";
for( my $i = 3; $i < $line_nums-1; ++$i ){
	print "\t$params[$i],\n"; 
}
print <<EOF;
	0 // terminate
};

const static int g_igrf_coeffs_nums = $element_nums;

}}};// namespace rtc::model::magnet
EOF
