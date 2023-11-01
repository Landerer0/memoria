// g++ maintuplearchivo.cpp klltuple.hpp klltuple.cpp minheap.cpp minheap.hpp -o maintuplearchivo
// ./maintuplearchivo archivoTraza              // GENERAL
// ./maintuplearchivo pares3000000.txt
// ./maintuplearchivo Mendeley.txt
// ./maintuplearchivo numElementos
// ./maintuplearchivo 10000000

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <cmath>
#include <map>


#include "klltuple.hpp"

using namespace std;

// Función para almacenar un par (suma, segundo) en un vector
void processPair(std::vector<std::pair<int64_t, int64_t>>& results, std::pair<int64_t, int64_t> pair) {
    for (auto& result : results) {
        if (result.second == pair.second) {
            result.first += pair.first;
            return;
        }
    }
    results.push_back(pair);
}

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

// Función para leer un archivo de pares y calcular la suma de los primeros componentes
std::vector<std::pair<int64_t, int64_t>> calculateSum(const std::string& fileName) {
    std::ifstream file(fileName);
    std::vector<std::pair<int64_t, int64_t>> results;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::pair<int64_t, int64_t> pair;
            std::istringstream iss(line);
            if (iss >> pair.first >> pair.second) {
                processPair(results, pair);
            }
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo: " << fileName << std::endl;
    }

    // Ordenar los resultados por la suma de los primeros componentes
    std::sort(results.begin(), results.end());

    return results;
}


int main(int argc, char*argv[]){
    uint64_t n = numLineasArchivo(argv[1]);
    string archivo = argv[1];
    double epsilon = 0.01;
    double delta = 0.001;
    double c = 0.5;
    int mink= 7;

    KLLTuple kll(n/4, epsilon, delta, c, mink);

    ifstream file(archivo);
    std::string linea;
    while (std::getline(file, linea)) {
        std::pair<int64_t, int64_t> elemento;
        std::istringstream iss(linea);
        if (iss >> elemento.first >> elemento.second) {
            // Insertar el elemento en el Min Heap
            kll.add(elemento);
        } else {
            std::cerr << "Error al leer la línea: " << linea << std::endl;
        }
    }


    kll.print();

    vector<std::pair<int64_t, int64_t>>  truth = calculateSum(archivo);
    cout << endl << endl;
    cout << "Resultados" << endl;
    int size = truth.size()-1;
    for(int i=0;i<150;i++){
        cout << truth.at(size-i).first << " " << truth.at(size-i).second << endl;
    }


    return 0;
}

