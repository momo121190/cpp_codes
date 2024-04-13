cmake_minimum_required(VERSION 2.8.12)
project(cpp_codes)

add_definitions("-std=c++11")

 
# Inclure le fichier conan_toolchain.cmake
include(conan_toolchain.cmake)
set(options TARGETS NO_OUTPUT_DIRS SKIP_RPATH KEEP_RPATHS SKIP_STD SKIP_FPIC)
find_package(PahoMqttCpp)
 
if(PahoMqttCpp_FOUND)
    add_executable(async_publish async_publish.cpp)
    include_directories(${PahoMqttCpp_INCLUDE_DIRS})
  target_link_libraries(async_publish PahoMqttCpp::paho-mqttpp3-static)
 
else()
    message(FATAL_ERROR "PahoMqttCpp not found")
endif()



 
 
