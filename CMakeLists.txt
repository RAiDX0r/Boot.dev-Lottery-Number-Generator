cmake_minimum_required(VERSION 3.14)

# Your explicit project name matching your GitHub repository
project(BootDevLotteryGenerator)

# Enable modern C++17 rules for your Windows and Mac environments
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------
# DEPENDENCY 1: libcurl Networking Library (HTTPS Only optimization)
# -------------------------------------------------------------------
include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")  # Force a static, fast-loading library
set(BUILD_LIBCURL_DOCS OFF CACHE INTERNAL "") # Don't waste time downloading user manuals
set(BUILD_MISC_DOCS OFF CACHE INTERNAL "")    # Disable compilation of miscellaneous documentation files
set(ENABLE_CURL_MANUAL OFF CACHE INTERNAL "") # Stop CMake from building massive, built-in offline user manual tool inside the library's code blocks
set(USE_LIBIDN2 OFF CACHE INTERNAL "Disable IDN2 support completely")

if(WIN32)
  set(CURL_USE_SCHANNEL ON CACHE INTERNAL "") # Windows Secure Provider
else()
  set(CURL_USE_SECTRANSP ON CACHE INTERNAL "") # Mac Secure Provider
endif()

FetchContent_Declare(
  curl
  GIT_REPOSITORY "https://github.com/curl/curl.git"
  GIT_TAG        "curl-8_11_1"
)
FetchContent_MakeAvailable(curl)

# -------------------------------------------------------------------
# TARGET CONFIGURATION: Main Executable & Dependency Mapping
# -------------------------------------------------------------------
# Define your main executable targeting system
add_executable(lottery_generator
  main.cpp
  src/DataStore.cpp
  src/NetworkClient.cpp
  src/Scraper.cpp
  src/LexborDocument.cpp
  src/LexborCollection.cpp
  src/LexborSelector.cpp
  src/MetricsEngine.cpp
)

# Modern C++ Target-Based Directory Ingestion
target_include_directories(lottery_generator PRIVATE 
  include
  /opt/homebrew/include
)

# Modern C++ Target-Based Linker Mapping
target_link_directories(lottery_generator PRIVATE 
  /opt/homebrew/lib
)

# Link the system-installed lexbor tool and fetched libcurl safely
target_link_libraries(lottery_generator PRIVATE 
  lexbor 
  libcurl
)
