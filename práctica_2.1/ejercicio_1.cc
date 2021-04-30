#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *res;

    //Corrección parametros de entrada usuario
    if(argc < 2 || argc >3){
        std::cerr << "Parámetros incorrectos\n Comando: ./ejercicio_1 [ip/dirweb/localhost (obligatorio)] [puerto (opcional)]\n ";
        return -1;
    }

    //Memoria reservada para el struct con "los filtros" para las conexiones
    //Aqui no vamos a filrar nada por lo que no asignamos valor a ningun atributo de hints
    memset((void *)&hints, 0, sizeof(struct addrinfo));

    //Cogemos en res todas las conexiones que cumplen los filtros
    int rcv = getaddrinfo(argv[1], argv[2], &hints, &res);

    //Gestion de errores
    if (rcv != 0)
    {
        std::cerr << "Fallo despues de la llamada de [getaddrinfo]\n";
        return -1;
    }

    //Recorremos la lista de conexiones mostrando su ip, su familia y su tipo de socket
    for (addrinfo* i = res; i != NULL; i = i->ai_next) {

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        
        //Rellenamos los buffers con la informacion
        getnameinfo(i->ai_addr,i->ai_addrlen,host,NI_MAXHOST,serv ,NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);

        //Mostramos info
        std::cout  << host << " "<< i->ai_family <<" " <<i->ai_socktype << std::endl;
    }

    //Liberamos la lista de los resultados
    freeaddrinfo(res);

    return 0;
}