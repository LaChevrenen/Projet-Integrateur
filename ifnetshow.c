#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Modifier avec l'IP de l'agent
#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char hostname[BUFFER_SIZE];

    // Création du socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erreur de création du socket");
        exit(1);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        exit(1);
    }

    // Connexion à l'agent
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de connexion");
        exit(1);
    }

    // Recevoir le hostname
    memset(hostname, 0, BUFFER_SIZE);
    recv(client_socket, hostname, BUFFER_SIZE, 0);
    printf("Nom d'hôte reçu : %s\n", hostname);

    close(client_socket);
    return 0;
}
