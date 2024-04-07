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

# Aller dans le répertoire cpp_codes
WORKDIR /cpp_codes/Dockerfile_conan_openssl_project
#RUN conan profile detect --name myprofile
RUN rm -rf  conanfile.py
RUN conan profile detect
RUN rm -rf build
RUN mkdir build && cd build &&   pwd&& conan install ..
 
RUN cmake /cpp_codes/ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
RUN cmake --build .
 
