#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char response[BUFFER_SIZE];
    char *server_ip = NULL;

    // Vérifier si l'option -n est fournie
    if (argc == 3 && strcmp(argv[1], "-n") == 0) {
        server_ip = argv[2]; // Récupérer l'adresse IP fournie
    } else {
        fprintf(stderr, "Usage: %s -n <adresse_ip>\n", argv[0]);
        exit(1);
    }

    // Création du socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erreur de création du socket");
        exit(1);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Adresse IP invalide");
        exit(1);
    }

    // Connexion à l'agent
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de connexion");
        exit(1);
    }

    // Recevoir la réponse du serveur (nom d'hôte ou adresse IP)
    memset(response, 0, BUFFER_SIZE);
    int bytes_received = recv(client_socket, response, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        perror("Erreur de réception des données");
        close(client_socket);
        exit(1);
    }

    // Afficher la réponse reçue (adresse IP ou autre information)
    printf("Réponse du serveur : %s\n", response);

    // Fermeture du socket
    close(client_socket);
    return 0;
}
