#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

/*
Formato de comando esperado: ./ejercicio_3 127.0.0.1 3000 t
*/

const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{
    if(argc != 4){
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_3 <direccion> <puerto> <comando>\n ";
        return EXIT_FAILURE;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    //Memoria reservada para el struct con "los filtros" para las conexiones
    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;      //IPv4
    hints.ai_socktype = SOCK_DGRAM; //UDP

    //Cogemos en res todas las conexiones que cumplen los filtros
    int rcv = getaddrinfo(argv[1], argv[2], &hints, &res);

    //Gestion de errores
    if (rcv != 0)
    {
        fprintf(stderr, "Error [getaddrinfo]: %s\n", gai_strerror(rcv));
        return EXIT_FAILURE;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    //Si no se ha podido crear un socket
    if (sd == -1)
    {
        std::cerr << "Error en la creación de [socket]\n";
        return EXIT_FAILURE;
    }

    //Liberamos memoria de la lista puesto que ya no es necesaria
    freeaddrinfo(res);

    char buffer[MESSAGE_MAX_SIZE];

    //Enviamos información esperando recibir respuesta
    if(sendto(sd, argv[3], strlen(argv[3])+1, 0, res->ai_addr, res->ai_addrlen) == -1){
        std::cerr << "Error en el envío de información al servidor [sendTo]\n";
        return EXIT_FAILURE;
    }

    //Recibimos mensaje del servidor
    if(recvfrom(sd, buffer, (MESSAGE_MAX_SIZE - 1) * sizeof(char), 0, res->ai_addr, &res->ai_addrlen) == -1){
        std::cerr << "Error en la recepción bytes de [recvfrom]\n";
        return EXIT_FAILURE;
    }

    std::cout << buffer << "\n";

    //Cerramos socket
    close(sd);

    return 0;
}