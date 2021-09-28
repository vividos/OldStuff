#!/usr/bin/perl -w
#
# perl script to produce altpng.h from header files
# call with: altpng.h.pl altpng.h $(altpng_headers)
#
# note that this is my first (and surely last) Perl script
#

#
# altpng - a versatile png reading and writing library
# Copyright (c) 2003,2004,2021 Michael Fink
#

use strict;

my $line;
my @rest;

my $header = << "HEADER";
/*! \\file altpng.h

   this is the API header file for altpng

*/

#include <stdio.h>
HEADER


# open output file

open(OUTH, "> altpng.h") or die "Error while open()ing: $!\n";


# print header

{
  open(INH, "< $ARGV[0]") or die "Error while open()ing: $!\n";

  do {
    $line = <INH>;
    if (not $line =~ m{\$Id\:}) { print OUTH $line; }
  } while (not $line =~ m{\$Id\:});
  print OUTH "*/\n" . $header;

  close(\*INH) or die "Error while close()ing: $!\n";
}

# print C++ export statement

print OUTH << "CPLUSPLUS";

#ifdef __cplusplus
extern "C" {
#endif
CPLUSPLUS


# process all passed header files

foreach (@ARGV) {

  print OUTH "\n/* file $_\n";

  open(INH, "<$_") or die "Error while open()ing: $!\n";

  # search line starting with /*!

  do { $line = <INH>; } while (not $line =~ m{\/\*\!});

  # output rest of file

  @rest = <INH>;


  # filter out #include statements

  foreach (@rest) {
    if ($_ =~ m{\#include|needed includes}) {
       $_ = "";
    }
  }

  print OUTH @rest;

  close(\*INH) or die "Error while close()ing: $!\n";

}


print OUTH << "CPLUSPLUS2";

#ifdef __cplusplus
}
#endif

CPLUSPLUS2


# close output file

close(\*OUTH) or die "Error while close()ing: $!\n";


# end
