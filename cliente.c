/*
Integrantes Grupo3:
-Blasich, Fabricio
-Barrera, Paula
-Rubio, Martina
-Villanueva, Camilo
*/

/*
Nota: Ejecutar el programa con sudo
Ejemplo: sudo ./cliente.out
*/


#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 200
#define GROUP "ff05::1234" // Dirección multicast IPv6
#define PORT 1023

void mostrar_fecha_hora();

int main() {
    int udp_socket;
    struct sockaddr_in6 my_addr;
    struct sockaddr_in6 peer_addr;
    struct ipv6_mreq group;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(peer_addr);

    // Crear el socket UDP para IPv6
    if ((udp_socket = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        return EXIT_FAILURE;
    }

    // Configurar la dirección local para recibir en el puerto multicast
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(PORT);
    my_addr.sin6_addr = in6addr_any;
    
    if (bind(udp_socket, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("Error al enlazar el socket");
        close(udp_socket);
        return EXIT_FAILURE;
    }

    // Unirse al grupo multicast
    inet_pton(AF_INET6, GROUP, &group.ipv6mr_multiaddr);
    group.ipv6mr_interface = 0;  // 0 para usar la interfaz predeterminada

    if (setsockopt(udp_socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof(group)) < 0) {
        perror("Error al unirse al grupo multicast");
        close(udp_socket);
        return EXIT_FAILURE;
    }

    printf("Escuchando mensajes en el grupo multicast %s en el puerto %d...\n\n", GROUP, PORT);

    while (1)
    {
        // Recibir mensajes del grupo multicast
        int bytes_received = recvfrom(udp_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&peer_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Error al recibir mensaje");
        } else {
            buffer[bytes_received] = '\0';

            mostrar_fecha_hora();
            char ip_servidor[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &peer_addr.sin6_addr, ip_servidor, sizeof(ip_servidor));
            printf("Mensaje recibido de: %s:%d\n", ip_servidor, ntohs(peer_addr.sin6_port));
            printf("Mensaje recibido: %s\n\n", buffer);
        }

    }
    // Dejar el grupo multicast
    setsockopt(udp_socket, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &group, sizeof(group));
    close(udp_socket);
    return EXIT_SUCCESS;
}


void mostrar_fecha_hora()
{
    time_t t = time(NULL);
    struct tm tiempo = *localtime(&t);
    printf("Fecha y hora: %02d-%02d-%d %02d:%02d:%02d\n", tiempo.tm_mday, tiempo.tm_mon + 1, tiempo.tm_year + 1900, tiempo.tm_hour -3, tiempo.tm_min, tiempo.tm_sec);
}