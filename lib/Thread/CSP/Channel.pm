package Thread::CSP::Channel;

use strict;
use warnings;

use 5.008001;

use Thread::CSP;

1;

#ABSTRACT: Channels for Communicating sequential processes

__END__

=head1 SYNOPSIS

 my $c = Thread::CSP::Channel->new;
 
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

=method receive_ready_fh()

This will return a filehandle that one byte will be written to when a value has been send to the channel.

=method send_ready_fh()

This will return a filehandle that one byte will be written to when a value is being received.

=method close()

This will close the queue. Any C<receive> will now return undef, and any write is ignored.
