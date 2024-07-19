#!/usr/bin/bash -l
# only execute these lines if the `module` command is present in the environment
# used for the BU SCC
if command -v module &>/dev/null; then
    module load gcc/12.2.0
    module load miniconda
    conda env create -f "/projectnb/respond/environment.yml" 2&>/dev/null
    conda activate respond
fi

# help message to be output either with the -h flag or when using invalid syntax
showhelp () {
    printf "\033[31m%s\033[0m" "$0 - Build the DataManagement Model"
    echo
    echo
    echo "Syntax: $(basename "$0") [-h|-t OPTION|-p|-l|-n]"
    echo "h              Print this help screen."
    echo "t OPTION       Set the build type to OPTION"
    echo "               Options: [Debug|Release]"
    echo "               Default: Debug"
    echo "p              Build and run tests."
    echo "l              Build Shared Library."
    echo "n              Build Benchmarking executable"
}

# set default build type
BUILDTYPE="Release"
BUILD_TESTS=""
BUILD_BENCHMARK=""
BUILD_SHARED_LIBRARY=""
BUILD_STATIC_LIBRARY="ON"

# process optional command line flags
while getopts ":hnptal:" option; do
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
            BUILD_SHARED_LIBRARY="ON"
            BUILD_STATIC_LIBRARY="OFF"
            ;;
        n)
            BUILD_BENCHMARK="ON"
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

    # ensure the `build/` directory exists
    ([[ -d "build/" ]] && rm -rf build/*) || mkdir "build/"

    (
        cd "build" || exit
        # build tests, if specified
        CMAKE_COMMAND="cmake .. -DCMAKE_PREFIX_PATH=$CONDA_PREFIX -DCMAKE_BUILD_TYPE=$BUILDTYPE"
        if [[ -n "$BUILD_TESTS" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_TESTS=$BUILD_TESTS"
        fi
        # build benchmarking executable
        if [[ -n "$BUILD_BENCHMARK" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_BENCHMARK=$BUILD_BENCHMARK"
        fi
        # build static libraries
        if [[ -n "$BUILD_STATIC_LIBRARY" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_STATIC_LIBRARY=$BUILD_STATIC_LIBRARY"
        fi
        # build shared libraries
        if [[ -n "$BUILD_SHARED_LIBRARY" ]]; then
            CMAKE_COMMAND="$CMAKE_COMMAND -DBUILD_SHARED_LIBRARY=$BUILD_SHARED_LIBRARY"
        fi

        # run the full build command as specified
        $CMAKE_COMMAND
        (
            # determine the number of processing units available
            CORES="$(nproc --all)"
            # if CORES > 1 compile in parallel where possible
            ([[ -n "$CORES" ]] && cmake --build . -j"$CORES" --target install) || cmake --build .  --target install
        )
        # run tests, if they built properly
    )
    if [[ (-n "$BUILD_TESTS") && (-f "bin/dataTests") ]]; then
        bin/dataTests
    fi
)
