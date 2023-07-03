// g++ pruebaKLL.cpp kll.hpp kll.cpp -o pruebaKLL
// ./pruebaKLL media stdDeviation
// ./pruebaKLL 1000 500

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
    if(argc!=3){
        cout << "Ingresar media y desviacion estandar" << endl;
        return 1;
    }
    uint64_t n = 10000;
    // DEFINICION DE PARAMETROS
    double epsilon = 0.05;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    KLL kll1(n,epsilon,delta,c,3);
    KLL kll2(n,epsilon/10.0,delta,c,4);
    KLL kllkmin1(150);
    KLL kllkmin2(200);

    std::default_random_engine generator;
    generator.seed(0);

    double media = stod(argv[1]);
    double desviacion = stod(argv[2]);

    std::normal_distribution<> d{media, desviacion};
    for(int i=0;i<n;i++){
        double toAdd = d(generator);
        kll1.add(toAdd);
        kll2.add(toAdd);
        kllkmin1.add(toAdd);
        kllkmin2.add(toAdd);
    }

    cout << endl << "KLL1: " << endl << endl;
    cout << kll1.sizeInBytes() << endl;
    kll1.print();
    cout << endl << "KLL2: " << endl << endl;
    cout << kll2.sizeInBytes() << endl;
    kll2.print();

    cout << endl << "MERGE (KLL): " << endl;
    KLL kll3=kll1.kllMerge(kll2);
    cout << endl << "KLL3: " << endl << endl;
    cout << kll2.sizeInBytes() << endl;
    kll2.print();

    cout << endl << "MRL1: " << endl << endl;
    cout << kllkmin1.sizeInBytes() << endl;
    kllkmin1.print();
    cout << endl << "MRL2: " << endl << endl;
    cout << kllkmin2.sizeInBytes() << endl;
    kllkmin2.print();

    cout << endl << "MERGE (MRL): " << endl;
    KLL kllkmin3=kllkmin1.kllMerge(kllkmin2);
    cout << endl << "MRL3: " << endl << endl;
    cout << kllkmin2.sizeInBytes() << endl;
    kll2.print();

    return 0;
}