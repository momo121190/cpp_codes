# Use the base image Ubuntu 22.04
FROM ubuntu:22.04
# Set user to root
USER root

# Update the system and install necessary tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    python3 \
    git \
    wget \
    python3-pip \
    make \
    build-essential

# Install Conan using pip
RUN pip3 install conan

# Clone the project source code from GitHub
RUN git clone -b main https://github.com/momo121190/cpp_codes.git
# Navigate to the project directory
WORKDIR /cpp_codes/
# Switch Git branch to "main"
RUN git checkout main
# Navigate to the subdirectory containing the project code
WORKDIR /cpp_codes/Dockerfile_conan_openssl_project

# Copy the CMakeLists.txt file into the project directory
COPY CMakeLists.txt /cpp_codes/Dockerfile_conan_openssl_project/

# Remove the conanfile.py if it exists
RUN rm -rf  conanfile.py
# Detect the Conan profile in use
RUN conan profile detect
# Remove the build directory if it exists
RUN rm -rf build
# Create the build directory and navigate to it
RUN mkdir build && cd build &&   pwd && conan install .. --build=missing

# Search for all .h files in the system and write them to result.txt
RUN search_result=$(find / -name "*.h") && echo "$search_result" > result.txt

# Configure CMake with the project directory and the build type
RUN cmake /cpp_codes/Dockerfile_conan_openssl_project -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release  
# Build the project with cmake
RUN cmake --build .

