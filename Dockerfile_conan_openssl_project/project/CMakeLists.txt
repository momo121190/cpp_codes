cmake_minimum_required(VERSION 2.8.12)
project(cpp_codes)

add_definitions("-std=c++11")

# Inclure le chemin d'inclusion pour OpenSSL
include_directories(${CONAN_INCLUDE_DIRS})

# Inclure le fichier conan_toolchain.cmake
include(conan_toolchain.cmake)

set(options TARGETS NO_OUTPUT_DIRS SKIP_RPATH KEEP_RPATHS SKIP_STD SKIP_FPIC)

add_executable(openssl_random_generator openssl_random_generator.cpp)

# Lier les bibliothèques Conan à votre exécutable
target_link_libraries(openssl_random_generator ${CONAN_LIBS})

