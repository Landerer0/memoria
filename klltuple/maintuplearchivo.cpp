// g++ maintuplearchivo.cpp klltuple.hpp klltuple.cpp minheap.cpp minheap.hpp -o maintuplearchivo
// ./maintuplearchivo archivoTraza  numNivelesHeap            // GENERAL
// ./maintuplearchivo pares3000000.txt
// ./maintuplearchivo paresBiasSimpleEscalonShuffle.txt 7

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
#include <ctime>  
#include <cstdlib>  


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
std::vector<std::pair<int64_t, int64_t>> getTruthFromSortedFile(string fileName) {
    std::ifstream file(fileName);
    std::vector<std::pair<int64_t, int64_t>> results;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::pair<int64_t, int64_t> pair;
            std::istringstream iss(line);
            if (iss >> pair.first >> pair.second) {
                results.push_back(pair);
            }
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo: " << fileName << std::endl;
    }

    return results;
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

    std::sort(results.begin(), results.end(), [](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
        return a.first > b.first;
    });

    return results;
}

// Función para leer un archivo de pares y calcular la suma de los primeros componentes
std::vector<std::pair<int64_t, int64_t>> calculateLocalSum(const std::string& fileName, int n) {
    std::ifstream file(fileName);
    std::vector<std::pair<int64_t, int64_t>> results;
    int linea=0;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::pair<int64_t, int64_t> pair;
            std::istringstream iss(line);
            if (iss >> pair.first >> pair.second) {
                if(linea<n) linea++;
                else break;
                processPair(results, pair);
            }
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo: " << fileName << std::endl;
    }


    std::sort(results.begin(), results.end(), [](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
        return a.first > b.first;
    });

    return results;
}


int main(int argc, char*argv[]){
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    uint64_t n = numLineasArchivo(argv[1]);
    int heapLevels = stoi(argv[2]);
    string archivo = argv[1];
    double epsilon = 0.01;
    double delta = 0.001;
    double c = 0.5;
    int mink= 7;

    KLLTuple kll(n, epsilon, delta, c, mink);
    kll.resetHeap(heapLevels);

    ifstream file(archivo);
    std::string linea;
    int stop = 1000;
    int stopActual = 0;
    while (std::getline(file, linea)) {
        std::pair<int64_t, int64_t> elemento;
        std::istringstream iss(linea);
        if (iss >> elemento.first >> elemento.second) {
            // if(stopActual<stop) stop++;
            // else break;
            // Insertar el elemento en el Min Heap
            kll.add(elemento);

        } else {
            std::cerr << "Error al leer la línea: " << linea << std::endl;
        }
    }
    file.close();


    kll.print();

    vector<std::pair<int64_t, int64_t>>  heap = kll.getSortHeap();
    cout << endl << endl;
    cout << "Resultados" << endl;
    cout << "Truth \t Estimado" << endl;

    //LOCAL
    // vector<std::pair<int64_t, int64_t>>  localTruth = calculateLocalSum(archivo,stop);
    // for(int i=0;i<150;i++){
    //     if(i<heap.size()) cout << localTruth.at(i).first << " " << localTruth.at(i).second << " \t " << heap.at(i).first << " " << heap.at(i).second << endl;
    //     else if(i<localTruth.size()) cout << localTruth.at(i).first << " " << localTruth.at(i).second << endl;
    // }

    vector<std::pair<int64_t, int64_t>>  truth = getTruthFromSortedFile("sort"+archivo);
    for(int i=0;i<150;i++){
        if(i<heap.size()) cout << truth.at(i).first << " " << truth.at(i).second << " \t " << heap.at(i).first << " " << heap.at(i).second << endl;
        else if(i<truth.size()) cout << truth.at(i).first << " " << truth.at(i).second << endl;
    }


    return 0;
}

