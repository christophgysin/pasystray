#!/bin/bash
set -euo pipefail

export CC=${USE_CC:-$CC}

$CC --version

aclocal
autoconf
autoheader
automake --add-missing
./configure || (cat config.log; exit 1)
make
make test
