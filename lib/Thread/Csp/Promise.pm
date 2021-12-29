package Thread::Csp::Promise;

use strict;
use warnings;

use 5.008001;

use Thread::Csp;

1;

#ABSTRACT: Promises for thread return values.

__END__

=head1 SYNOPSIS

 my $promise = Thread::Csp->spawn('Module', 'Module::function', $input, $output);
 $promise->get;

=head1 DESCRIPTION

This represents the return value of a thread, as return by spawn. B<This class is highly experimental and may disappear in the future>.

=method get()

This waits for the thread to finish, and will either return its value, or throw the exception that it died with. It may be called any number of times.

=method is_finished()

This returns true if the promise is finished.

=method set_notify($handle, $value)

This will cause C<$value> to be written to $handle when the promise finishes, unless it's already being waited upon.
