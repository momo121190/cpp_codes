cmake_minimum_required(VERSION 3.22.1)
project(cpp_codes)

add_definitions("-std=c++11")

 
# Inclure le fichier conan_toolchain.cmake
include(conan_toolchain.cmake)
set(options TARGETS NO_OUTPUT_DIRS SKIP_RPATH KEEP_RPATHS SKIP_STD SKIP_FPIC)
find_package(PahoMqttCpp)
 find_package(ZLIB)
 find_package(OpenSSL)
 if(ZLIB_FOUND)
        message(STATUS "ZLIB found.")
    else()
        message(WARNING "ZLIB not found.")
    endif()
  if(OpenSSL_FOUND)
        message(STATUS "OpenSSL found.")
    else()
        message(WARNING "OpenSSL not found.")
    endif()
find_package(PkgConfig REQUIRED)
pkg_check_modules(PAHO_MQTT_CPP REQUIRED IMPORTED_TARGET paho-mqtt-cpp)
if(PahoMqttCpp_FOUND)
include_directories(${PahoMqttCpp_INCLUDE_DIRS})
    include_directories(${OPENSSL_INCLUDE_DIRS})
        include_directories(${ZLIB_INCLUDE_DIRS})
    add_executable(async_publish async_publish.cpp)
   
  target_link_libraries(async_publish ${ZLIB_LIBRARIES} ${OPENSSL_LIBRARIES} PahoMqttCpp::paho-mqttpp3-static ${CONAN_LIBS}  PRIVATE PkgConfig::PAHO_MQTT_CPP)

#else()
 #   message(FATAL_ERROR "PahoMqttCpp not found")
#endif()



 
