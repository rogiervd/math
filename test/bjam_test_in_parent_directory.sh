#!/bin/bash

# Run this from the root directory of the "math" repository.

# This script will generate a directory ../math-test and not remove it!
# So watch out where you run it.

# Run the tests by using this repository as submodule of the "math-test"
# repository.

# This is necessary because this repository cannot be tested by itself.


# Travis CI looks for this line.
set -ev

set -o nounset
set -o errexit

(
    # Check out the "master" branch of "math-test" from GitHub, in the parent
    # directory.

    cd ../
    git clone git://github.com/rogiervd/math-test.git
    cd math-test
    git checkout master
    git submodule init
    git submodule update
    # Then replace the "math" submodule with the one in ../math.
    rm -r math
    ln -s ../math

    # Test it
    bjam test "$@"

)
