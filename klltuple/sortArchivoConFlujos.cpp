// g++ sortArchivoConFlujos.cpp -o sortArchivoConFlujos
// ./sortArchivoConFlujos archivoAOrdenar nombreArchivoOrdenado              // GENERAL
// ./sortArchivoConFlujos pares3000000.txt
// ./sortArchivoConFlujos paresBiasShuffle.txt sortparesBiasShuffle.txt
// ./sortArchivoConFlujos paresBiasShuffle2.txt sortparesBias2Shuffle.txt
// ./sortArchivoConFlujos paresBiasSimpleEscalonShuffle.txt sortparesBiasSimpleEscalon.txt
// ./sortArchivoConFlujos flujosMendeley.txt sortflujosMendeley.txt
// ./sortArchivoConFlujos flujosChicago-20160121.txt sortflujosChicago-20160121.txt
// ./sortArchivoConFlujos flujosMawi-20181201.txt sortflujosMawi-20181201.txt
// ./sortArchivoConFlujos flujosSanjose-20081016.txt sortflujosSanjose-20081016.txt

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
    string archivoEntrada = argv[1];
    vector<std::pair<int64_t, int64_t>>  truth = calculateSum(archivoEntrada);

    string archivoSalida = argv[2];
    std::ofstream outFile(archivoSalida);

    for(int i=truth.size()-1;i>=0;i--){
        outFile << truth.at(i).first << " " << truth.at(i).second << endl;
    }

    outFile.close();

    std::cout << "Se ordenaron los valores encontrados en "<<archivoEntrada<<" en el fichero " << archivoSalida << std::endl;

    return 0;
}

