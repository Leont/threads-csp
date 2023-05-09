package Thread::CSP::Promise;

use strict;
use warnings;

use 5.008001;

use Thread::CSP;

1;

#ABSTRACT: Promises for thread return values.

__END__

=head1 SYNOPSIS

 my $promise = Thread::CSP->spawn('Module', 'Module::function', $input, $output);
 $promise->get;

=head1 DESCRIPTION

This represents the return value of a thread, as return by spawn. B<This class is highly experimental and may disappear in the future>.

=method get()

This waits for the thread to finish, and will either return its value, or throw the exception that it died with. It may be called any number of times.

=method is_finished()

This returns true if the promise is finished.

=method finished_fh()

This returns a handle that one byte will be written to when the promise finishes (or immediately if the promise is already finished).
