#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

//Constantes
const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *res;

    //Corrección parametros de entrada usuario
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_2 <direccion> <puerto>\n ";
        return EXIT_FAILURE;
    }

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

    bool serverActive = true;
    time_t rawtime;
    size_t tam;
    char buffer[MESSAGE_MAX_SIZE];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    while (serverActive)
    {
        struct sockaddr client;
        socklen_t clientLength = sizeof(struct sockaddr);

        int bytesReceived = recvfrom(sd, (void *)buffer, (MESSAGE_MAX_SIZE - 1)*sizeof(char), 0, &client, &clientLength);
        buffer[MESSAGE_MAX_SIZE] = '\0';

        if (bytesReceived == -1)
        {
            std::cerr << "Error en la recepcion de bytes, cerrando conexión\n";
            return EXIT_FAILURE;
        }

        //Obtenemos nombre y puerto del cliente y lo mostramos información por consola
        getnameinfo(&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << bytesReceived << " bytes de " << host << ":" << serv << "\n";

        //Procesamos comandos
        switch (buffer[0])
        {

        //Mostrar hora
        case 't':
            time(&rawtime);
            tam = strftime(buffer, MESSAGE_MAX_SIZE - 1, "%T %p", localtime(&rawtime));
            sendto(sd, buffer, tam, 0, &client, clientLength);
            break;

        //Mostrar fecha
        case 'd':
            time(&rawtime);
            tam = strftime(buffer, MESSAGE_MAX_SIZE - 1, "%F", localtime(&rawtime));
            sendto(sd, buffer, tam, 0, &client, clientLength);
            break;

        //Cerrar servidor
        case 'q':
            std::cout << "Saliendo...\n";
            serverActive = false;
            sendto(sd, "Usted ha cerrado el servidor\n", 30, 0, &client, clientLength);
            break;

        //Comando desconocido
        default:
            std::cout << "Comando no soportado: " << buffer[0] << "\n";
            sendto(sd, "Comando no valido\n", 19, 0, &client, clientLength);

        }
    }

    //Cerramos socket
    close(sd);

    return 0;
}