# Utiliser l'image de base Ubuntu 22.04
FROM ubuntu:22.04
USER root
# Mettre à jour le système et installer les dépendances nécessaires
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    python3 \
    git \
    wget \
    python3-pip\
    make \
    build-essential

# Installer Conan à l'aide de pip
RUN pip3 install conan

 

# Cloner le code source de l'application
RUN git clone -b master https://github.com/momo121190/cpp_codes.git
COPY . /cpp_codes/Dockerfile_conan_openssl_project/
# Aller dans le répertoire cpp_codes
WORKDIR /cpp_codes 
#COPY conanfile.txt /cpp_codes/Dockerfile_conan_openssl_project/project

COPY CMakeLists.txt /cpp_codes/Dockerfile_conan_openssl_project/
# Exécuter ls -l pour afficher le contenu du répertoire /app
 

 
RUN rm -rf  conanfile.py
RUN conan profile detect
RUN rm -rf build
RUN mkdir build && cd build &&   pwd&& conan install .. --build=missing
#COPY ./conan_toolchain.cmake  /cpp_codes/Dockerfile_conan_openssl_project/
RUN search_result=$(find / -name "lib*.a" -o -name "lib*.so" -o -name "*.h")

# Afficher le résultat
RUN echo "$search_result"
#RUN cmake /cpp_codes/Dockerfile_conan_openssl_project -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
#RUN cmake --build .
 
