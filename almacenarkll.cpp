// g++ almacenarkll.cpp kll.cpp kll.hpp -o almacenarkll
// ./almacenarkll numeros.txt

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
#include <cmath>

#include <fstream>
#include <sstream>

#include "kll.hpp"

using namespace std;

uint32_t numLineasArchivo(string archivo){
    fstream in(archivo);
    int n = 0;
    int lines = 0;
    char endline_char = '\n';
    while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
    {
        ++lines;
    }
    n = lines-1;
    cout << "En en el archivo " << archivo << " hay " << n << " lineas" << endl;
    return n;
}

void ingresarDatosEnKLL(string archivoTxt, KLL &kll){
    std::ifstream archivo(archivoTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                kll.add(dato);
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoTxt << std::endl;
        return;
    }
}

int main(int argc, char*argv[]){
    if(argc > 2){
        uint64_t kmin = stol(argv[1]);
        // Definición de nombres de archivos a utilizar
        string archivoActualTxt = argv[2];
        string archivoActual = archivoActualTxt;
        size_t posicion = archivoActual.find(".txt");

        if (posicion != std::string::npos) {
            archivoActual.erase(posicion, 4); // Eliminamos 4 caracteres: .txt
        }

        cout << archivoActualTxt << endl;
        KLL kllkmin(kmin);
        ingresarDatosEnKLL(archivoActualTxt, kllkmin);


        kllkmin.saveData(archivoActual+".bin");
        KLL kllkmin2 = kllkmin.readData(archivoActual+".bin");

        kllkmin.print();
        kllkmin2.print();

        cout << "KLL: " << kllkmin.quantile(50) << endl << "KLLkmin2: " << kllkmin2.quantile(50) << endl;
    } else {
        KLL kll(200);
        kll.saveData("vacio.bin");
    }

    return 0;
}