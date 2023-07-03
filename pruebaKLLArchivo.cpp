// g++ pruebaKLLArchivo.cpp kll.hpp kll.cpp -o pruebaKLLArchivo
// ./pruebaKLLArchivo data/Mendeley.txt

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

    KLL kll(n,epsilon,delta,c,20);
    KLL kllkmin(200);
    std::ifstream archivo(nombreArchivo);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) 
                kll.add(dato);
                kllkmin.add(dato);
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return 0;
    }
    /*
    vector<uint64_t> vectorRank = {10,20};
    vector<double> vectorDouble = {10.0,20.0};
    uint64_t kll1rank = kll1.rank(50.0); 
    vector<uint64_t> vectorkll1rank = kll1.rank(vectorDouble); 
    cout << "KLL Rank: " << kll1rank << endl;
    */
    /*
    cout << kll1.sizeInBytes() << endl;
    kll1.print();
    */
    
    vector<double> ranks = {0,49, 499, 4999, 9999};
    vector<double> quantiles = {1,10,25, 50, 75, 90,99};
    vector<uint64_t> resultadoRanks = kll.rank(ranks);
    vector<double> resultadoQuantiles = kll.quantile(quantiles);

    /*
    cout << "RANKS INDIVIDUAL" << endl;
    for(int i = 0;i<ranks.size();i++){
        cout << "Rank " << i <<": " << kll.rank(ranks[i]) << endl;
    }
    cout << "RANKS GRUPAL" << endl;
    for(int i = 0;i<resultadoRanks.size();i++){
        cout << "Rank " << i <<": " << resultadoRanks[i] << endl;
    }
    cout << "QUANTILES INDIVIDUAL" << endl;
    for(int i = 0;i<quantiles.size();i++){
        cout << "Quantile " << i <<": " << kll.quantile(quantiles[i]) << endl;
    }
    cout << "QUANTILES GRUPAL" << endl;
    for(int i = 0;i<resultadoQuantiles.size();i++){
        cout << "Quantile " << i <<": " << resultadoQuantiles[i] << endl;
    }
    */
    
    cout << endl << "KLL" << endl << endl;
    kll.print();
    kll.saveData("kll.bin");
    KLL kllres = kll.readData("kll.bin");
    cout << "KLL res" << endl;
    kllres.print();
    
    cout << endl << "MRL" << endl << endl;
    kllkmin.print();
    kllkmin.saveData("kllkmin.bin");
    KLL kllkminres = kllkmin.readData("kllkmin.bin");
    cout << "KLLkmin res" << endl;
    kllkminres.print();

    return 0;
}