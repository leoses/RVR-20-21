#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

//Threads
#include <thread>

const size_t MESSAGE_MAX_SIZE = 100;
#define MAX_THREADS 5

class MessageThread
{
private:
    int sd;
    int id;

public:
    MessageThread(int sd_, int i_) : sd(sd_), id(i_) {}

    void haz_mensaje()
    {
        time_t rawtime;
        size_t tam;
        char buffer[MESSAGE_MAX_SIZE];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientLength = sizeof(struct sockaddr);

        while (true)
        {
            //Para probar la concurrencia
            sleep(3); 

            int bytesReceived = recvfrom(sd, (void *)buffer, (MESSAGE_MAX_SIZE - 1) * sizeof(char), 0, &client, &clientLength);
            buffer[MESSAGE_MAX_SIZE] = '\0';
            
            int i = 0;

            /*------Para comprobar si se entremezclan los threads
            while(i < 20){
                std::cout << i << " Thread:"<< std::this_thread::get_id()<<"\n";
                i++;
                sleep(1);
            }
            */
            

            if (bytesReceived == -1)
            {
                std::cerr << "Error en la recepcion de bytes, cerrando conexión\n";
                return;
            }

            //Obtenemos nombre y puerto del cliente y lo mostramos información por consola
            getnameinfo(&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout << bytesReceived << " bytes de " << host << ":" << serv << ". Thread: " << std::this_thread::get_id() << "\n";

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

            //Comando desconocido
            default:
                std::cout << "Comando no soportado: " << buffer[0] << "\n";
                sendto(sd, "Comando no valido\n", 19, 0, &client, clientLength);
            }
        }
    }
};

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *res;

    //Corrección parametros de entrada usuario
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_6 <direccion> <puerto>\n ";
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

    std::thread threads[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        MessageThread *m = new MessageThread(sd, i);
        std::thread([&m]() {m->haz_mensaje(); delete m; });
    }

    //El thread principal es el que procesa el cierre del servidor cuando
    //se recibe una q
    std::string q;
    do
    {
        std::cin >> q;
    } while (q != "q");

    //Cerramos socket del servidor
    close(sd);

    return 0;
}