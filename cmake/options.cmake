option(DATAMANAGEMENT_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(DATAMANAGEMENT_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# testing options
option(DATAMANAGEMENT_BUILD_TESTS "Build tests" ON)

# bench options
option(DATAMANAGEMENT_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# warning options
option(DATAMANAGEMENT_BUILD_WARNINGS "Enable compiler warnings" OFF)

# install options
option(DATAMANAGEMENT_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(DATAMANAGEMENT_INSTALL "Generate the install target" ${DATAMANAGEMENT_MASTER_PROJECT})
option(DATAMANAGEMENT_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)
