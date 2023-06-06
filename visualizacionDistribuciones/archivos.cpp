// g++ archivos.cpp -o archivos
// ./archivos nombreArchivo | sort -n -k2 -r 
// ./archivos data/Mendeley-txt | sort -n -k2 -r > resMendeley.txt

// almacena los valores de la distribucion del archivo proporcionado y los muestra en consola

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <map>
#include <random>

using namespace std;

int main(int argc, char **argv) {
    std::ifstream inputFile(argv[1]); // Reemplaza "archivo.txt" con la ruta de tu archivo

    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    map<double, uint64_t> mp;
    map<double, uint64_t>::iterator m;
    std::string linea;
    uint64_t numElements = 0;

    while (std::getline(inputFile, linea)) {
        std::stringstream ss(linea);
        std::string elementoStr;

        while (ss >> elementoStr) {
            double x = std::stod(elementoStr);
            numElements++;
            m = mp.find(x);
            if(m != mp.end())
                mp[x]++;
            else
                mp[x] = 1;
        }
    }
    
    for(m=mp.begin(); m!=mp.end(); m++)
	    cout<<m->first<<": "<<m->second<<endl;
        
    

    inputFile.close();

    std::cout << "Cantidad de elementos en el archivo: " << numElements << std::endl;

    return 0;
}


