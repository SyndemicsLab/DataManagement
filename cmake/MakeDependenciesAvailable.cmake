set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(LoadSQLiteCpp)
include(LoadBoost)
include(LoadEigen)
include(LoadSpdlog)

FetchContent_MakeAvailable(Eigen SQLiteCpp Boost spdlog)