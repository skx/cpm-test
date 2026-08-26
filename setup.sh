#!/bin/sh
#
# Checkout each of the emulators we run our tests with, and
# update from the local repositories as necessary.
#



# cpm
if [ ! -d cpm ] ; then
    git clone https://github.com/jhallen/cpm.git
else
    (cd cpm && git pull)
fi
( cd cpm && make )



# cpmulator
if [ ! -d cpmulator ] ; then
    git clone https://github.com/skx/cpmulator.git
else
    (cd cpmulator && git pull)
fi
( cd cpmulator && go build . )



# iz-cpm
if [ ! -d iz-cpm ] ; then
    git clone https://github.com/ivanizag/iz-cpm.git
else
    (cd iz-cpm && git pull)
fi
( cd iz-cpm && cargo build )



# ntvcm
if [ ! -d ntvcm ]; then
    git clone https://github.com/davidly/ntvcm.git
else
    (cd ntvcm && git pull)
fi
( cd ntvcm && make )



# tnylpo
if [ ! -d tnylpo ] ; then
    git clone https://gitlab.com/gbrein/tnylpo.git
else
    (cd tnylpo && git pull)
fi
( cd tnylpo && make )


