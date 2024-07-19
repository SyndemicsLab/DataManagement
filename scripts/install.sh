#!/bin/bash -l
if command -v module &>/dev/null; then
    module load gcc/12.2.0
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build DataManagement"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|-b]"
    echo "h              Print this help screen."
    echo "p              Build and run tests."
    echo "i              Set the default build directory"
    echo "               Default: \`dminstall\`"
}

# set default build type
BUILDTYPE="Release"
PREFIX="dminstall"
BUILD_TESTS=""

# process optional command line flags
while getopts ":hpi:" option; do
    case $option in
        h)
            showhelp
            exit
            ;;
	i)
	    if [[ -z "$OPTARG" ]]; then
		echo "Install folder name cannot be empty!"
		exit 1
	    fi
	    PREFIX="$OPTARG"
	    ;;
        p)
            BUILD_TESTS="ON"
	    BUILDOPT="-DBUILD_DATA_TESTS=ON"
	    BUILDTYPE="Debug"
            ;;
        \?)
            echo "Error: Invalid option flag provided!"
            showhelp
            exit
            ;;
    esac
done

(
    # change to the top-level git folder
    TOPLEVEL="$(git rev-parse --show-toplevel)"
    cd "$TOPLEVEL" || exit

    # clean the build folder
    rm -rf build/*
    BUILDCOMMAND="cmake -S . -B build -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=$BUILDTYPE $BUILDOPT"
    echo "$BUILDCOMMAND"
    $BUILDCOMMAND
    cmake --build build
    cmake --install build --prefix "$PREFIX"
    if [[ -n "$BUILD_TESTS" && -f "bin/dataTests" ]]; then
	bin/dataTests
    fi
)
