// g++ reuExpress.cpp minheap.cpp klltuple.cpp -o reuExpress
// ./reuExpress archivoTraza              // GENERAL
// ./reuExpress paresBiasSimpleEscalon.txt
// ./reuExpress flujosMendeley.txt

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

#include "klltuple.hpp"

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

void almacenarDatos(string nombreBinario, vector<pair<int64_t,int64_t>> estimacionTopk, map<int64_t,int64_t> &data, KLLTuple kll){

    string nombreArchivo="";

    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "estimacionFrecuencias/";

    string archivoGuardado = nombreCarpeta+nombreBinario+".txt";
    // Nombre del archivo de salida
    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(archivoGuardado);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        // formato: [Flujo PayloadReal PayloadEstimado] 
        for (const auto& element : estimacionTopk) {
            outfileParametros << element.second << " " << data[element.second] << " " << element.first << endl;
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
    vector<KLLTuple> klls;

    string archivoActualTxt = "";
    string archivoActual = "";

    string carpetaPreprocesada = "";
    string carpetaPreprocesadaOrdenada = "";
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
    // klls.push_back(KLLTuple(100));
    // klls.push_back(KLLTuple(150));
    // klls.push_back(KLLTuple(200));
    // klls.push_back(KLLTuple(250));
    // klls.push_back(KLLTuple(300));
    
    // klls.push_back(KLLTuple(n,(double)0.05,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n,(double)0.01,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n,(double)0.005,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n,(double)0.0075,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n,(double)0.001,(double)0.001000,(double)0.5,20));

    // klls.push_back(KLLTuple(n,(double)0.05,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n,(double)0.01,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLLTuple(n,(double)0.005,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n,(double)0.0075,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n,(double)0.001,(double)0.001000,(double)0.666666,20));
    
    // INGRESO DATOS KLLS --------------------------------------------------------------------------------------------------------------------------------------
    //! CAMBIAR "archivoLecturaKLL" dependiendo de si se trabaja con datos reales o generados
    //string archivoLecturaKLL = archivoActual+"Shuffle.txt";
    string archivoLecturaKLL = archivoActual+".txt";

    std::ifstream archivo(archivoLecturaKLL);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            std::pair<int64_t, int64_t> elemento;
            std::istringstream iss(linea);
            if (iss >> elemento.first >> elemento.second) {
                for(int i=0;i<klls.size();i++){
                    klls.at(i).add(elemento); 
                }
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoLecturaKLL << std::endl;
        return 0;
    }

    
    // calcular frecuencia real
    map<int64_t,int64_t> data;
    string archivoHistogramaElementos = "sort"+archivoActual+".txt";
    cerr << archivoHistogramaElementos << endl;
    ifstream file(archivoHistogramaElementos);
    string line;
    while (std::getline(file, line)) {
        std::pair<int64_t, int64_t> elemento;
        std::istringstream iss(line);
        if (iss >> elemento.first >> elemento.second) {
            data[elemento.second] = elemento.first;

        } else {
            std::cerr << "Error al leer la línea: " << line << std::endl;
        }
    }

    // Calcular e impŕimir estimaciones
    for(int i=0;i<klls.size();i++){
        klls.at(i).print();
        vector<pair<int64_t,int64_t>> estimaciones, estimacionHeap, estimacionKll;
        cout << "CalculoEstimaciones:" << endl;
        estimaciones = klls.at(i).getTopFlows(estimacionHeap,estimacionKll);
        //klls.at(i).print();
        cout << "H'': " << klls.at(i).getH_pp() << endl;
        cout << "ESTIMACIONES HEAP" << endl;
        cout << "Flujo\tPayload_Real\tPayload_Estimado\tError_Relativo\n";
        for(int j=0;j<estimacionHeap.size();j++){
            cout << estimacionHeap.at(j).second << "\t" << data[estimacionHeap.at(j).second] << "\t" << estimacionHeap.at(j).first 
                 << "\t" << (double)abs(estimacionHeap.at(j).first-data[estimacionHeap.at(j).second])/(double)data[estimacionHeap.at(j).second] << endl;
        }
        cout << "ESTIMACIONES KLL" << endl;
        cout << "Flujo\tPayload_Real\tPayload_Estimado\tError_Relativo\n";
        for(int j=0;j<estimacionKll.size();j++){
            cout << estimacionKll.at(j).second << "\t" << data[estimacionKll.at(j).second] << "\t" << estimacionKll.at(j).first 
                 << "\t" << (double)abs(estimacionKll.at(j).first - data[estimacionKll.at(j).second])/(double)data[estimacionKll.at(j).second] << endl;
        }
        cout << "ESTIMACIONES GENERALES" << endl;
        cout << "Flujo\tPayload_Real\tPayload_Estimado\tError_Relativo\n";
        cout << estimaciones.size() << endl;
        for(int j=0;j<estimaciones.size();j++){
            cout << estimaciones.at(j).second << "\t" << data[estimaciones.at(j).second] << "\t" << estimaciones.at(j).first 
                 << "\t" << (double)abs(estimaciones.at(j).first-data[estimaciones.at(j).second])/(double)data[estimaciones.at(j).second] << endl;
        }
    }

    // Imprimir los datos almacenados en el mapa
    // pair<uint64_t, uint64_t> par;
    // for (const auto& par : data) {
    //     std::cout << "Payload: " << par.first << ", Flujo: " << par.second << '\n';
    // }

    // OBTENCION TOP-K's y frecuencias
    // for(int i=0;i<klls.size();i++){

    //     vector<pair<int64_t,int64_t>> estimaciones;
    //     estimaciones = klls.at(i).getTopFlows();
    //     klls.at(i).print();
    //     almacenarDatos(klls.at(i).binarySaveName(archivoActual),estimaciones,data,klls.at(i));
    // }


    return 0;
}