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
# Failure logs, one per emulator
#
emulators=(cpm cpmulator iz-cpm ntvcm tnylpo)

for emulator in "${emulators[@]}"; do
    rm -f "failures.${emulator}"
done


#
# Print a header
#
echo "### Test Results $(date +%D)"
echo ""
echo "| Test | [cpm](https://github.com/jhallen/cpm) | [cpmulator](https://github.com/skx/cpmulator) | [iz-cpm](https://github.com/ivanizag/iz-cpm) | [ntvcm](https://github.com/davidly/ntvcm) | [tnyplo](https://gitlab.com/gbrein/tnylpo)"
echo "| ---- | -- | --  | -- | -- | -- |"

#
# Did running the given command produce "PASSED" on STDOUT?
#
# Hide STDERR.
#
# If it failed, append the complete output to the emulator's
# failure log.
#
wasPass() {
    local emulator="$1"
    local test="$2"
    shift 2

    "$@" > out.tmp 2>&1

    if grep --silent "PASSED" out.tmp; then
        return 0
    else
        {
            echo "===== ${test} ====="
            cat out.tmp
            echo
        } >> "failures.${emulator}"

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
    if wasPass "cpm" "${file}" "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi

    #
    # CPMULATOR - keep the name as-is
    #
    cmd=(./cpmulator/cpmulator --input=stty "${file}")
    if wasPass "cpmulator" "${file}" "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi


    #
    # iz-cpm - keep the name as-is
    #
    cmd=(./iz-cpm/target/debug/iz-cpm "${file}")
    if wasPass "iz-cpm" "${file}" "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL|"
    fi


    #
    # ntvcm - keep the name as-is
    #
    cmd=(./ntvcm/ntvcm "${file}")
    if wasPass "ntvcm" "${file}" "${cmd[@]}" ; then
        printf " ✔️ |"
    else
        printf " FAIL |"
    fi


    #
    # tnylpo
    #
    # This wants the filename in lower-case - and without the .com suffix
    #
    cmd=(./tnylpo/tnylpo -f .tnylpo.conf tmp)
    if wasPass "tnylpo" "${file}" "${cmd[@]}" ; then
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
# Print failures
#
for emulator in "${emulators[@]}"; do
    if [[ -f "failures.${emulator}" ]]; then
        echo
        echo "#### Failures for ${emulator}"
        echo
        cat "failures.${emulator}"
    fi
done

#
# Remove failure logs
#
for emulator in "${emulators[@]}"; do
    rm -f "failures.${emulator}"
done

#
# Remove the output file
#
rm out.tmp
