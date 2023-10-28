// g++ maptest.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o maptest
// ./maptest numElementos             
// ./maptest 1000
// gdb -ex=r --args maptest Chicago-20080319.klle0.005000d0.001000c0.666660mk20.bin 100
// gdb -ex=r --args maptest Chicago-20080319.mrlk200.bin 100

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
#include <map>

#include "streamsketch.hpp"

using namespace std;

int main(int argc, char*argv[]){
    uint64_t n = stoull(argv[1]);
    uint64_t k = 10;

    map<double,uint64_t> mapa;

    for(int i=0;i<n;i++){
        double valorActual = rand()%1000;
        if (mapa.find(valorActual) != mapa.end()) {
            // Si la clave ya existe, incrementar la frecuencia
            mapa[valorActual]++;
        } else {
            // Si la clave no existe, agregar una nueva entrada con frecuencia 1
            mapa[valorActual] = 1;
        }
    }

    // Imprimir la frecuencia de los elementos
    for (const auto& entry : mapa) {
        std::cout << "Value: " << entry.first << ", Frequency: " << entry.second << std::endl;
    }

    // Crear un multimap ordenado por frecuencia (en orden descendente)
    std::multimap<uint64_t, double, std::greater<uint64_t>> sortedFrequencyMap;
    for (const auto& entry : mapa) {
        sortedFrequencyMap.insert(std::make_pair(entry.second, entry.first));
    }

    cout << endl << endl;
    // Imprimir los elementos en orden descendente según sus frecuencias
    // for (const auto& entry : sortedFrequencyMap) {
    //     std::cout << "Value: " << entry.second << ", Frequency: " << entry.first << std::endl;
    // }

    uint32_t count = 0;
    for (auto it = sortedFrequencyMap.begin(); it != sortedFrequencyMap.end() && count < k; ++it, ++count) {
        std::cout << "Frecuencia: " << it->first << ", Valor: " << it->second << std::endl;
    }


    return 0;
}