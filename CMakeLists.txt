cmake_minimum_required(VERSION 3.14)

# Your explicit project name matching your GitHub repository
project(BootDevLotteryGenerator)

# Enable modern C++17 rules for your Windows and Mac environments
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake to download the parser library from the direct GitHub repository link
include(FetchContent)
FetchContent_Declare(
  HtmlParser
  GIT_REPOSITORY "https://github.com/ImArjunJ/HtmlParser.git"
  GIT_TAG        "master"
)
FetchContent_MakeAvailable(HtmlParser)

# FIX: Tell the compiler to automatically look for header files inside your include folder
include_directories(include)

# Define your main executable targeting system
add_executable(lottery_generator main.cpp)

# Link the downloaded parser tool using the official namespace structure
target_link_libraries(lottery_generator PRIVATE HtmlParser::HtmlParser)
