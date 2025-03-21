set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(cmake/LoadBoost.cmake)

set(BUILD_TESTING OFF)
FetchContent_MakeAvailable(Boost)
unset(BUILD_TESTING)