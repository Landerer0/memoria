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

    uint64_t espacio = 20000;
    double epsilon = 0.01;
    double delta = 0.01;
    double c = 2.0/3.0;
    int minK = 2;
    KLL kll(n, epsilon, delta, c, minK);
    KLL kllEpsilon(n, epsilon, c, minK);
    KLL mrlEspacio(espacio,n);
    KLL mrl(200);
    KLL mrlEpsilon(0.01,n);
    //KLL kll(n,epsilon,delta,c,20);
    //KLL kllkmin(200);
    
    for(uint64_t i=0;i<n;i++){
        //cerr << "elemento " << i << endl;
        //if(kll.add(rand()%1000)) cout << "se lleno kll" << endl;
        if(kllEpsilon.add(rand()%1000)) cout << "se lleno kll" << endl;

        //if(mrlEspacio.add(rand()%1000)) cout << "se lleno mrlEspacio" << endl;
        //if(mrl.add(rand()%1000)) cout << "se lleno mrl" << endl;
        //if(mrlEpsilon.add(rand()%1000)) cout << "se lleno mrlEpsilon" << endl;
    } 
    
    //cout << "size kll: " << kll.sizeInBytes() << endl;
    cout << "size kllEpsilon: " << kllEpsilon.sizeInBytes() << endl;
    //cout << "size mrlEspacio: " << mrlEspacio.sizeInBytes() << endl;
    //cout << "size mrl: " << mrl.sizeInBytes() << endl;
    //cout << "size mrlEpsilon: " << mrlEpsilon.sizeInBytes() << endl;
    //mrlEspacio.print();
    //kll.print();
    kllEpsilon.print();

    return 0;
}