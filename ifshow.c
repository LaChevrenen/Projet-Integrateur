#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

// Fonction pour afficher les adresses IPv4 et IPv6 d'une interface réseau spécifique
void show_interface_addresses(const char *ifname) {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];

    // Obtenir la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Adresses associées à l'interface : %s\n", ifname);

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

            printf("  Adresse IPv4 : %s/%u\n", address, netmask_bits);
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

            printf("  Adresse IPv6 : %s/%u\n", address, netmask_bits);
        }
    }

    freeifaddrs(ifaddr);
    printf("Fin de la liste des adresses pour l'interface : %s\n", ifname);
}

// Fonction pour afficher toutes les interfaces réseau et leurs adresses associées
void show_all_interfaces() {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;
    char address[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];

    // Récupérer la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Liste des interfaces réseau et leurs adresses associées :\n");

    // Parcourir la liste des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        printf("\nInterface : %s\n", ifa->ifa_name);

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

            printf("  Adresse IPv4 : %s/%u\n", address, netmask_bits);
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

            printf("  Adresse IPv6 : %s/%u\n", address, netmask_bits);
        }
    }

    freeifaddrs(ifaddr);
    printf("\nFin de la liste des interfaces réseau.\n");
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Erreur : Aucun argument fourni.\n");
        exit(EXIT_FAILURE);
    }

    // Vérifie si l'option fournie est "-i"
    if (strcmp(argv[1], "-i") == 0) {
        // Vérifie qu'un nom d'interface a été spécifié après "-i"
        if (argc < 3) {
            fprintf(stderr, "Erreur : Vous devez spécifier un nom d'interface avec l'option -i.\n");
            exit(EXIT_FAILURE);
        }
        const char *interface_name = argv[2];
        // On affiche toutes les adresses de l'interface choisie
        show_interface_addresses(interface_name);
    }
    // Vérifie si l'option fournie est "-a"
    else if (strcmp(argv[1], "-a") == 0) {
        // On affiche toutes les interfaces et leurs adresses
        show_all_interfaces();
    }
    // Si l'option n'est ni "-i" ni "-a", on affiche une erreur
    else {
        fprintf(stderr, "Erreur : Option invalide \"%s\".\n", argv[1]);
        fprintf(stderr, "Utilisation : %s [-i ifname | -a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
