cmake_minimum_required(VERSION 2.8.12)
project(cpp_codes)

add_definitions("-std=c++11")

 
# Inclure le fichier conan_toolchain.cmake
include(conan_toolchain.cmake)
find_package("OpenSSL")

if(OpenSSL_FOUND)
   add_executable(openssl_random_generator openssl_random_generator.cpp)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    target_link_libraries(enough ${OPENSSL_LIBRARIES})
else()
    message(FATAL_ERROR "OPENSSL not found")
endif()




