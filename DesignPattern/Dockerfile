# Utilisez une image de base avec un compilateur C++ (par exemple, g++)
FROM gcc:latest

# Copiez les fichiers de votre projet dans le conteneur
COPY /home/mohamed/cpp_codes/DesignPattern  /app

# Définissez le répertoire de travail
WORKDIR  /app

# Compilez votre projet (ajustez les commandes en fonction de votre projet)
RUN g++ -o Factory_without_sonar.cpp

# Commande par défaut pour exécuter votre programme (à adapter)
CMD ["./Factory_without_sonar"]
