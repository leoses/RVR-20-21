#include <netdb.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_5 <direccion> <puerto> \n ";
        return EXIT_FAILURE;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    //Memoria reservada para el struct con "los filtros" para las conexiones
    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;       //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP

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

    int sd_servidor = connect(sd, (struct sockaddr *)res->ai_addr, res->ai_addrlen);

    if (sd_servidor == -1)
    {
        std::cerr << "No ha sido posible conectarse al servidor [connect]\n";
        return EXIT_FAILURE;
    }

    bool connected = true;
    char buffer[MESSAGE_MAX_SIZE];

    while (connected)
    {
        //Leemos informacion de consola
        std::cin >> buffer;

        //La enviamos al servidor
        send(sd_servidor, buffer, (MESSAGE_MAX_SIZE - 1)*sizeof(char), 0);

        //Si pulsamos Q cerramos la conexion
        if (buffer[0] == 'Q' && buffer[1] == '\0')
        {
            connected = false;
            continue;
        }

        //Recibimos la respuesta (echo) del servidor
        recv(sd_servidor, buffer,(MESSAGE_MAX_SIZE - 1)*sizeof(char),0);

        //La mostramos por pantalla
        std::cout << buffer << '\n';
    }

    //close(sd_servidor);
    close(sd);
    return 0;
}