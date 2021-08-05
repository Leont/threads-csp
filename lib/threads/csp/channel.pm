package threads::csp::channel;

use strict;
use warnings;

use 5.008001;

use threads::csp;

1;

#ABSTRACT: Channels for Communicating sequential processes

__END__

=head1 SYNOPSIS

 my $c = threads::csp::channel->new;
 
 $c->send("value");

 my $rec = $c->receive;

=head1 DESCRIPTION

This class represents a channel between two or more CSP threads, allowing any cloneable value (unblessed values, channels and potentially others) to be passed around between threads.

=method new()

This creates a new channel.

=method send($value)

This sends a value over the channel. It will block until another thread is prepared to receive the value.

=method receive()

This receives a value from the channel. It will block until another thread is prepared to send the value.

=method set_notify($handle, $value)

This will cause C<$value> to be written to C<$handle> whenever a new value becomes available, unless it's already being read.
