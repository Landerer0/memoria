// g++ pruebaEspacio.cpp kll.hpp kll.cpp -o pruebaEspacio
// ./pruebaEspacio data/Mendeley.txt

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

#include <fstream>
#include <sstream>

#include "kll.hpp"

using namespace std;

int main(int argc, char*argv[]){
    if(argc!=2){
        cout << "indicar nombre archivo" << endl;
        return 1;
    }
    string nombreArchivo = argv[1];

    uint64_t n;

    fstream in(nombreArchivo);
        int lines = 0;
        char endline_char = '\n';
        while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
        {
            ++lines;
        }
        n = lines-1;
        cout << "En en el archivo " << nombreArchivo << " hay " << n << " lineas" << endl;
    
    // DEFINICION DE PARAMETROS
    double epsilon = 0.05;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    //KLL kll(n,epsilon,delta,c,20);
    //KLL kllkmin(200);
    uint64_t espacio = 20000;
    KLL mrlEspacio(espacio,n);
    std::ifstream archivo(nombreArchivo);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) 
                mrlEspacio.add(dato);
                //kll.add(dato);
                //kllkmin.add(dato);
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return 0;
    }
    
    cout << "size mrlEspacio: " << mrlEspacio.sizeInBytes() << endl;
    //mrlEspacio.print();

    return 0;
}