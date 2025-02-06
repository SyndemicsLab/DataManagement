message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG 1.87.0
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")