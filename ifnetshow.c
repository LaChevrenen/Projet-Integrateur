#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char *server_ip = NULL;
    char *interface_name = NULL;
    int option_found = 0;

    // Vérifier que les arguments sont valides
    if (argc < 4) {
        fprintf(stderr, "Usage: %s -n <adresse_ip> -i <interface_name> | -a\n", argv[0]);
        exit(1);
    }

    // Analyser les arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            server_ip = argv[i + 1];
            i++; // Passer à l'argument suivant
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            interface_name = argv[i + 1];
            i++; // Passer à l'argument suivant
            option_found = 1;
        }
        else if (strcmp(argv[i], "-a") == 0) {
            option_found = 2;
        }
    }

    if (!server_ip) {
        fprintf(stderr, "Erreur : Vous devez spécifier une adresse IP avec l'option -n.\n");
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

    // Connexion au serveur
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de connexion");
        exit(1);
    }

    // Construire la commande à envoyer au serveur
    if (option_found == 1) {
        snprintf(buffer, sizeof(buffer), "-i %s", interface_name);
    } else if (option_found == 2) {
        snprintf(buffer, sizeof(buffer), "-a");
    } else {
        fprintf(stderr, "Erreur : Option -i ou -a attendue.\n");
        exit(1);
    }

    // Envoi de la commande au serveur
    send(client_socket, buffer, strlen(buffer), 0);

    // Réception de la réponse du serveur
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        perror("Erreur de réception des données");
        close(client_socket);
        exit(1);
    }

    // Affichage de la réponse du serveur
    //printf("Réponse du serveur : \n%s\n", buffer);

    // Fermeture du socket
    close(client_socket);
    return 0;
}
