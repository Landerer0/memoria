// g++ sortArchivoConFlujos2.cpp -o sortArchivoConFlujos2
// ./sortArchivoConFlujos2 archivoAOrdenar nombreArchivoOrdenado              // GENERAL
// ./sortArchivoConFlujos2 pares3000000.txt
// ./sortArchivoConFlujos2 paresBiasShuffle.txt sortparesBiasShuffle.txt
// ./sortArchivoConFlujos2 paresBiasShuffle2.txt sortparesBias2Shuffle.txt
// ./sortArchivoConFlujos2 paresBiasSimpleEscalonShuffle.txt sortparesBiasSimpleEscalon.txt
// ./sortArchivoConFlujos2 flujosMendeley.txt sortflujosMendeley.txt
// ./sortArchivoConFlujos2 flujosChicago-20160121.txt sortflujosChicago-20160121.txt
// ./sortArchivoConFlujos2 flujosMawi-20181201.txt sortflujosMawi-20181201.txt
// ./sortArchivoConFlujos2 flujosSanjose-20081016.txt sortflujosSanjose-20081016.txt
// ./sortArchivoConFlujos2 flujosChicago2015.txt > sortflujosChicago2015_2.txt

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

bool compararPorPayload(const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
    return a.first > b.first;
}

int main(int argc, char*argv[]){
    string archivoEntrada = argv[1];
    ifstream inFile(archivoEntrada);

    map<int64_t, int64_t> datos;
    string linea;
    while (getline(inFile, linea)) {
        istringstream iss(linea);
        int64_t payload, flujo;

        if (iss >> payload >> flujo) {
            datos[flujo] += payload;
        }
    }
    inFile.close();

    std::vector<std::pair<int64_t, int64_t>> vectorDatos;


    // Llena el vector con los datos del mapa
    for (const auto& par : datos) {
        vectorDatos.push_back(std::make_pair(par.second, par.first));
    }

    
    // Ordena el vector según los payloads
    std::sort(vectorDatos.begin(), vectorDatos.end(), compararPorPayload);
    for (const auto& par : vectorDatos) {
        std::cout << par.first << " " << par.second << std::endl;
    }

    return 0;
}

