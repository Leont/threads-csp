package threads::csp::promise;

use strict;
use warnings;

use 5.008001;

use threads::csp;

1;

#ABSTRACT: Promises for thread return values.

__END__

=head1 DESCRIPTION

This represents the return value of a thread, as return by spawn. B<This class is highly experimental and may disappear in the future>.

=method get()

This waits for the thread to finish, and will either return its value, or throw the exception that it died with. It may be called any number of times.
