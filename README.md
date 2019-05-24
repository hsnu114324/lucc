lucc
==============

lucc is a compiler for the C-- programming language.

lucc supports x86-64 Linux only. It was only tested in ubuntu 14.04 LTS.

lucc include a parser, optimizer and generator.

parser(lucc) supports if(), for(), function.

optimizer(luop) supports constprop, copyprop, dce.

generator(lugen) supports x86-64 Linux.


Build
-----

Run make to build:

    make

How to test:

    ./test.sh

The resuly of the test:

    result: 49





