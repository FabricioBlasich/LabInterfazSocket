/*
Integrantes Grupo3:
-Blasich, Fabricio
-Barrera, Paula
-Rubio, Martina
-Villanueva, Camilo
*/

/*
Nota: Ejecutar el programa con sudo
Ejemplo: sudo ./servidor.out
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 200
#define GROUP "ff05::1234" // Dirección multicast IPv6
#define PORT 1023
#define ORIGIN_PORT 1022 // Puerto origen

int main(int argc, char const *argv[])
{

    char message[BUFFER_SIZE];
    struct sockaddr_in6 multicast_addr, my_addr;

    printf("\tBienvenido!\n");
    printf("Servidor de mensajeria iniciado.\n");

    // Crear el socket UDP para IPv6
    int udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("Error al crear el socket");
        return EXIT_FAILURE;
    }

    // Configurar la dirección local para enlazar al puerto origen
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin6_family = AF_INET6;
    my_addr.sin6_port = htons(ORIGIN_PORT);
    my_addr.sin6_addr = in6addr_any;

    if (bind(udp_socket, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("Error al enlazar el socket al puerto origen");
        close(udp_socket);
        return EXIT_FAILURE;
    }

    // Configurar la dirección multicast
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin6_family = AF_INET6;
    multicast_addr.sin6_port = htons(PORT);
    inet_pton(AF_INET6, GROUP, &multicast_addr.sin6_addr);

    while (1)
    {
            
        printf("Ingrese un mensaje (maximo de %d caracteres): ", BUFFER_SIZE);
        if (fgets(message,BUFFER_SIZE,stdin)==NULL)
        {
            perror("Error al leer el mesaje");
            return EXIT_FAILURE;
        }
        
        // Enviar mensaje al grupo multicast
        if (sendto(udp_socket, message, strlen(message) + 1, 0, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr)) < 0) {
            perror("Error al enviar el mensaje");
            close(udp_socket);
            return EXIT_FAILURE;
        }

        message[strlen(message)-1] = '\0';
        printf("Mensaje enviado a %s:%d= \"%s\"", GROUP, PORT, message);
        printf("\n");
    }

    close(udp_socket);
    
    return EXIT_SUCCESS;
}
