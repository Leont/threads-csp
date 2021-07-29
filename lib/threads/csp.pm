package threads::csp;

use strict;
use warnings;

use 5.008001;

use XSLoader;
XSLoader::load('threads::csp');

1;

#ABSTRACT: Communicating sequential processes threading for Perl

__END__

=head1 SYNOPSIS

 # in script.pl
 use threads::csp;
 my $input = threads::csp::channel->new;
 my $output = threads::csp::channel->new;
 threads::csp->spawn('Module', 'Module::function', $input, $output);

 while (<>) {
     $input->send($_);
     print $output->receive;
 }
 $input->send(undef);


 # in Module.pm
 package Module;
 sub function {
     my ($input, $output) = @_;
     while (defined(my $entry = $input->receive)) {
         $output->send(2 * $entry);
     }
 }
 1;

=head1 DESCRIPTION

This module implements threads for perl. One crucial difference with C<threads.pm> threads is that the threads are disconnected, except by channels. It thus facilitates a message passing style of multi-threading.

Please note that B<this module is a research project>. In no way is API stability guaranteed. It is released for evaluation purposes only, not for production usage.

=method spawn($module, $sub, @args)

Spawn a new thread. It will load $module and then run C<$sub> (fully-qualified function name) with C<@args> as arguments.

=cut
