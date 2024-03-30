# Utilisez une image de base avec un compilateur C++ (par exemple, g++)
FROM gcc:latest

# Créez un répertoire /app à l'intérieur du conteneur (s'il n'existe pas déjà)
RUN mkdir /app

# Copiez les fichiers de votre projet dans le conteneur
COPY . /app

# Définissez le répertoire de travail
WORKDIR /app

# Compilez votre projet (ajustez les commandes en fonction de votre projet)
RUN g++ -o mon_programme Factory_without_sonar.cpp

# Commande par défaut pour exécuter votre programme (à adapter)
CMD ["./mon_programme"]
