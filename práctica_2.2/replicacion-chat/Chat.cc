#include "Chat.h"
#include <memory.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char *temp = _data;

    //Copiamos tipo de mensaje a partir de la direccion que marca temp
    memcpy(temp, &type, sizeof(uint8_t));
    temp += sizeof(uint8_t);

    //Copiamos el nombre a partir de la direccion que marca temp
    memcpy(temp, &nick, sizeof(char) * 8);
    temp += sizeof(char) * 8;

    //Copiamos el mensaje a partir de la direccion que marca temp
    memcpy(temp, &message, sizeof(char) * 80);
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *temp = _data;

    memcpy(&type, &temp, sizeof(uint8_t));
    temp += sizeof(uint8_t);

    memcpy(&nick, &temp, sizeof(char) * 8);
    temp += sizeof(char) * 8;

    memcpy(&message, &temp, sizeof(char) * 80);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    //Abrir conexion servidor
    socket.bind();

    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Esperamos recibir un mensaje de cualquier socket
        ChatMessage cm;
        Socket *s;

        //Esperamos a recibir un objeto serializable desde cualquier socket
        socket.recv(cm, s);

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch (cm.type)
        {
        case ChatMessage::LOGIN:
            /* code */
            std::cout << "Jugador conectado: " << cm.nick << "\n";
            //Lo añadimos a la lista de clientes convirtiendo el socket en un unique_ptr y usando move
            clients.push_back(std::move(std::make_unique<Socket>(s)));
            break;

        case ChatMessage::LOGOUT:
            /* code */
            auto it  = clients.begin();

            while(it != clients.end() && (*it).get() != s)++it;

            if(it == clients.end()) std::cout << "El jugador ya se había desconectado previamente\n";
            else{
                std::cout << "Jugador desconectado: " << cm.nick << "\n";
                clients.erase(it); //Lo sacamos del vector
                Socket* delSock = (*it).release(); //Eliminamos la pertenencia del socket de dicho unique_ptr
                delete delSock; //Borramos el socket
            }

            break;
        case ChatMessage::MESSAGE:
            //Reenviar el mensaje a todos los clientes menos a si mismo
            for(int i =0; i < clients.size(); i++)
                if(clients[i].get() != s) clients[i].get()->send(cm, *s);
            break;
        
        default:
            std::cerr << "Something went wrong bro\n";
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while (true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}
