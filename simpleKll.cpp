// g++ simpleKll.cpp kll.hpp kll.cpp -o simpleKll
// ./simpleKll numElementos
// ./simpleKll 1000000

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

    
    KLL kllCte(n/8, epsilon, delta, c, minK, true);
    kllCte.print();
    cout << endl << endl;
    // KLL kll(n, epsilon, delta, c, minK);
    // KLL kllEpsilon(n, epsilon, c, minK);
    // KLL mrl(200);
    // KLL mrlEspacio(espacio,n);
    // KLL mrlEpsilon(0.01,n);
    // //KLL kll(n,epsilon,delta,c,20);
    // //KLL kllkmin(200);
    
    for(uint64_t i=0;i<n;i++){
         if(kllCte.add(rand()%1000)){
            cout << "se lleno kll en " << i << endl;
            kllCte.print();
         }
    //     if(kll.add(rand()%1000)) cout << "se lleno kll" << endl;
    //     if(kllEpsilon.add(rand()%1000)) cout << "se lleno kllEpsilon" << endl;
    //     if(mrl.add(rand()%1000)) cout << "se lleno mrl" << endl;
    //     if(mrlEspacio.add(rand()%1000)) cout << "se lleno mrlEspacio" << endl;
    //     if(mrlEpsilon.add(rand()%1000)) cout << "se lleno mrlEpsilon" << endl;
    } 
    
    kllCte.print();
    cout << endl << endl;
    // kll.print();
    // cout << endl << endl;
    // kllEpsilon.print();
    // cout << endl << endl;
    // mrl.print();
    // cout << endl << endl;
    // mrlEspacio.print();
    // cout << endl << endl;
    // mrlEpsilon.print();
    // cout << endl << endl;

    return 0;
}