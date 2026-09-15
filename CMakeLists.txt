cmake_minimum_required(VERSION 3.14)

# Fixes the FetchContent timestamp warning globally
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

project(BootDevLotteryGenerator)

# -------------------------------------------------------------------
# DEPENDENCIES: FetchContent Management
# -------------------------------------------------------------------
include(FetchContent)

# 1. libcurl (Networking)
# Optimization flags to speed up curl compilation
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "") # Force a static, fast-loading library
set(BUILD_DOCS OFF CACHE INTERNAL "") # Turn off all documentation and manuals completely
set(ENABLE_CURL_MANUAL OFF CACHE INTERNAL "") # Stop CMake from building massive, built-in offline user manual tool inside the library's code blocks
set(USE_LIBIDN2 OFF CACHE INTERNAL "") # Disable IDN2 support completely
set(CURL_USE_LIBPSL OFF CACHE INTERNAL "") # Disable libpsl to prevent error when linking

# SSL Backend Selection (Critical for Cross-Platform Support)
if(WIN32)
  set(CURL_USE_SCHANNEL ON CACHE INTERNAL "") # Windows Secure Channel
elseif(APPLE)
  set(CURL_USE_SECTRANSP ON CACHE INTERNAL "") # macOS Secure Transport
else()
  set(CURL_USE_OPENSSL ON CACHE INTERNAL "") # Linux default (Requires libssl-dev on host)
endif()

# Disable optional compression and HTTP/2 protocols to keep build self-contained
set(CURL_BROTLI OFF CACHE INTERNAL "") # Prevents 'library brotlidec not found' error
set(CURL_ZSTD OFF CACHE INTERNAL "") # Disables Zstd compression dependency
set(USE_NGHTTP2 OFF CACHE INTERNAL "") # Disables HTTP/2 dependency

FetchContent_Declare(
  curl
  URL https://curl.se/download/curl-8.22.0.tar.xz
  URL_HASH SHA256=f7ef3ae8a22e521f289803fe93543eb64c329b58aa73a9e224dfd915a2a5f4f7
)
FetchContent_MakeAvailable(curl)

# 2. nlohmann/json (Config Parsing)
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
)
FetchContent_MakeAvailable(nlohmann_json)

# -------------------------------------------------------------------
# SYSTEM DEPENDENCIES: Lexbor (HTML Parsing) - Self-Contained
# -------------------------------------------------------------------
add_subdirectory(third_party/lexbor)

# -------------------------------------------------------------------
# TARGET CONFIGURATION
# -------------------------------------------------------------------
add_executable(lottery_generator
  main.cpp
  src/DataStore.cpp
  src/NetworkClient.cpp
  src/Scraper.cpp
  src/LexborDocument.cpp
  src/LexborCollection.cpp
  src/LexborSelector.cpp
  src/MetricsEngine.cpp
  src/ConsoleReporter.cpp
)

# Enforce C++17 strictly for this target
target_compile_features(lottery_generator PRIVATE cxx_std_17)

# Include Directories (Cross-Platform Safe)
target_include_directories(lottery_generator PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link Libraries (Using Imported Targets for Path Safety)
target_link_libraries(lottery_generator PRIVATE
  nlohmann_json::nlohmann_json
  # Link to the self-contained Lexbor build
  lexbor
)

# Link curl safely (handles different curl target naming conventions)
if(TARGET curl::curl)
  target_link_libraries(lottery_generator PRIVATE curl::curl)
elseif(TARGET CURL::libcurl)
  target_link_libraries(lottery_generator PRIVATE CURL::libcurl)
else()
  # Fallback to raw library variable if targets aren't exported
  target_link_libraries(lottery_generator PRIVATE ${CURL_LIBRARIES})
  target_include_directories(lottery_generator PRIVATE ${CURL_INCLUDE_DIRS})
endif()

# Link OS-specific network/crypto libraries required for static curl builds
if(WIN32)
  target_link_libraries(lottery_generator PRIVATE ws2_32 crypt32 advapi32 normaliz)
elseif(APPLE)
  target_link_libraries(lottery_generator PRIVATE "-framework CoreFoundation" "-framework Security")
endif()

# Ensure games.json is available in the build directory at runtime
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/games.json DESTINATION ${CMAKE_BINARY_DIR})
# Fix for Asset Copying: Ensure games.json copies on every compilation step
add_custom_command(
  TARGET lottery_generator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${CMAKE_CURRENT_SOURCE_DIR}/games.json"
  "${CMAKE_BINARY_DIR}/games.json"
  COMMENT "Syncing games.json to the output build directory..."
)
