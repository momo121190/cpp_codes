cmake_minimum_required(VERSION 2.8.12)
project(cpp_codes)

add_definitions("-std=c++11")

include(conan_toolchain.cmake)
set(options TARGETS NO_OUTPUT_DIRS SKIP_RPATH KEEP_RPATHS SKIP_STD SKIP_FPIC)


 add_executable(pointer_overloading pointer_overloading.cpp)
 target_link_libraries(pointer_overloading ${CONAN_LIBS})
