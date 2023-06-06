//g++ -o kllex kllex.cpp -std=c++11 -I../datasketches-cpp-master/kll/include/ -I../datasketches-cpp-master/common/include/
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <map>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    std::default_random_engine generator;
    generator.seed(0);
    std::exponential_distribution<> d(stof(argv[1])); 
    int nelems = stoi(argv[2]);

    for (int i = 0; i < nelems; i++) {
            cout<<(int)d(generator)<<endl; // mean=0, stddev=1
    }
    

  }

