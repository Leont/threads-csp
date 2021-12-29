package Thread::Csp;

use strict;
use warnings;

use 5.008001;

use XSLoader;
XSLoader::load('Thread::Csp');

1;

#ABSTRACT: Communicating sequential processes threading for Perl

__END__

=head1 SYNOPSIS

 # in script.pl
 use Thread::Csp;
 my $input = Thread::Csp::Channel->new;
 my $output = Thread::Csp::Channel->new;
 Thread::Csp->spawn('Module', 'Module::function', $input, $output);

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

This module implements share-nothing threads for perl. One crucial difference with C<threads.pm> threads is that the original thread will not be cloned except for the arguments that you pass on thread creation. L<Thread::Csp::Channel|Thread::Csp::Channel>s (also using cloning to get values across) are used for inter-thread communication; one or more channels will nearly always be such creation arguments.

Please note that B<at this stage this module is a research project>. In no way is API stability guaranteed. It is released for evaluation purposes only, not for production usage.

=method spawn($module, $sub, @args)

Spawn a new thread. It will load $module and then run C<$sub> (fully-qualified function name) with C<@args> as arguments. It returns a L<Thread::Csp::Promise|Thread::Csp::Promise> that will finish when the thread is finished.

=cut
