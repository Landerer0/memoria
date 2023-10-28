// g++ visualizacionMerge.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o visualizacionMerge
// ./visualizacionMerge numElementos             
// ./visualizacionMerge numElementos numElementos 2             
// ./visualizacionMerge 6      
// ./visualizacionMerge 6 10
// gdb -ex=r --args visualizacionMerge 14 15

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

#include "streamsketch.hpp"

using namespace std;

int main(int argc, char*argv[]){
    if(!(argc==2||argc==3)) return 0;

    uint64_t numTotalElementos1 = stoull(argv[1]);
    uint64_t numTotalElementos2;
    if(argc==3) numTotalElementos2 = stoull(argv[2]);
    else numTotalElementos2 = numTotalElementos1;

    uint64_t n1 = numTotalElementos1;
    uint64_t n2 = numTotalElementos2;

    uint64_t H1 = 2;
    uint64_t s1 = 1;
    uint64_t H_pp1 = 1;
    vector<int> sizeNiveles1 = {4,5};

    uint64_t H2 = 2;
    uint64_t s2 = 1;
    uint64_t H_pp2 = 0;
    vector<int> sizeNiveles2 = {2,3,5};

    uint64_t H_ppMrl = 0;
    uint64_t sMrl = 1;

    uint64_t H1Mrl = 1;
    vector<int> sizeNivelesMrl1 = {5,5};

    uint64_t H2Mrl = 1;
    vector<int> sizeNivelesMrl2 = {4,4};

    bool isAnMrl = true;
    bool isAnKll = false;
    bool hasLimitedSpace = true;

    cout << "kll1:" << endl;
    KLL kll1(H1, s1, H_pp1, sizeNiveles1, isAnKll, !hasLimitedSpace);
    KLL mrl1(H1Mrl, sMrl, H_ppMrl, sizeNivelesMrl1, isAnMrl, !hasLimitedSpace);
    // kll1.print();
    // mrl1.print();

    cout << endl << "kll2:" << endl;
    KLL kll2(H2, s2, H_pp2, sizeNiveles2, isAnKll, !hasLimitedSpace);
    KLL mrl2(H2Mrl, sMrl, H_ppMrl, sizeNivelesMrl2, isAnMrl, !hasLimitedSpace);
    // kll2.print();
    // mrl2.print();

    for(int i=0;i<n1;i++){
        // cerr << i << endl;
        // mrl1.add(rand()%20);
        kll1.add(rand()%20);
    }
    
    for(int i=0;i<n2;i++){
        // cerr << i << endl;
        // mrl2.add(rand()%20);
        kll2.add(rand()%20);
    }
    // cout << endl << "kll1 resultados:" << endl;
    // kll1.print();

    // cout << endl << "kll2 resultados:" << endl;
    // kll2.print();

    cout << "Merge:" << endl << endl;
    KLL kllmerge = kll1.kllMerge(kll2);
    kllmerge.print();
    // KLL mrlmerge = mrl1.kllMerge(mrl2);
    // mrlmerge.print();
    // cout << endl << "kllMerge resultados:" << endl;
}