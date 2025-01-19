#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


void show_interface_addresses(const char *ifname) {
    struct ifaddrs *ifaddr, *ifa;
    char addr[INET6_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Adresses pour l'interface %s :\n", ifname);

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, ifname) != 0)
            continue;

        // IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), addr, INET_ADDRSTRLEN);
            printf("  IPv4 : %s\n", addr);
        }
        // IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa = (struct sockaddr_in6 *)ifa->ifa_addr;
            inet_ntop(AF_INET6, &(sa->sin6_addr), addr, INET6_ADDRSTRLEN);
            printf("  IPv6 : %s\n", addr);
        }
    }

    freeifaddrs(ifaddr);
}


void show_all_interfaces() {
    struct ifaddrs *ifaddr, *ifa;
    char addr[INET6_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Liste des interfaces réseau et leurs adresses :\n");

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        printf("Interface : %s\n", ifa->ifa_name);

        // IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), addr, INET_ADDRSTRLEN);
            printf("  IPv4 : %s\n", addr);
        }
        // IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa = (struct sockaddr_in6 *)ifa->ifa_addr;
            inet_ntop(AF_INET6, &(sa->sin6_addr), addr, INET6_ADDRSTRLEN);
            printf("  IPv6 : %s\n", addr);
        }
    }

    freeifaddrs(ifaddr);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage : %s [-i ifname | -a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-i") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Erreur : Vous devez spécifier un nom d'interface avec -i.\n");
            exit(EXIT_FAILURE);
        }
        show_interface_addresses(argv[2]);
    } else if (strcmp(argv[1], "-a") == 0) {
        show_all_interfaces();
    } else {
        fprintf(stderr, "Option invalide : %s\n", argv[1]);
        fprintf(stderr, "Usage : %s [-i ifname | -a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
