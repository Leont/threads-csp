#! perl

use strict;
use warnings;

use Test::More;
use threads::csp;

$ENV{PERL5LIB} = join ':', 't/lib', split ':', $ENV{PERL5LIB};

subtest 'First', sub {
	my $q = threads::csp::channel->new;
	my $r = threads::csp->spawn('Basic', 'Basic::basic', $q, 7);

	$q->send(6);
	is($q->receive, 42, 'product is correct');

	is($r->get, 13, 'returns 13');
	is($r->get, 13, 'returns 13 again');
};

subtest 'Second', sub {
	my $r = threads::csp->spawn('Basic', 'Basic::non_existent');
	my $val = eval { $r->get };
	like($@, qr/Undefined subroutine &Basic::non_existent called./);
};

subtest 'Third', sub {
	my $r = threads::csp->spawn('NonExistent', 'Basic::one');
	my $val = eval { $r->get };
	like($@, qr/Can't locate NonExistent.pm in \@INC/);
};

done_testing();
