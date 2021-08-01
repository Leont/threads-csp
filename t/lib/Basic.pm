package Basic;

use strict;
use warnings;

sub basic {
	my ($q, $second) = @_;
	my $first = $q->receive;
	$q->send($first * $second);
	13;
}

1;
