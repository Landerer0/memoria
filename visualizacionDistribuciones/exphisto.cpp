//g++ -o kllex kllex.cpp -std=c++11 -I../datasketches-cpp-master/kll/include/ -I../datasketches-cpp-master/common/include/
// g++ exphisto.cpp -o exphisto
// ./exphisto lamda numElementos | sort -n -k2 -r | head

// almacena los valores de la distribucion en un mapa y luego lo imprime por pantalla

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

    map<int, uint64_t> mp;
    map<int, uint64_t>::iterator m;
    for (int i = 0; i < nelems; i++) {
	    
	    int x = (int)d(generator);
	    m = mp.find(x);
	    if(m != mp.end())
		    mp[x]++;
	    else
		    mp[x] = 1;
		    
    }
    
    for(m=mp.begin(); m!=mp.end(); m++)
	    cout<<m->first<<": "<<m->second<<endl;


  }

