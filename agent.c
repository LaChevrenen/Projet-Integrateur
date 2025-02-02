#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define PORT 5000
#define MAX_BUFFER_SIZE 2048



// Fonction pour afficher les adresses IPv4 et IPv6 d'une interface réseau spécifique
char* show_interface_addresses(const char *ifname) {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];
    static char buffer[MAX_BUFFER_SIZE];  // Buffer statique pour renvoyer le résultat
    int offset;

    // Obtenir la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE);
    }

    // Initialiser le buffer
    offset = snprintf(buffer, MAX_BUFFER_SIZE, "Adresses associées à l'interface : %s\n", ifname);

    // Parcourir la liste des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Vérifier que l'interface possède une adresse
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        // Vérifier si le nom de l'interface correspond à celui demandé
        if (strcmp(ifa->ifa_name, ifname) != 0) {
            continue;
        }

        // Vérifier si l'adresse est de type IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
            struct sockaddr_in *ipv4_netmask = (struct sockaddr_in *)ifa->ifa_netmask; // Masque IPv4

            inet_ntop(AF_INET, &(ipv4->sin_addr), address, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ipv4_netmask->sin_addr), netmask, INET_ADDRSTRLEN);

            // Calculer le masque en CIDR pour IPv4
            unsigned int netmask_bits = 0;
            unsigned int mask = ipv4_netmask->sin_addr.s_addr;
            while (mask) {
                netmask_bits += mask & 1;
                mask >>= 1;
            }

            // Ajouter l'adresse IPv4 et le CIDR au buffer
            offset += snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "  Adresse IPv4 : %s/%u\n", address, netmask_bits);
        }
        // Vérifier si l'adresse est de type IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            struct sockaddr_in6 *ipv6_netmask = (struct sockaddr_in6 *)ifa->ifa_netmask; // Masque IPv6

            inet_ntop(AF_INET6, &(ipv6->sin6_addr), address, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(ipv6_netmask->sin6_addr), netmask, INET6_ADDRSTRLEN);

            // Calculer le préfixe CIDR pour IPv6
            unsigned int netmask_bits = 0;
            unsigned int *mask = (unsigned int *)&ipv6_netmask->sin6_addr;
            for (int i = 0; i < 4; i++) {
                unsigned int m = mask[i];
                while (m) {
                    netmask_bits += m & 1;
                    m >>= 1;
                }
            }

            // Ajouter l'adresse IPv6 et le CIDR au buffer
            offset += snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "  Adresse IPv6 : %s/%u\n", address, netmask_bits);
        }
    }

    freeifaddrs(ifaddr);
    snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "Fin de la liste des adresses pour l'interface : %s\n", ifname);

    return buffer;
}

// Fonction pour afficher toutes les interfaces réseau et leurs adresses associées
char* show_all_interfaces() {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];
    static char buffer[MAX_BUFFER_SIZE];  // Buffer statique pour renvoyer le résultat
    int offset;

    // Récupérer la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE);
    }

    // Initialiser le buffer
    offset = snprintf(buffer, MAX_BUFFER_SIZE, "Liste des interfaces réseau et leurs adresses associées :\n");

    // Parcourir la liste des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        // Ajouter l'interface au buffer
        offset += snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "\nInterface : %s\n", ifa->ifa_name);

        // Vérifier si l'adresse est de type IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
            struct sockaddr_in *ipv4_netmask = (struct sockaddr_in *)ifa->ifa_netmask; // Masque IPv4

            inet_ntop(AF_INET, &(ipv4->sin_addr), address, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(ipv4_netmask->sin_addr), netmask, INET_ADDRSTRLEN);

            unsigned int netmask_bits = 0;
            unsigned int mask = ipv4_netmask->sin_addr.s_addr;
            while (mask) {
                netmask_bits += mask & 1;
                mask >>= 1;
            }

            // Ajouter l'adresse IPv4 et le CIDR au buffer
            offset += snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "  Adresse IPv4 : %s/%u\n", address, netmask_bits);
        }
        // Vérifier si l'adresse est de type IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            struct sockaddr_in6 *ipv6_netmask = (struct sockaddr_in6 *)ifa->ifa_netmask; // Masque IPv6

            inet_ntop(AF_INET6, &(ipv6->sin6_addr), address, INET6_ADDRSTRLEN);
            inet_ntop(AF_INET6, &(ipv6_netmask->sin6_addr), netmask, INET6_ADDRSTRLEN);

            unsigned int netmask_bits = 0;
            unsigned int *mask = (unsigned int *)&ipv6_netmask->sin6_addr;
            for (int i = 0; i < 4; i++) {
                unsigned int m = mask[i];
                while (m) {
                    netmask_bits += m & 1;
                    m >>= 1;
                }
            }

            // Ajouter l'adresse IPv6 et le CIDR au buffer
            offset += snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "  Adresse IPv6 : %s/%u\n", address, netmask_bits);
        }
    }

    freeifaddrs(ifaddr);
    snprintf(buffer + offset, MAX_BUFFER_SIZE - offset, "\nFin de la liste des interfaces réseau.\n");

    return buffer;
}


int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

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

    //printf("Agent en écoute sur le port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == -1) {
            perror("Erreur d'accept");
            continue;
        }

        //printf("Connexion établie avec un client.\n");

        // Lire la commande envoyée par le client
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            perror("Erreur de réception des données");
            close(client_socket);
            continue;
        }

        // Vérifie si l'option est -i ou -a
        if (strncmp(buffer, "-i", 2) == 0) {
            // Récupérer le nom de l'interface et envoyer les adresses
            char *interface_name = buffer + 3;  // On suppose que le nom de l'interface suit l'option -i
            char *result = show_interface_addresses(interface_name);
            send(client_socket, result, strlen(result), 0);
        } else if (strncmp(buffer, "-a", 2) == 0) {
            // Envoyer toutes les interfaces
            char *result = show_all_interfaces();
            send(client_socket, result, strlen(result), 0);
        } else {
            // Option invalide
            const char *error_message = "Erreur : Option invalide.\n";
            send(client_socket, error_message, strlen(error_message), 0);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}