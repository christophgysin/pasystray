#!/usr/bin/env bash
set -euo pipefail

rm -rf autom4te.cache
rm -f config.cache
aclocal
autoconf
autoheader
automake --add-missing
CFLAGS="-g -O0" ./configure --sysconfdir=/etc "$@"
make clean
