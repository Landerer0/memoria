// g++ transformaTrazasEnFlujos.cpp -o transformaTrazasEnFlujos
// ./transformaTrazasEnFlujos nombreFicheroEntrada nombreFicheroSalida
// ./transformaTrazasEnFlujos Mendeley.txt flujosMendeley.txt
// ./transformaTrazasEnFlujos Chicago-20160121.txt flujosChicago-20160121.txt
// ./transformaTrazasEnFlujos Mawi-20181201.txt flujosMawi-20181201.txt 
// ./transformaTrazasEnFlujos Sanjose-20081016.txt flujosSanjose-20081016.txt 

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char *argv[]) {
    // Verifica si se proporciona el nombre del archivo como argumento
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo_entrada> <nombre_archivo_salida>\n";
        return 1;
    }

    // Valor que se agregará al principio de cada línea
    int x = 50;

    // Abre el archivo de entrada
    std::ifstream archivoEntrada(argv[1]);
    if (!archivoEntrada) {
        std::cerr << "No se puede abrir el archivo de entrada.\n";
        return 1;
    }

    // Abre el archivo de salida
    std::ofstream archivoSalida(argv[2]);
    if (!archivoSalida) {
        std::cerr << "No se puede abrir el archivo de salida.\n";
        return 1;
    }

    std::string linea;
    while (std::getline(archivoEntrada, linea)) {
        // Agrega el valor al principio de cada línea
        archivoSalida << x << ' ' << linea << '\n';
    }

    std::cout << "Proceso completado. Se ha creado el archivo de salida: " << argv[2] << '\n';

    return 0;
}
