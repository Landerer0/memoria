//g++ -o kllex kllex.cpp -std=c++11 -I../datasketches-cpp-master/kll/include/ -I../datasketches-cpp-master/common/include/
// g++ norm.cpp -o norm
// ./norm media stdDeviation numElementos | sort -n -k2 -r | head


#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <map>
#include <string>

using namespace std;

int main(int argc, char **argv) {
    //std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine generator;
    generator.seed(0);
    std::normal_distribution<> d(stoi(argv[1]),stoi(argv[2])); 

    map<int, uint64_t> mp;
    map<int, uint64_t>::iterator m;
    for (int i = 0; i < 1000000; i++) {
	    int x = d(generator);
	    m = mp.find(x);
	    if(m != mp.end())
		    mp[x]++;
	    else
		    mp[x] = 1;
           // cout<<d(generator)<<endl; // mean=0, stddev=1
    }
    for(m=mp.begin(); m!=mp.end(); m++)
	    cout<<m->first<<": "<<m->second<<endl;


  }

