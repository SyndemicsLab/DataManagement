set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(cmake/LoadSQLiteCpp.cmake)
include(cmake/LoadBoost.cmake)
include(cmake/LoadEigen.cmake)
include(cmake/LoadSpdlog.cmake)

if(DATAMANAGEMENT_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(Eigen SQLiteCpp Boost spdlog)
elseif(DATAMANAGEMENT_BUILD_TESTS STREQUAL "ON")
    include(cmake/LoadGtest.cmake)
    FetchContent_MakeAvailable(Eigen SQLiteCpp Boost spdlog googletest)
    include(GoogleTest)
endif()
