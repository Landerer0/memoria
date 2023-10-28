// g++ kllSave.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o kllSave
// ./kllSave archivoTraza epsilon delta c minK              // KLL 
// ./kllSave archivoTraza mink                              // MRL 
// ./kllSave Chicago-20080319.txt 0.005 0.001 0.66666 20
// ./kllSave Chicago-20080319.txt 200
// gdb -ex=r --args kllSave Chicago-20080319.txt 0.005 0.001 0.66666 20
// gdb -ex=r --args kllSave Chicago-20080319.txt 200

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

uint32_t numLineasArchivo(string archivo){
    fstream in(archivo);
    int n = 0;
    int lines = 0;
    char endline_char = '\n';
    while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
    {
        ++lines;
    }
    n = lines-1;
    cout << "En en el archivo " << archivo << " hay " << n << " lineas" << endl;
    return n;
}

int main(int argc, char*argv[]){
    
    if(argc!=3&&argc!=6){
        cout << "no se proporcionaron los parametros correctos" << endl;
        return 0;
    }

    string archivoActualTxt = "";
    string archivoActual = "";

    string carpetaPreprocesada = "data/";
    string carpetaPreprocesadaOrdenada = "orden/";
    cout << argv[1] << endl;

    // Definición de nombres de archivos a utilizar
    archivoActualTxt = argv[1];
    archivoActual = archivoActualTxt;
    size_t posicion = archivoActual.find(".txt");
    if (posicion != std::string::npos) {
        archivoActual.erase(posicion, 4); // Eliminamos 4 caracteres: .txt
    }
    cout << archivoActualTxt << endl;

    archivoActualTxt = carpetaPreprocesada;
    archivoActualTxt = archivoActualTxt.append(archivoActual);
    archivoActualTxt = archivoActualTxt.append(".txt");

    cout << "archivo: " << archivoActual << endl;
    cout << "archivo Txt: " << archivoActualTxt << endl;

    uint64_t n = numLineasArchivo(archivoActualTxt);
    // double epsilon, delta, c;
    // uint64_t minK;

    KLL kll = (argc==3)? KLL(stoi(argv[2])):KLL(n, stod(argv[2]), stod(argv[3]), stod(argv[4]), stoi(argv[5]));

    std::ifstream archivo(archivoActualTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                kll.add(dato); 
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
        return 0;
    }

    string nombreKLL = "";
    string nombreCarpetaAlmacenada = "saveKll";
    nombreKLL.append(nombreCarpetaAlmacenada + "/");
    nombreKLL.append(archivoActual);
    if(kll.isAnMrl()){
        nombreKLL.append(".mrlk");
        nombreKLL.append(to_string(stoi(argv[2])));
    } else{
        nombreKLL.append(".klle");
        nombreKLL.append(to_string(stod(argv[2])));
        nombreKLL.append("d");
        nombreKLL.append(to_string(stod(argv[3])));
        nombreKLL.append("c");
        nombreKLL.append(to_string(stod(argv[4])));
        nombreKLL.append("mk");
        nombreKLL.append(to_string(stoi(argv[5])));
    }

    cout << "kll se guardara en " << nombreKLL << endl;
    kll.saveData(nombreKLL);

    cout << "KLL print" << endl;
    kll.print();

    //cout << "readData de " << nombreKLL << endl;
    KLL kllread = KLL(200).readData(nombreKLL+".bin");

    cout << "KLL Read print" << endl;
    kllread.print();

    if(kll.areEqual(kllread)) cout << "kll y kll almacenado en binario son iguales" << endl;
    else cout << "kll y kll almacenado en binario son distintos" << endl;

    return 0;
}
