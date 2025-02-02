#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define PORT 5000
#define BUFFER_SIZE 1024

void send_interface_addresses(int client_socket, const char *ifname) {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];

    // Obtenir la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        perror("Erreur lors de l'appel à getifaddrs");
        return;
    }

    // Envoyer le titre de l'interface
    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header), "Adresses associées à l'interface : %s\n", ifname);
    send(client_socket, header, strlen(header), 0);

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

            // Envoi de l'adresse IPv4 et du CIDR
            char ipv4_msg[BUFFER_SIZE];
            snprintf(ipv4_msg, sizeof(ipv4_msg), "  Adresse IPv4 : %s/%u\n", address, netmask_bits);
            send(client_socket, ipv4_msg, strlen(ipv4_msg), 0);
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

            // Envoi de l'adresse IPv6 et du CIDR
            char ipv6_msg[BUFFER_SIZE];
            snprintf(ipv6_msg, sizeof(ipv6_msg), "  Adresse IPv6 : %s/%u\n", address, netmask_bits);
            send(client_socket, ipv6_msg, strlen(ipv6_msg), 0);
        }
    }

    freeifaddrs(ifaddr);
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
        send_interface_addresses(client_socket, "eth0");  // Remplacer par le nom de l'interface que tu veux
    }

    close(server_socket);
    return 0;
}
