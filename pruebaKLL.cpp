// g++ pruebaKLL.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -o pruebaKLL
// ./pruebaKLL numElementos
// ./pruebaKLL 100000
// gdb -ex=r --args pruebaKLL 100000

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
        cout << "Ingresar numElementos" << endl;
        return 1;
    }

    // DEFINICION DE PARAMETROS
    uint64_t n = stoi(argv[1]);
    double epsilon = 0.01;
    double delta = 0.001;
    double c = (double) 2/(double) 3;
    int minK = 6;
    bool samplerEnMenorNivel = true;
    
    uint64_t H,s,H_pp;
    bool isAnMrl, hasLimitedSpace;
    vector<int> sizeNiveles;
    H=2;
    s=1;
    H_pp=0;
    isAnMrl = true;
    hasLimitedSpace = true;
    sizeNiveles.push_back(5);
    sizeNiveles.push_back(5);
    sizeNiveles.push_back(5);
    //KLL kllArtificial(H,s, H_pp, sizeNiveles, isAnMrl, hasLimitedSpace); // MRL/KLL que se le indican los parametros que va a ocupar

    cout << endl << endl << endl;

    //KLL kll(n,epsilon,delta,c,3);
    //KLL kllCte(n/8, epsilon,delta, c, minK, samplerEnMenorNivel);
    
    for(int i=0;i<n;i++){
        //if(kllArtificial.add(rand()%1000)) break;
        //kll.add(rand()%1000);
        //kllCte.add(rand()%1000);
    }

    cout << "Finalizo de agregar elementos" << endl;
    //kllArtificial.print();
    //kll.print();
    //kllCte.print();

    // KLL kll1(n,epsilon,delta,c,3);
    // KLL kll2(n,epsilon/10.0,delta,c,4);
    // KLL kllkmin1(150);
    // KLL kllkmin2(200);

    // std::default_random_engine generator;
    // generator.seed(0);

    // double media = stod(argv[1]);
    // double desviacion = stod(argv[2]);

    // std::normal_distribution<> d{media, desviacion};
    // for(int i=0;i<n;i++){
    //     double toAdd = d(generator);
    //     kll1.add(toAdd);
    //     kll2.add(toAdd);
    //     kllkmin1.add(toAdd);
    //     kllkmin2.add(toAdd);
    // }

    // cout << endl << "KLL1: " << endl << endl;
    // cout << kll1.sizeInBytes() << endl;
    // kll1.print();
    // cout << endl << "KLL2: " << endl << endl;
    // cout << kll2.sizeInBytes() << endl;
    // kll2.print();

    // cout << endl << "MERGE (KLL): " << endl;
    // KLL kll3=kll1.kllMerge(kll2);
    // cout << endl << "KLL3: " << endl << endl;
    // cout << kll2.sizeInBytes() << endl;
    // kll2.print();

    // cout << endl << "MRL1: " << endl << endl;
    // cout << kllkmin1.sizeInBytes() << endl;
    // kllkmin1.print();
    // cout << endl << "MRL2: " << endl << endl;
    // cout << kllkmin2.sizeInBytes() << endl;
    // kllkmin2.print();

    // cout << endl << "MERGE (MRL): " << endl;
    // KLL kllkmin3=kllkmin1.kllMerge(kllkmin2);
    // cout << endl << "MRL3: " << endl << endl;
    // cout << kllkmin2.sizeInBytes() << endl;
    // kll2.print();

    return 0;
}