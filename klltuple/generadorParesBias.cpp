// g++ generadorParesBias.cpp -o generadorParesBias
// ./generadorParesBias  nombreFicheroSalida
// ./generadorParesBias paresBiasShuffle.txt
// ./generadorParesBias paresBiasShuffle2.txt

#include <iostream>
#include <fstream>
#include <random>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    int numPacketsMayorFlujo = 1000000;
    int numPacketsFlujosMayores = numPacketsMayorFlujo;
    int numPacketsFlujosMenores = numPacketsMayorFlujo;
    int numPacketsMenorFlujo = numPacketsMayorFlujo;

    

    string nombreFichero = argv[1];
    std::ofstream outFile(nombreFichero);

    if (!outFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    //! paresBiasShuffle.txt
    // // Generar n pares aleatorios y escribirlos en el archivo
    // for (int i = 0; i < numPacketsMayorFlujo; ++i) {
    //     int64_t first = rand()%500;
    //     int64_t second = 1+rand()%3;
    //     outFile << first << " " << second << std::endl;
    // }

    // for (int i = 0; i < numPacketsFlujosMayores; ++i) {
    //     int64_t first = rand()%500;
    //     int64_t second = 4+rand()%150;
    //     outFile << first << " " << second << std::endl;
    // }

    // for (int i = 0; i < numPacketsFlujosMenores; ++i) {
    //     int64_t first = rand()%500;
    //     int64_t second = 154+rand()%847;
    //     outFile << first << " " << second << std::endl;
    // }


    // for (int i = 0; i < numPacketsMenorFlujo; ++i) {
    //     int64_t first = rand()%500;
    //     int64_t second = 1000+rand()%29000;
    //     outFile << first << " " << second << std::endl;
    // }


    //! paresBiasShuffle2.txt
    // Generar n pares aleatorios y escribirlos en el archivo
    for (int i = 0; i < numPacketsMayorFlujo; ++i) {
        int64_t first = rand()%500;
        int64_t second = 1;
        outFile << first << " " << second << std::endl;
    }

    for (int i = 0; i < numPacketsFlujosMayores; ++i) {
        int64_t first = rand()%500;
        int64_t second = 2+rand()%150;
        outFile << first << " " << second << std::endl;
    }

    for (int i = 0; i < numPacketsFlujosMenores; ++i) {
        int64_t first = rand()%500;
        int64_t second = 152+rand()%846;
        outFile << first << " " << second << std::endl;
    }


    for (int i = 0; i < numPacketsMenorFlujo; ++i) {
        int64_t first = rand()%500;
        int64_t second = 1000+rand()%29000;
        outFile << first << " " << second << std::endl;
    }


    outFile.close();

    std::cout << "Se generaron pares aleatorios y se guardaron en " << nombreFichero << std::endl;

    return 0;
}