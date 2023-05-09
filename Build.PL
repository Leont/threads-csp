use strict;
use warnings;
use Module::Build;
use Config;

if (not $Config{useithreads}) {
    print STDERR "Can't use Threads::CSP without threading support";
    exit 0;
}

my $builder = Module::Build->new(
  module_name  => 'Thread::CSP',
  c_source     => 'src',
##{ $plugin->get_prereqs ##}
);

if ($^O ne 'MSWin32') {
	$builder->extra_compiler_flags('-pthread');
	$builder->extra_linker_flags('-pthread');
}

$builder->create_build_script();
