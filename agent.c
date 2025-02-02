#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

void send_hostname(int client_socket) {
    char hostname[BUFFER_SIZE];

    // Récupérer le hostname
    if (gethostname(hostname, BUFFER_SIZE) != 0) {
        perror("Erreur lors de la récupération du hostname");
        strcpy(hostname, "Unknown");
    }

    // Envoyer le hostname au client
    send(client_socket, hostname, strlen(hostname), 0);
    printf("Hostname envoyé : %s\n", hostname);

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Création du socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erreur de création du socket");
        exit(1);
    }

    // Configuration de l'adresse et du port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Liaison du socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de bind");
        exit(1);
    }

    // Mise en écoute
    if (listen(server_socket, 5) == -1) {
        perror("Erreur de listen");
        exit(1);
    }

    printf("Agent en écoute sur le port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == -1) {
            perror("Erreur d'accept");
            continue;
        }

        printf("Connexion établie avec un client.\n");
        send_hostname(client_socket);
    }

    close(server_socket);
    return 0;
}
