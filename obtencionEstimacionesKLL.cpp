// g++ obtencionEstimacionesKLL.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o obtencionEstimacionesKLL
// ./obtencionEstimacionesKLL archivoTraza              // GENERAL
// nohup ./obtencionEstimacionesKLL Mendeley.txt &
// nohup ./obtencionEstimacionesKLL Chicago-20160121.txt &
// nohup ./obtencionEstimacionesKLL Sanjose-20081016.txt &
// nohup ./obtencionEstimacionesKLL Mawi-20181201.txt &

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

void almacenarDatos(string nombreBinario, vector<pair<double,uint64_t>> estimacionTopk, map<uint64_t,uint64_t> &data, KLL kll){

    string nombreArchivo="";

    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "estimacionFrecuencias/";

    string archivoGuardado = nombreCarpeta+nombreBinario+".txt";
    // Nombre del archivo de salida
    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(archivoGuardado);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : estimacionTopk) {
            outfileParametros << element.second << " " << data[(uint64_t)element.first] << " " << (uint64_t)element.first << endl;
        }
        outfileParametros << kll.sizeInBytes();
        outfileParametros << std::endl;
        // Cerrar el archivo
        outfileParametros.close();
        std::cout << "Las estimaciones de frecuencia se han almacenado correctamente en el archivo " << archivoGuardado << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << archivoGuardado << " para escritura." << std::endl;
    }
    
    return;
}

int main(int argc, char*argv[]){
    vector<KLL> klls;

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

    // CREACION KLLS
    klls.push_back(KLL(100));
    klls.push_back(KLL(150));
    klls.push_back(KLL(200));
    klls.push_back(KLL(250));
    klls.push_back(KLL(300));
    klls.push_back(KLL(n,(double)0.05,(double)0.001000,(double)0.5,20));
    klls.push_back(KLL(n,(double)0.01,(double)0.001000,(double)0.5,20));
    klls.push_back(KLL(n,(double)0.005,(double)0.001000,(double)0.5,20));
    klls.push_back(KLL(n,(double)0.0075,(double)0.001000,(double)0.5,20));
    klls.push_back(KLL(n,(double)0.001,(double)0.001000,(double)0.5,20));
    klls.push_back(KLL(n,(double)0.05,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLL(n,(double)0.01,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLL(n,(double)0.005,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLL(n,(double)0.0075,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLL(n,(double)0.001,(double)0.001000,(double)0.666666,20));
    
    // INGRESO DATOS KLLS
    std::ifstream archivo(archivoActualTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                for(int i=0;i<klls.size();i++){
                    klls.at(i).add(dato); 
                }
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
        return 0;
    }

    // GUARDADO DE KLLS
    string nombreKLL = "";
    string nombreKLLPrefix = "";
    string nombreCarpetaAlmacenada = "saveKll";
    nombreKLLPrefix.append(nombreCarpetaAlmacenada + "/");
    nombreKLLPrefix.append(archivoActual);
    for(int i=0;i<klls.size();i++){
        nombreKLL = "";
        nombreKLL.append(nombreKLLPrefix);
        if(klls.at(i).isAnMrl()){
            vector<double> parametros = klls.at(i).getMRLParameters();
            nombreKLL.append(".mrlk");
            nombreKLL.append(to_string((int)parametros.at(0))); // mink
        } else{
            vector<double> parametros = klls.at(i).getKLLParameters();
            nombreKLL.append(".klle");
            nombreKLL.append(to_string(parametros.at(0))); // epsilon
            nombreKLL.append("d");
            nombreKLL.append(to_string(parametros.at(1))); // delta
            nombreKLL.append("c");
            nombreKLL.append(to_string(parametros.at(2))); // c
            nombreKLL.append("mk");
            nombreKLL.append(to_string((int)parametros.at(3))); // mink
        }

        cout << "kll se guardara en " << nombreKLL << endl;
        klls.at(i).saveData(nombreKLL);
    }

    
    // calcular frecuencia real
    map<uint64_t,uint64_t> data;
    string archivoHistogramaElementos = "histogramas/datosHistogramaElementos"+archivoActual+".txt";
    ifstream file(archivoHistogramaElementos);
    string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        uint64_t frequency, element;
        if (!(iss >> frequency >> element)) { break; } // error

        data[element] = frequency;
    }

     // Imprimir los datos almacenados en el mapa
    pair<uint64_t, uint64_t> par;
    for (const auto& par : data) {
        std::cout << "Clave: " << par.first << ", Valor: " << par.second << '\n';
    }

    // OBTENCION TOP-K's y frecuencias
    for(int i=0;i<klls.size();i++){

        vector<pair<double,uint64_t>> estimaciones;
        estimaciones = klls.at(i).estimateElementsFrequency();
        almacenarDatos(klls.at(i).binarySaveName(archivoActual),estimaciones,data,klls.at(i));
    }


    return 0;
}
