message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.gz
    FIND_PACKAGE_ARGS
    DOWNLOAD_EXTRACT_TIMESTAMP 1
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")