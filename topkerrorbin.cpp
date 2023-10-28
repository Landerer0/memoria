// g++ topkerrorbin.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o topkerrorbin
// ./topkerrorbin archivoTraza nombreBinario k             
// ./topkerrorbin Mendeley.txt Mendeley.klle0.005000d0.001000c0.666660mk20.bin 20
// ./topkerrorbin Mendeley.txt Mendeley.mrlk200.bin 20
// ./topkerrorbin Chicago-20080319.txt Chicago-20080319.klle0.005000d0.001000c0.666660mk20.bin 20
// ./topkerrorbin Chicago-20080319.txt Chicago-20080319.mrlk200.bin 20
// gdb -ex=r --args topkerrorbin Chicago-20080319.klle0.005000d0.001000c0.666660mk20.bin 100
// gdb -ex=r --args topkerrorbin Chicago-20080319.mrlk200.bin 100

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
#include <map>
#include <inttypes.h>

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

void almacenarDatosTopk(string nombreBinario, vector<double> estimacionTopk,string k, KLL kll){

    string nombreArchivo="";

    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultadosAGraficarTopk/";

    string archivoGuardado = nombreCarpeta+nombreBinario+"K"+k+".txt";
    // Nombre del archivo de salida
    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(archivoGuardado);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : estimacionTopk) {
            outfileParametros << element << " ";
        }
        outfileParametros << kll.sizeInBytes();
        outfileParametros << std::endl;
        // Cerrar el archivo
        outfileParametros.close();
        std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << archivoGuardado << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << archivoGuardado << " para escritura." << std::endl;
    }
    
    return;
}

int main(int argc, char*argv[]){
    if(argc!=4){
        cout << "error en la entrega de parametros" << endl;
        return 0;
    }
    
    string archivoActualTxt = "";
    string archivoActual = "";

    string carpetaPreprocesada = "data/";
    string carpetaPreprocesadaOrdenada = "orden/";

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
    map<double,uint64_t> mapa;

    std::ifstream archivo(archivoActualTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                if (mapa.find(dato) != mapa.end()) {
                    // Si la clave ya existe, incrementar la frecuencia
                    mapa[dato]++;
                } else {
                    // Si la clave no existe, agregar una nueva entrada con frecuencia 1
                    mapa[dato] = 1;
                }
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
        return 0;
    }

    string nombreCarpeta = "saveKll";
    string nombreBinario = "";
    nombreBinario.append(nombreCarpeta+"/");
    nombreBinario.append(argv[2]);
    uint32_t k = stoi(argv[3]);

    KLL kll(200);
    kll = kll.readData(nombreBinario);

    vector<pair<double,uint64_t>> topK = kll.topKElementsFrequency(k);

    cout << "para k: " << k << " topK elements:" << endl;
    printf("\tk\telemento\tfrecuenciaKLL\t\tfrecuenciaReal\tErrorRelativo\n");
    double errorRelativoTotal = 0;
    vector<double> errorEstimacionTopk;
    for(int i=0;i<topK.size();i++){
        auto it = mapa.find(topK.at(i).first);
        double errorRelativo = abs((double)it->second-(double)topK.at(i).second)/(double)it->second;
        errorRelativoTotal+=errorRelativo;
        errorEstimacionTopk.push_back(errorRelativo);
        printf("\t%d\t%lf\t%" PRId64 "\t\t%" PRId64 "\t\t%lf\n",i+1,topK.at(i).first,topK.at(i).second, it->second,errorRelativo);
        //if(topK.at(i)==3232235891.000000) cout << endl << "SE ENCONTRO 3232235891.000000" << endl; // para mendeley
    }
    errorRelativoTotal/=k;
    cout << endl;

    cout << endl << "Error Relativo Final: " << errorRelativoTotal << endl;

    almacenarDatosTopk(kll.binarySaveName(archivoActual), errorEstimacionTopk,argv[3], kll);

    return 0;
}