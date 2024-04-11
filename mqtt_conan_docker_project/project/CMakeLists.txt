cmake_minimum_required(VERSION 2.8.12)
project(cpp_codes)

add_definitions("-std=c++11")

 
# Inclure le fichier conan_toolchain.cmake
include(conan_toolchain.cmake)
set(options TARGETS NO_OUTPUT_DIRS SKIP_RPATH KEEP_RPATHS SKIP_STD SKIP_FPIC)
find_package(PahoMqttCpp REQUIRED)
 
if(PahoMqttCpp_FOUND)
   #add_executable(openssl_random_generator openssl_random_generator.cpp)
   # include_directories(${OPENSSL_INCLUDE_DIRS})
 #  target_link_libraries(YOUR_TARGET PahoMqttCpp::paho-mqttpp3-static)
else()
    message(FATAL_ERROR "PahoMqttCpp not found")
endif()



 
