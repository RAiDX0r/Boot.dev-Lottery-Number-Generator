# Fixes the FetchContent timestamp warning globally
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# Fetch Lexbor v3.0.1 (Latest Stable Release)
FetchContent_Declare(
  lexbor
  URL https://github.com/lexbor/lexbor/archive/refs/tags/v3.0.1.tar.gz
  URL_HASH SHA256=08ce3d18efdd09b8b3488779b97509f83cc181e09a5a4cd8162ac08d77266600
)

# Disable Lexbor's documentation and tests to speed up build
set(BUILD_DOCS OFF CACHE INTERNAL "")
set(BUILD_TESTS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(lexbor)

# Ensure Lexbor is built as a static library with position-independent code
set_target_properties(lexbor PROPERTIES POSITION_INDEPENDENT_CODE ON)
