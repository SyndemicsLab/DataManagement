#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load cmake/3.22.2
    module load boost/1.83.0
    module load sqlite3
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the datamanagement Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|-l|-n]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
    echo "l              Build Shared Library."
    echo "n              Build Benchmarking executable."
    echo "i PATH         Install Library To PATH."
}

# red output
RED='\033[0;31m'
# no color
NC='\033[0m'

err () {
    echo -e "$RED$1$NC"
}

# set default build type
BUILDTYPE="Debug"
BUILD_TESTS=""
BUILD_BENCHMARK=""
BUILD_SHARED_LIBS="OFF"
INSTALL_PATH="${CMAKE_SOURCE_DIR}/install"
INSTALL=""

# process optional command line flags
while getopts ":lhnpti:" option; do
    case $option in
        h)
            showhelp
            exit
            ;;
        t)
            case "$OPTARG" in
                "Debug"|"Release")
                    BUILDTYPE="$OPTARG"
                    ;;
                *)
                    echo "Specified build type is invalid!"
                    exit
                    ;;
            esac
            ;;
        p)
            BUILD_TESTS="ON"
            ;;
        l)
            BUILD_SHARED_LIBS="ON"
            ;;
        n)
            BUILD_BENCHMARK="ON"
            ;;
        i)
            INSTALL_PATH="$OPTARG"
            INSTALL="ON"
            ;;
        \?)
            err "Error: Invalid option flag provided!"
            showhelp
            exit
            ;;
    esac
done

(
    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"
    # ensure the `lib/` directory exists
    # ([[ -d "lib/" ]] && rm -rf lib/*) || mkdir "lib/"

    (
        cd "build" || exit
        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_BUILD_TYPE=$BUILDTYPE"
        if [[ -n "$BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_TESTS=$BUILD_TESTS"
            rm -rf ../bin/dataTests
        fi
        # build benchmarking executable
        if [[ -n "$BUILD_BENCHMARK" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_BENCHMARK=$BUILD_BENCHMARK"
        fi
        # build static library if BUILD_STATIC_LIBRARY is on, otherwise build
        # shared library
        CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS"

        err "[EXECUTE] $CMAKE_COMMAND"
            # run the full build command as specified
        $CMAKE_COMMAND
        # catch the error if build fails
        ERROR_CODE="$?"
        if [[ "$ERROR_CODE" -ne "0" ]]; then
            err "Build failed. Exiting with error code $ERROR_CODE..."
            exit "$ERROR_CODE"
	    fi
        (
            # determine the number of processing units available
            CORES="$(nproc --all)"
            # if CORES > 1 compile in parallel where possible
            ([[ -n "$CORES" ]] && cmake --build . -j"$CORES") || cmake --build .
        )
    
        # Install project
        if [[ (-n "$INSTALL") ]]; then
            err "[EXECUTE] cmake --install . --prefix=$INSTALL_PATH"
            # if CORES > 1 compile in parallel where possible
            cmake --install . --prefix=$INSTALL_PATH
        fi
    )

    # run tests, if they built properly
    if [[ (-n "$BUILD_TESTS") && (-f "bin/dataTests") ]]; then
        bin/dataTests
    fi
)
