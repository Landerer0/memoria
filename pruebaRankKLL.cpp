// g++ pruebaRankKLL.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -o pruebaRankKLL
// ./pruebaRankKLL 

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
#include<inttypes.h>

using namespace std;

int main(int argc, char*argv[]){
    string nombreFichero = "saveKll/Mendeley.klle0.001000d0.001000c0.666666mk20.bin";
    KLL kll(200);
    kll = kll.readData(nombreFichero);

    double X1 = 3756459726;
    double X2 = 3715900684;
    uint64_t rank1 = kll.rank(X1);
    uint64_t rank2 = kll.rank(X2);
    printf("x %"PRIu64" %lf\n",rank1,X1);
    printf("x %"PRIu64" %lf\n",rank2,X2);
    cout << "Estimacion frecuencia: " << rank1-rank2 << endl;

}