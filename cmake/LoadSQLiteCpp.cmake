include(FetchContent)
message(CHECK_START "SQLiteCpp Not Found, Fetching...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
FetchContent_Declare(
    sqlitecpp
    GIT_REPOSITORY  https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG         master
    GIT_PROGRESS    TRUE
)
set(SQLITECPP_RUN_CPPCHECK "Don't check" OFF)
set(SQLITECPP_BUILD_TESTS "Don't build tests" OFF)
set(SQLITECPP_USE_GCOV "Don't apply coverage" OFF)
set(SQLITECPP_RUN_CPPLINT "Don't Lint" OFF)
set(SQLITECPP_USE_STATIC_RUNTIME "Don't use static runtime" OFF)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "SQLiteCpp Fetched")