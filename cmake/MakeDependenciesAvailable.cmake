set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(cmake/LoadSQLiteCpp.cmake)
include(cmake/LoadBoost.cmake)
include(cmake/LoadEigen.cmake)
include(cmake/LoadSpdlog.cmake)

FetchContent_MakeAvailable(Eigen SQLiteCpp Boost spdlog)