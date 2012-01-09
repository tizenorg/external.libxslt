#!/bin/sh
## ----------------------------------------------------------------------
## autogen.sh : refresh GNU autotools toolchain for libxslt
## ----------------------------------------------------------------------
## Requires: autoconf (2.5x), automake1.9, libtool (1.5.x)
## ----------------------------------------------------------------------

## ----------------------------------------------------------------------
set -e

## ----------------------------------------------------------------------
libtoolize --force --copy

## ----------------------------------------------------------------------
aclocal-1.9

## ----------------------------------------------------------------------
autoheader

## ----------------------------------------------------------------------
automake-1.9 --foreign --add-missing --force-missing --copy

## ----------------------------------------------------------------------
autoconf

# clean up the junk that was created
rm -rf autom4te.cache

## ----------------------------------------------------------------------
exit 0

## ----------------------------------------------------------------------
