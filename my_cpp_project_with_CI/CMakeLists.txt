cmake_minimum_required(VERSION 3.10)
project(my_project)

set(CMAKE_CXX_STANDARD 17)  # Set C++ version
add_executable(my_project src/main.cpp)  # Add an executable

# Include the header directories
target_include_directories(my_project PUBLIC ${PROJECT_SOURCE_DIR}/include)
