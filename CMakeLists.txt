cmake_minimum_required(VERSION 3.14)

# Your explicit project name matching your GitHub repository
project(BootDevLotteryGenerator)

# Enable modern C++17 rules for your Windows and Mac environments
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------
# DEPENDENCY 1: HTML Parser Library
# -------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  HtmlParser
  GIT_REPOSITORY "https://github.com/ImArjunJ/HtmlParser.git"
  GIT_TAG        "master"
)
FetchContent_MakeAvailable(HtmlParser)

# -------------------------------------------------------------------
# DEPENDENCY 2: libcurl Networking Library (HTTPS Only optimization)
# -------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")  # Force a static, fast-loading library
set(BUILD_LIBCURL_DOCS OFF CACHE INTERNAL "") # Don't waste time downloading user manuals
set(BUILD_MISC_DOCS OFF CACHE INTERNAL "")    # Disable compilation of miscellaneous documentation files
set(ENABLE_CURL_MANUAL OFF CACHE INTERNAL "") # Stop CMake from building massive, built-in offline user manual tool inside the library's code blocks

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

# FIX: Tell the compiler to automatically look for header files inside your include folder
include_directories(include)

# Define your main executable targeting system
add_executable(lottery_generator
  main.cpp
  src/DataStore.cpp
  src/NetworkClient.cpp
)

# Link the downloaded parser tool using the official namespace structure
target_link_libraries(lottery_generator PRIVATE HtmlParser::HtmlParser libcurl)
