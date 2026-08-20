#!/bin/sh
#
# Clone each of the emulators we run our tests with.
#

#
# The list of source URLs we clone
#

if [ ! -d cpm ] ; then
    git clone https://github.com/jhallen/cpm.git
    ( cd cpm && make )
fi

if [ ! -d cpmulator ] ; then
    git clone https://github.com/skx/cpmulator.git
    ( cd cpmulator && go build . )
fi

if [ ! -d tnylpo ] ; then
    git clone https://gitlab.com/gbrein/tnylpo.git
    ( cd tnylpo && make )
fi

if [ ! -d iz-cpm ] ; then
    git clone https://github.com/ivanizag/iz-cpm.git
    cd iz-cpm && cargo build
fi
