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
    struct ifaddrs *ifaddr = NULL; // Pointeur pour la liste des interfaces
    struct ifaddrs *ifa = NULL;   // Pointeur pour parcourir la liste
    char address[INET6_ADDRSTRLEN]; // Tampon pour stocker l'adresse (IPv4 ou IPv6)

    // Obtenir la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        // Afficher un message d'erreur en cas d'échec
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE); // Terminer le programme avec un code d'échec
    }

    // Afficher un message pour indiquer quelle interface est en cours de traitement
    printf("Adresses associées à l'interface : %s\n", ifname);

    // Parcourir la liste des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Vérifier que l'interface possède une adresse
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        // Vérifier si le nom de l'interface correspond à celui demandé
        if (strcmp(ifa->ifa_name, ifname) != 0) {
            continue; // Passer à l'interface suivante si le nom ne correspond pas
        }

        // Vérifier si l'adresse est de type IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr; // Conversion
            // Convertir l'adresse binaire en texte lisible
            inet_ntop(AF_INET, &(ipv4->sin_addr), address, INET_ADDRSTRLEN);
            printf("  Adresse IPv4 : %s\n", address);
        }
        // Vérifier si l'adresse est de type IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr; // Conversion
            // Convertir l'adresse binaire en texte lisible
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), address, INET6_ADDRSTRLEN);
            printf("  Adresse IPv6 : %s\n", address);
        }
    }

    // Libérer la mémoire allouée pour la liste des interfaces
    freeifaddrs(ifaddr);

    // Ajouter un message de fin pour plus de clarté
    printf("Fin de la liste des adresses pour l'interface : %s\n", ifname);
}

// Fonction pour afficher toutes les interfaces réseau et leurs adresses associées
void show_all_interfaces() {
    struct ifaddrs *ifaddr = NULL; // Pointeur pour la liste des interfaces
    struct ifaddrs *ifa = NULL;   // Pointeur pour parcourir la liste
    char address[INET6_ADDRSTRLEN]; // Tampon pour stocker les adresses (IPv4 et IPv6)

    // Récupération de la liste des interfaces réseau
    if (getifaddrs(&ifaddr) == -1) {
        // Gestion des erreurs si l'appel échoue
        perror("Erreur lors de l'appel à getifaddrs");
        exit(EXIT_FAILURE); // Quitter le programme en cas d'échec
    }

    // Message pour indiquer le début de la liste
    printf("Liste des interfaces réseau et leurs adresses associées :\n");

    // Parcourir la liste des interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        // Vérifier si l'interface possède une adresse
        if (ifa->ifa_addr == NULL) {
            continue; // Passer à l'interface suivante
        }

        // Afficher le nom de l'interface réseau
        printf("\nInterface : %s\n", ifa->ifa_name);

        // Vérifier si l'adresse est de type IPv4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr; // Conversion
            // Convertir l'adresse binaire en texte lisible
            inet_ntop(AF_INET, &(ipv4->sin_addr), address, INET_ADDRSTRLEN);
            printf("  Adresse IPv4 : %s\n", address);
        }
        // Vérifier si l'adresse est de type IPv6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ifa->ifa_addr; // Conversion
            // Convertir l'adresse binaire en texte lisible
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), address, INET6_ADDRSTRLEN);
            printf("  Adresse IPv6 : %s\n", address);
        }
    }

    // Libérer la mémoire allouée pour la liste des interfaces
    freeifaddrs(ifaddr);

    // Message pour indiquer la fin de l'affichage
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
            fprintf(stderr, "Utilisation : %s -i <nom_interface>\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        // Appelle la fonction pour afficher les adresses de l'interface spécifiée
        const char *interface_name = argv[2];
        show_interface_addresses(interface_name);
    }
    // Vérifie si l'option fournie est "-a"
    else if (strcmp(argv[1], "-a") == 0) {
        // Appelle la fonction pour afficher toutes les interfaces et leurs adresses
        show_all_interfaces();
    }
    // Si l'option n'est ni "-i" ni "-a", affiche une erreur
    else {
        fprintf(stderr, "Erreur : Option invalide \"%s\".\n", argv[1]);
        fprintf(stderr, "Utilisation : %s [-i ifname | -a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Retourne 0 pour indiquer que le programme s'est terminé avec succès
    return 0;
}
