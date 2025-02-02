#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>

#define PORT 5000
#define BUFFER_SIZE 4096

void get_all_interfaces(char *buffer, size_t buffer_size) {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    int offset = 0;
    size_t remaining = buffer_size;

    buffer[0] = '\0';

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        strncpy(buffer, "Erreur lors de la récupération des interfaces", remaining);
        return;
    }

    offset += snprintf(buffer + offset, remaining - offset, "Liste des interfaces réseau:\n");

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;

        int family = ifa->ifa_addr->sa_family;
        if (family != AF_INET && family != AF_INET6) continue;

        offset += snprintf(buffer + offset, remaining - offset, "\nInterface: %s\n", ifa->ifa_name);
        if (offset >= remaining) break;

        void *addr;
        const char *family_str;
        int prefix_len = 0;

        if (family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
            addr = &(ipv4->sin_addr);
            family_str = "IPv4";

            struct sockaddr_in *netmask_in = (struct sockaddr_in *)ifa->ifa_netmask;
            unsigned int mask = netmask_in->sin_addr.s_addr;
            while (mask) {
                prefix_len += mask & 1;
                mask >>= 1;
            }
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            addr = &(ipv6->sin6_addr);
            family_str = "IPv6";

            struct sockaddr_in6 *netmask_in6 = (struct sockaddr_in6 *)ifa->ifa_netmask;
            unsigned int *mask = (unsigned int *)&netmask_in6->sin6_addr;
            for (int i = 0; i < 4; i++) {
                unsigned int m = mask[i];
                while (m) {
                    prefix_len += m & 1;
                    m >>= 1;
                }
            }
        }

        inet_ntop(family, addr, address, sizeof(address));
        offset += snprintf(buffer + offset, remaining - offset, "  Adresse %s: %s/%d\n", 
                         family_str, address, prefix_len);
        if (offset >= remaining) break;
    }

    offset += snprintf(buffer + offset, remaining - offset, "\nFin de la liste.\n");
    freeifaddrs(ifaddr);
}

void send_interfaces(int client_socket) {
    char buffer[BUFFER_SIZE];
    get_all_interfaces(buffer, BUFFER_SIZE);
    send(client_socket, buffer, strlen(buffer), 0);
    printf("Interfaces envoyées:\n%s\n", buffer);
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erreur de création du socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur de bind");
        exit(1);
    }

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
        send_interfaces(client_socket);
    }

    close(server_socket);
    return 0;
}