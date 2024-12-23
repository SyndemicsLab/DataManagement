include(FetchContent)
message(CHECK_START "Fetching Glaze...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Glaze Fetched")