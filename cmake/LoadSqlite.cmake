message(CHECK_START "Searching for SQLite3...")
find_package(SQLite3)
if( NOT SQLite3_FOUND )
    message(FATAL_ERROR "SQLite3 Not Found! Exiting...")
    # include(FetchContent)
    # message(CHECK_START "SQLite3 Not Found, Fetching...")
    # list(APPEND CMAKE_MESSAGE_INDENT "  ")
    # FetchContent_Declare(
    #         SQLite3
    #         GIT_REPOSITORY  https://github.com/gabime/spdlog.git
    #         GIT_TAG         v1.x
    #         GIT_PROGRESS    TRUE
    # )
    # FetchContent_MakeAvailable(spdlog)
    # list(POP_BACK CMAKE_MESSAGE_INDENT)
    # message(CHECK_PASS "SQLite3 Fetched")
else()
    message(CHECK_PASS "SQLite3 Found")
endif()