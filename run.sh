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
echo "| Test | [cpm](https://github.com/jhallen/cpm) | [cpmulator](https://github.com/skx/cpmulator) | [iz-cpm](https://github.com/ivanizag/iz-cpm) | [ntvcm](https://github.com/davidly/ntvcm) | [tnyplo](https://gitlab.com/gbrein/tnylpo)"
echo "| ---- | -- | --  | -- | -- | -- |"

#
# Did running the given command produce "PASSED" on STDOUT?
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
    # Copy the file to "tmp.com"
    #
    # Because some emulators prefer lower
    # names that are short.
    #
    cp "${file}" "tmp.com"

    #
    # CPM - copy to a lower-cased name, and execute without the ".com" suffix
    #
    cmd=(./cpm/cpm --exec tmp)
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi

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
    # iz-cpm - keep the name as-is
    #
    cmd=(./iz-cpm/target/debug/iz-cpm "${file}")
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL|"
    fi


    #
    # ntvcm - keep the name as-is
    #
    cmd=(./ntvcm/ntvcm "${file}")
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi


    #
    # tnylpo
    #
    # This wants the filename in lower-case - and without the .com suffix
    #
    cmd=(./tnylpo/tnylpo tmp)
    if wasPass "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi

    #
    # End of line
    #
    echo ""

    #
    # Remove the temporary copy of the test-case
    #
    rm "tmp.com"

done

#
# Remove the output file
#
rm out.tmp
