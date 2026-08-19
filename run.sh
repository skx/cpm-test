#!/bin/bash
#
# For each of our tests run them with a list of known emulators
#
# NOTE: Some emulators require upper/lower cased filenames, and
# some want the .COM suffix to be stripped.
#
# For example:
#
#  $ ./tnylpo/tnylpo SEARCH
#  tnylpo: command file name (SEARCH) not valid
#
# So we copy to "tmp.com" and execute as just "tmp":
#
#  $ cp SEARCH.COM  tmp.com
#  $ ./tnylpo/tnylpo  tmp
#  PASSED.
#
#



#
# Print a header
#
echo "| Test | [cpm](https://github.com/jhallen/cpm) | [cpmulator](https://github.com/skx/cpmulator) | xx | [tnyplo](https://gitlab.com/gbrein/tnylpo)"
echo "| ---- | -- | -- | -- |"

#
# Did running the given command produce "PASSED" in stdout?
#
# Hide STDERR.
#
function wasPass {
    "$@" > out.tmp 2>&1

    if ( grep --silent "PASSED" out.tmp ) ; then
        return 0
    else
        return 1
    fi
}



#
# For each test
#
for file in *.COM; do


    #
    # Show the name of the test
    #
    printf "| %s | " "${file}"


    #
    # CPM - copy to a lower-cased name, and execute without the ".com" suffix
    #
    cp "${file}" "tmp.com"
    cmd=(./cpm/cpm --exec tmp)
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi
    rm "tmp.com"

    #
    # CPMULATOR - keep the name as-is
    #
    cmd=(./cpmulator/cpmulator --input=stty "${file}")
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi

    #
    # iz-cpm
    #

    #
    # tnylpo
    #
    # This wants the filename in lower-case - and without the .com suffix
    #
    cp "${file}" "tmp.com"
    cmd=(./tnylpo/tnylpo tmp)
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi
    rm "tmp.com"

    #
    # End of line
    #
    echo ""

done
