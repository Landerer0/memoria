// g++ maintuple.cpp klltuple.hpp klltuple.cpp minheap.cpp minheap.hpp -o maintuple
// ./maintuple archivoTraza              // GENERAL
// ./maintuple Mendeley.txt
// ./maintuple numElementos
// ./maintuple 10000000

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
#include <map>

#include <fstream>
#include <sstream>

#include "klltuple.hpp"

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

void almacenarDatos(string nombreBinario, vector<pair<double,uint64_t>> estimacionTopk, map<uint64_t,uint64_t> &data, KLLTuple kll){

    string nombreArchivo="";

    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "estimacionFrecuencias/";

    string archivoGuardado = nombreCarpeta+nombreBinario+".txt";
    // Nombre del archivo de salida
    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(archivoGuardado);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : estimacionTopk) {
            outfileParametros << element.second << " " << data[(uint64_t)element.first] << " " << (uint64_t)element.first << endl;
        }
        outfileParametros << kll.sizeInBytes();
        outfileParametros << std::endl;
        // Cerrar el archivo
        outfileParametros.close();
        std::cout << "Las estimaciones de frecuencia se han almacenado correctamente en el archivo " << archivoGuardado << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << archivoGuardado << " para escritura." << std::endl;
    }
    
    return;
}

int main(int argc, char*argv[]){
    vector<KLLTuple> klls;

    uint64_t n = stoull(argv[1]);
    double epsilon = 0.01;
    double delta = 0.001;
    double c = 0.5;
    int mink= 7;

    KLLTuple kll(n/4, epsilon, delta, c, mink);
    for(int i =0;i<n;i++){
        kll.add(make_pair(rand()%3000,rand()%16));
    }

    kll.print();

    return 0;
}
