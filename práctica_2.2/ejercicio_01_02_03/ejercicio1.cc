#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_LENGHT 80

class Jugador : public Serializable
{
public:
    Jugador(const char *_n, int16_t _x, int16_t _y) : x(_x), y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        //tam de la clase jugador [2 veces el tam de int16_t (x,y) y el nombre]
        _size = sizeof(int16_t) * 2 + sizeof(char) * MAX_NAME_LENGHT;
        //Reservamos memoria para el objeto de esta clase
        alloc_data(_size);

        char *temp = _data;
        //Copiamos el nombre a partir de la direccion que marca temp
        memcpy(temp, name, MAX_NAME_LENGHT * sizeof(char));
        //Avanzamos el puntero hasta dejar de apuntar el nombre que acabamos de copiar
        temp += MAX_NAME_LENGHT * sizeof(char);

        //Copiamos la variable x a partir de la direccion de memoria que marca temp
        memcpy(temp, &x, sizeof(int16_t));
        //Avanzamos el puntero hasta dejar de apuntar al entero que acabamos de copiar
        temp += sizeof(int16_t);

        //Copiamos la variable y a partir de la direccion de memoria que marca temp
        memcpy(temp, &y, sizeof(int16_t));
    }

    int from_bin(char *data)
    {
        //Hacemos un puntero temp que apunte a la direccion de memoria en la que queremos copiar
        //la informacion de nuestra clase player
        char *temp = data;
        
        //Copiamos el nombre
        memcpy(name, temp, MAX_NAME_LENGHT * sizeof(char));
        //Avanzamos el puntero hasta sobrepasar ese nombre
        temp += MAX_NAME_LENGHT * sizeof(char);
        
        //Copiamos el entero x
        memcpy(&x, temp, sizeof(int16_t));
        //Avanzamos el puntero hasta sobrepasar ese entero
        temp += sizeof(int16_t);
        
        //Copiamos la variable y
        memcpy(&y, temp, sizeof(int16_t));

        return 0;
    }

    int16_t getX() const{return x;}
    int16_t getY() const{return y;}
    char name[MAX_NAME_LENGHT];


private:

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);   
    
    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int fd = open("./player1_data.txt", O_CREAT | O_TRUNC |O_RDWR, 0666);
    ssize_t tam = write(fd,one_w.data(), one_w.size());
    if(tam != one_w.size()) std::cout << "Warning: no all info was saved correctly\n";
    close(fd);

    // 3. Leer el fichero
    fd = open("./player1_data.txt", O_RDONLY, 0666);
    char buffer[tam];
    
    if(read(fd,&buffer,tam)==-1){
        std::cerr << "Something wrong happened while reading: ./player1_data.txt\n";
        return -(EXIT_FAILURE);
    } 
    close(fd);

    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    
    // 5. Mostrar el contenido de one_r
    std::cout << "Nombre de one_r: " << one_r.name << ", x: " << one_r.getX() << ", y: " << one_r.getY() << "\n";

    return 0;
}
