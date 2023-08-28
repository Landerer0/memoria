// g++ pruebaStreamSketchSintetico.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -o pruebaStreamSketchSintetico
// ./pruebaStreamSketchSintetico numElementos
// ./pruebaStreamSketchSintetico 1000000
// gdb -ex=r --args pruebaStreamSketchSintetico 1000000

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
    StreamSketch kll(n/4, epsilon, delta, c, minK);
    StreamSketch mrl(200);
    StreamSketch mrlEspacio(espacio,n/4);
    StreamSketch kllEpsilon(n/4, epsilon, c, minK);
    StreamSketch mrlEpsilon(0.01,n/4);
    
    for(uint64_t i=0;i<n;i++){
        kll.add(rand()%1000);
        mrl.add(rand()%1000);
        mrlEspacio.add(rand()%1000);
        kllEpsilon.add(rand()%1000);
        mrlEpsilon.add(rand()%1000);
        //cout << "n:" << i << endl;
    } 
    
    cout << "PRINT KLL" << endl;
    kll.print();
    cout << "PRINT MRL" << endl;
    mrl.print();
    cout << "PRINT MRL_EPSPACIO" << endl;
    mrlEspacio.print();
    cout << "PRINT KLL_EPSILON" << endl;
    kllEpsilon.print();
    cout << "PRINT MRL_EPSILON" << endl;
    mrlEpsilon.print();

    return 0;
}