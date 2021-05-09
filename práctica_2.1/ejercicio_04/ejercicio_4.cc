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
    //Corrección parametros de entrada usuario
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_4 <direccion> <puerto>\n ";
        return EXIT_FAILURE;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;     //Devolver 0.0.0.0
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP

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

    //EL servidor se pone a escuchar en el socket
    //DE MOMENTO SOLO UNA CONEXION AL MISMO TIEMPO
    if(listen(sd, 1) == -1){
        std::cerr << "Fallo en el [listen]\n";
        return EXIT_FAILURE;
    }

    //ACEPTAR CONEXION DEL CLIENTE
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLength = sizeof(struct sockaddr);
    int cliente_sd = accept(sd, (struct sockaddr *)&client, &clientLength);

    //Gestion de errores
    if(cliente_sd == -1){
        std::cerr << "No se ha aceptado la conexion TCP con el cliente [accept]\n";
    } 

    getnameinfo((struct sockaddr *)&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    printf("Conexión desde Host:%s Puerto:%s\n", host, serv);


    //FUNCIONALIDAD DEL SERVIDOR ECHO
    bool serverActive = true;
    while (serverActive)
    {
        char buffer[MESSAGE_MAX_SIZE];

        //RECEPCION DESDE EL CLIENTE
        ssize_t bytesRecieved = recv(cliente_sd, (void *) buffer, (MESSAGE_MAX_SIZE -1 )* sizeof(char), 0);

        if( bytesRecieved <= 0 ){
            std::cout << "Conexion terminada\n";
            serverActive = false;
            continue;
        }
        
        //REENVIO DE LA INFORMACION DESDE EL SERVER AL CLIENTE
        send(cliente_sd, (void *) buffer, bytesRecieved, 0);
    }

    //Cerrar socket del cliente
    close(cliente_sd);

    return 0;
}