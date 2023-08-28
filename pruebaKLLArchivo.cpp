// g++ pruebaKLLArchivo.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -o pruebaKLLArchivo
// ./pruebaKLLArchivo archivoTxt numParticiones
// ./pruebaKLLArchivo data/Mendeley.txt 4
// gdb -ex=r --args pruebaKLLArchivo data/Mendeley.txt 4

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

#include "streamsketch.hpp"

using namespace std;

int main(int argc, char*argv[]){
    if(argc!=3){
        cout << "indicar nombre archivo" << endl;
        return 1;
    }
    string nombreArchivo = argv[1];
    int numFractions = stoi(argv[2]);

    uint64_t n;

    fstream in(nombreArchivo);
        uint64_t lines = 0;
        char endline_char = '\n';
        while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
        {
            ++lines;
        }
        n = lines-1;
        cout << "En en el archivo " << nombreArchivo << " hay " << n << " lineas" << endl;
    
    // DEFINICION DE PARAMETROS
    double epsilon = 0.01;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    KLL kll(n,epsilon,delta,c,20);
    vector<KLL> kllParticionado;
    for(int i=0;i<numFractions;i++) kllParticionado.push_back(KLL(n/numFractions,epsilon,delta,c,20));
    //KLL kll(n,epsilon,delta,c,20);
    //KLL kllkmin(200);
    std::ifstream archivo(nombreArchivo);
    std::string linea;
    uint64_t lineaActual=0;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) 
                kll.add(dato);
                if(lineaActual<(n/4)) kllParticionado.at(0).add(dato);
                else if(lineaActual<(n/2)) kllParticionado.at(1).add(dato);
                else if(lineaActual<(3*n/4)) kllParticionado.at(2).add(dato);
                else if(lineaActual<(n)) kllParticionado.at(3).add(dato);
                lineaActual++;
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return 0;
    }

    cout << "KLL Normal" << endl;
    kll.print();
    // cout << endl << "KLL Parte 1" << endl;
    // kllParticionado.at(0).print();
    // cout << endl << "KLL Parte 2" << endl;
    // kllParticionado.at(1).print();
    // cout << endl << "KLL Parte 3" << endl;
    // kllParticionado.at(2).print();
    // cout << endl << "KLL Parte 4" << endl;
    // kllParticionado.at(3).print();
    cerr << "MERGEEEE" << endl << endl << endl;
    KLL kllFinal = kllParticionado.at(0).kllMerge(kllParticionado.at(1));
    cout << endl << "MERGE KLL Parte 1" << endl;
    kllFinal.print();
    // kllFinal = kllFinal.kllMerge(kllParticionado.at(2));
    // cout << endl << "MERGE KLL Parte 2" << endl;
    // kllFinal.print();
    // kllFinal = kllFinal.kllMerge(kllParticionado.at(3));
    // cout << endl << "KLL Merge FINAL" << endl;
    // kllFinal.print();

    // vector<double> quantile={10,20,30,40,50,60,70,80,90};
    
    // vector<double> quantileKll = kll.quantile(quantile);
    // vector<double> quantileKllFinal = kllFinal.quantile(quantile);

    // for(int i=0;i<quantileKll.size();i++){
    //     cerr << "quantile " << i+1 << ", kll: " << quantileKll.at(i) << ", kllMerge: " << quantileKllFinal.at(i) << endl;
    // }

    // vector<double> ranks = {0,49, 499, 4999, 9999};
    // vector<double> quantiles = {1,10,25, 50, 75, 90,99};
    // vector<uint64_t> resultadoRanks = kll.rank(ranks);
    // vector<double> resultadoQuantiles = kll.quantile(quantiles);

    
    // cout << "Merge Result MRL (merge consigo mismo):" << endl;
    // KLL mergekllkmin = kllkmin.kllMerge(kllkmin);
    // cout << "print:" << endl;
    // mergekllkmin.print();   

    // cerr << "KLL Tradicional:" << endl;
    // kll.print();
    // cout << "Merge Result KLL Tradicional (merge consigo mismo):" << endl;
    // KLL mergekll = kll.kllMerge(kll);
    // cout << "print:" << endl;
    // mergekll.print();

    // cout << "Estimación num Elementos en rango [1307777537,2232238616] es: " << kll.numElementosRango(1307777537,2232238616) << endl;
    // cout << "Estimación num Elementos en rango [2607777537,3532238616] es: " << kll.numElementosRango(2607777537,3532238616) << endl;


    // cout << endl << "SAVE AND LOAD OF KLL WITH BINARY FILE" << endl << endl;
    // kll.print();
    // kll.saveData("kll.bin");
    // KLL kllres = kll.readData("kll.bin");
    // if(kll.areEqual(kllres)) cout << "KLL y KLL.bin son iguales" << endl;
    // else cout << "KLL y KLL.bin NO son iguales" << endl;
    // cout << "KLL" << endl;
    // kll.print();
    // cout << "KLL res" << endl;
    // kllres.print();
    
    // cout << endl << "MRL" << endl << endl;
    // kllkmin.print();
    // kllkmin.saveData("kllkmin.bin");
    // KLL kllkminres = kllkmin.readData("kllkmin.bin");
    // if(kllkmin.areEqual(kllkminres)) cout << "MRL y MRL.bin son iguales" << endl;
    // else cout << "KLL y KLL.bin NO son iguales" << endl;
    // cout << "KLLKMIN" << endl;
    // kllkmin.print();
    // cout << "KLLkmin res" << endl;
    // kllkminres.print();

    return 0;
}