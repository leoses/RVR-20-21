#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

    //Asignamos el addr al socket
    if (bind(sd, res->ai_addr, res->ai_addrlen) == -1)
    {
        std::cerr << "Error en la llamada a [bind]\n";
        return EXIT_FAILURE;
    }

    //Liberamos memoria de la lista puesto que ya no es necesaria
    freeaddrinfo(res);

    char buffer[MESSAGE_MAX_SIZE];

    

    return 0;
}