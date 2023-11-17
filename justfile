# -*-Makefile-*-

default:
    just test-nain4

test PATTERN *FLAGS: install-tests
    sh install/nain4-test/run-each-test-in-separate-process.sh {{PATTERN}} {{FLAGS}}

test-nain4 *FLAGS:
    just test '' {{FLAGS}}

clean:
    fd --no-ignore "^build$"   --exec rm -rf {}
    fd --no-ignore "^install$" --exec rm -rf {}

setup-builddir-only-when-needed src-name build-name:
    #!/usr/bin/env bash
    builddir=nain4/build/{{build-name}}
    if  [ ! -d $builddir ]; then
        meson setup $builddir nain4/{{src-name}}
    else
        echo nain4 {{src-name}} build directory already set up
    fi

install-nain4:
    just setup-builddir-only-when-needed src nain4
    meson compile -C nain4/build/nain4
    meson install -C nain4/build/nain4

install-tests: install-nain4
    just setup-builddir-only-when-needed test nain4-test
    meson compile -C nain4/build/nain4-test
    meson install -C nain4/build/nain4-test

# ----------------------------------------------------------------------
# Add recipes here to help with discovery of recipes in subdirectories

test-examples:
    just examples/

test-client-side:
    just client_side_tests/test-all

test-compile-time:
    just compile-time-tests/test-all
