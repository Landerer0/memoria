// g++ metrics.cpp -o metrics
// ./metrics reales.txt estimado.txt
// ./metrics top256.txt estimacionFrecuencias/H_pp2Espacio27936flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk71235936h7.txt
// ./metrics top256.txt estimacionFrecuencias/H_pp2Espacio106312flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk71235936h7.txt
// ./metrics top256.txt estimacionFrecuencias/H_pp1Espacio29544flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk2196637392h7.txt
// ./metrics top256.txt estimacionFrecuencias/H_pp1Espacio114320flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk2196637392h7.txt

#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

map<uint64_t, uint64_t> fpreal;
map<uint64_t, uint64_t> fpest;
map<uint64_t, uint64_t>::iterator mp,mp1;

int main(int argc, char *argv[]){
	ifstream file(argv[1]);
	if (file.is_open()) {
		string line;
        	while (std::getline(file, line)) {
            		std::pair<int64_t, int64_t> pair;
            		std::istringstream iss(line);
            		if (iss >> pair.first >> pair.second) {
				fpreal[pair.second] = pair.first;
				//cout<<" first "<<pair.first<<" second "<<fpay[pair.first]<<endl;
			}

            	}
        }
        file.close();
	ifstream file2(argv[2]);
	if (file2.is_open()) {
		string line;
        	while (std::getline(file2, line)) {
            		std::pair<int64_t, int64_t> pair;
            		std::istringstream iss(line);
            		if (iss >> pair.first >> pair.second) {
				fpest[pair.second] = pair.first;
			}

            	}
        }
        file2.close();
	uint64_t  tp=0, fp=0, tn=0, fn=0;
	for(mp=fpreal.begin(); mp!= fpreal.end(); mp++){
		mp1 = fpest.find(mp->first);
		if(mp1 != fpest.end()){
			tp++;
		}else {
			fn++;
			cout<<" falso negativo payload flujo "<<mp->second<<" "<<mp->first<<endl;
		}
	}
	for(mp=fpest.begin(); mp!= fpest.end(); mp++){
		mp1 = fpreal.find(mp->first);
		if(mp1 == fpreal.end()){
			fp++;
		}
	}
	
	cout<<" tp "<<tp <<" fn "<<fn<<" fp "<<fp<<endl;

}
