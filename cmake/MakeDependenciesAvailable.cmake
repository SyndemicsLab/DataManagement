set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE "ALWAYS")

include(FetchContent)

include(cmake/LoadBoost.cmake)

FetchContent_MakeAvailable(Boost)