#!/usr/bin/env sh

cd build
NPASSED=0
NFAILED=0
FAILED=
while read -r testname
do
    if ! ./nain4-tests "$testname" {{FLAGS}}; then
        FAILED=$FAILED"$testname"\\n
        NFAILED=$((NFAILED+1))
    else
        NPASSED=$((NPASSED+1))
    fi
done < <(./nain4-tests {{PATTERN}} --list-tests --verbosity quiet)
if ! [ -z "$FAILED" ]; then
    printf "\\033[91m===========================================================================\n"
    printf "\\033[32m Passed $NPASSED tests, \\033[91m Failed $NFAILED\n\n"
    printf "\\033[91m Failures: \n\n$FAILED\n"
    printf "\\033[91m===========================================================================\n"
    printf "\\033[91mOVERALL: ============================== FAIL ==============================\n"
    printf "\\033[91m===========================================================================\n"
    printf "\\033[0m"
    exit 1
else
    printf "\\033[32m Ran $NPASSED tests\n\n"
    printf "\\033[32m===========================================================================\n"
    printf "\\033[32mOVERALL: ============================== PASS ==============================\n"
    printf "\\033[32m===========================================================================\n"
    printf "\\033[0m"
fi
