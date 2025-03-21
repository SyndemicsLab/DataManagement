message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
find_package(Boost REQUIRED)

# include(FetchContent)

# option(BOOST_SKIP_INSTALL_RULES NO)
# FetchContent_Declare(
#     Boost
#     URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.7z
#     FIND_PACKAGE_ARGS
#     DOWNLOAD_EXTRACT_TIMESTAMP 1
# )
# list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")