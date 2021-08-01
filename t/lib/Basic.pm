package Basic;

use strict;
use warnings;

sub basic {
	my $q = shift;
	my $first = $q->receive;
	my $second = $q->receive;
	$q->send($first * $second);
	13;
}

1;
