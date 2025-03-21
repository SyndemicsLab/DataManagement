message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG c89e6267665516192015a9e40955e154466f4f68
    FIND_PACKAGE_ARGS
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")