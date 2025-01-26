#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        perror("Erreur lors de la réception de données du client");
        close(client_socket);
        return;
    }
    buffer[bytes_read] = '\0';

    printf("Requête reçue : %s\n", buffer);

    if (strcmp(buffer, "-a") == 0) {
        // Appelez la fonction pour collecter les informations sur toutes les interfaces
        char response[BUFFER_SIZE] = "Liste des interfaces réseau et leurs adresses ...";
        send(client_socket, response, strlen(response), 0);
    } else if (strncmp(buffer, "-i", 2) == 0) {
        // Extraire le nom de l'interface et renvoyer les informations
        char response[BUFFER_SIZE] = "Détails pour une interface spécifique ...";
        send(client_socket, response, strlen(response), 0);
    } else {
        char response[BUFFER_SIZE] = "Option non reconnue";
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Crée une socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind la socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors du bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Écoute sur la socket
    if (listen(server_socket, 5) == -1) {
        perror("Erreur lors de l'écoute");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Serveur en écoute sur le port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Erreur lors de l'acceptation du client");
            continue;
        }

        printf("Connexion acceptée d'un client\n");
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
