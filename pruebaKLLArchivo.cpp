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
    
    vector<double> ranks = {0,49, 499, 4999, 9999};
    vector<double> quantiles = {1,10,25, 50, 75, 90,99};
    vector<uint64_t> resultadoRanks = kll.rank(ranks);
    vector<double> resultadoQuantiles = kll.quantile(quantiles);

    
    cout << "Merge Result MRL (merge consigo mismo):" << endl;
    KLL mergekllkmin = kllkmin.kllMerge(kllkmin);
    cout << "print:" << endl;
    mergekllkmin.print();   

    cerr << "KLL Tradicional:" << endl;
    kll.print();
    cout << "Merge Result KLL Tradicional (merge consigo mismo):" << endl;
    KLL mergekll = kll.kllMerge(kll);
    cout << "print:" << endl;
    mergekll.print();

    cout << "Estimación num Elementos en rango [1307777537,2232238616] es: " << kll.numElementosRango(1307777537,2232238616) << endl;
    cout << "Estimación num Elementos en rango [2607777537,3532238616] es: " << kll.numElementosRango(2607777537,3532238616) << endl;


    cout << endl << "SAVE AND LOAD OF KLL WITH BINARY FILE" << endl << endl;
    kll.print();
    kll.saveData("kll.bin");
    KLL kllres = kll.readData("kll.bin");
    if(kll.areEqual(kllres)) cout << "KLL y KLL.bin son iguales" << endl;
    else cout << "KLL y KLL.bin NO son iguales" << endl;
    cout << "KLL" << endl;
    kll.print();
    cout << "KLL res" << endl;
    kllres.print();
    
    cout << endl << "MRL" << endl << endl;
    kllkmin.print();
    kllkmin.saveData("kllkmin.bin");
    KLL kllkminres = kllkmin.readData("kllkmin.bin");
    if(kllkmin.areEqual(kllkminres)) cout << "MRL y MRL.bin son iguales" << endl;
    else cout << "KLL y KLL.bin NO son iguales" << endl;
    cout << "KLLKMIN" << endl;
    kllkmin.print();
    cout << "KLLkmin res" << endl;
    kllkminres.print();

    return 0;
}