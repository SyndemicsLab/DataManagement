message(CHECK_START "Searching for Boost...")
find_package(Boost)

if( NOT Boost_FOUND )
    include(FetchContent)
    message(CHECK_START "Boost Not Found, Fetching...")
    list(APPEND CMAKE_MESSAGE_INDENT "  ")

    FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG 1.87.0
    )
    FetchContent_MakeAvailable(Boost)
    list(POP_BACK CMAKE_MESSAGE_INDENT)
    message(CHECK_PASS "Boost Fetched")
else()
    message(CHECK_PASS "Boost Found")
endif()