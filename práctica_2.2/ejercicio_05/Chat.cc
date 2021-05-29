#include "Chat.h"

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
    memcpy(temp, nick.c_str(), sizeof(char) * 8);
    temp += sizeof(char) * 8;

    //Copiamos el mensaje a partir de la direccion que marca temp
    memcpy(temp, message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *temp = _data;

    //Copiamos tipo
    memcpy(&type, temp, sizeof(uint8_t));
    temp += sizeof(uint8_t);

    //Se puede hacer porque es un string (\0)
    nick = temp;
    temp += sizeof(char) * 8;

    //Se puede hacer porque es un string (\0)
    message = temp;

    return 0;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
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
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

