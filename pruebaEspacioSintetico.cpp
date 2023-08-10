// g++ pruebaEspacioSintetico.cpp kll.hpp kll.cpp -o pruebaEspacioSintetico
// ./pruebaEspacioSintetico numElementos
// ./pruebaEspacioSintetico 1000000

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
        cout << "indicar numero de elementos" << endl;
        return 1;
    }
    uint64_t numTotalElementos = stoull(argv[1]);

    uint64_t n = numTotalElementos;

    // DEFINICION DE PARAMETROS
    double epsilon = 0.05;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    //KLL kll(n,epsilon,delta,c,20);
    //KLL kllkmin(200);
    uint64_t espacio = 20000;
    KLL mrlEspacio(espacio,n);
    
    for(uint64_t i=0;i<n;i++) mrlEspacio.add(rand()%1000);
    
    cout << "size mrlEspacio: " << mrlEspacio.sizeInBytes() << endl;
    //mrlEspacio.print();

    return 0;
}