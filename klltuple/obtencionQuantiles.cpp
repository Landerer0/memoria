// g++ obtencionQuantiles.cpp minheap.cpp klltuple.cpp -o obtencionQuantiles
// ./obtencionQuantiles archivoTraza boolArchivoReal             // GENERAL
// ./obtencionQuantiles paresBiasSimpleEscalon.txt 1
// nohup ./obtencionQuantiles flujosMendeley.txt 0 &
// nohup ./obtencionQuantiles flujosChicago-20160121.txt 0 &
// nohup ./obtencionQuantiles flujosSanjose-20081016.txt 0 &
// nohup ./obtencionQuantiles flujosMawi-20181201.txt 0 &
// nohup ./obtencionQuantiles flujosChicago2015.txt 0 &
// nohup ./obtencionQuantiles flujosChicago2016.txt 0 &
// gdb -ex=r --args obtencionQuantiles flujosChicago2016.txt 0

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

void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, vector<double> &consultaCuantiles,int numRepeticiones
                    , vector<uint64_t> &rank, vector<uint64_t> &quantile, double tiempo, int iteracionActual){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "quantiles/";
    string nombreIteracion = "";
    if(iteracionActual>0) nombreIteracion = to_string(iteracionActual);
    // Nombre del archivo de salida
    std::string filename = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Resultados"+nombreIteracion+".txt";

    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfile(filename);
    if (outfile.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : parametrosKLL) {
            outfile << element << " ";
        }
        outfile << "\n";
        std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << filename << std::endl;
        
        for (const auto& element : consultaCuantiles) {
            outfile << element << " ";
        }
        outfile << "\n";

        std::cout << "Los datos Consulta se han almacenado correctamente en el archivo " << filename << std::endl;
    
        if(rank.size()!=0){
            for (const auto& inner_vector : rank) {
                for (const auto& element : inner_vector) {
                    outfile << element << " ";
                }
                outfile << std::endl; 
            }
            std::cout << "Los datos rank se han almacenado correctamente en el archivo " << filename << std::endl;
        }
        
        if(quantile.size()!=0){
            for (const auto& inner_vector : quantile) {
                for (const auto& element : inner_vector) {
                    outfile << element << " ";
                }
                outfile << std::endl;
            }
            std::cout << "Los datos Quantile se han almacenado correctamente en el archivo " << filename << std::endl;
        }
        
        outfile << tiempo << endl;

        outfile.close();
    }
    // imprimir por pantalla los resultados de esta sesion de experimentos, tanto para rank como quantile
    if(rank.size()!=0){
        for(int i=0;i<consultaCuantiles.size();i++){ // rank
            cout << "resultados de consulta de RANK cuantil: " << consultaCuantiles.at(i) << endl;
            double rankAverage = 0;
            for(int j=0;j<numRepeticiones;j++){
                rankAverage+= rank.at(j).at(i);
                cout << rank.at(j).at(i) << " ";
            }
            if(numRepeticiones>1){
                cout << ". ---- Average: " << rankAverage/numRepeticiones;
            }
            cout << endl;
        }
    }
    
    if(quantile.size()!=0){
        for(int i=0;i<consultaCuantiles.size();i++){ // quantile
            cout << "resultados de consulta de QUANTILE cuantil: " << consultaCuantiles.at(i) << endl;
            double quantileAverage = 0;
            for(int j=0;j<numRepeticiones;j++){
                quantileAverage+= quantile.at(j).at(i);
                cout << quantile.at(j).at(i) << " ";
            }
            if(numRepeticiones>1){
                cout << ". ---- Average: " << quantileAverage/numRepeticiones;
            }
            cout << endl;
        }
    }
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
    klls.push_back(KLLTuple(100));
    klls.push_back(KLLTuple(150));
    klls.push_back(KLLTuple(200));
    klls.push_back(KLLTuple(250));
    klls.push_back(KLLTuple(300));
    int numMrl = klls.size();
    
    klls.push_back(KLLTuple(n/8,(double)0.05,(double)0.001000,(double)0.5,20));
    klls.push_back(KLLTuple(n/8,(double)0.01,(double)0.001000,(double)0.5,20));
    klls.push_back(KLLTuple(n/8,(double)0.005,(double)0.001000,(double)0.5,20));
    klls.push_back(KLLTuple(n/8,(double)0.0075,(double)0.001000,(double)0.5,20));
    klls.push_back(KLLTuple(n/8,(double)0.001,(double)0.001000,(double)0.5,20));

    klls.push_back(KLLTuple(n/8,(double)0.05,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLLTuple(n/8,(double)0.01,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLLTuple(n/8,(double)0.005,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLLTuple(n/8,(double)0.0075,(double)0.001000,(double)0.666666,20));
    klls.push_back(KLLTuple(n/8,(double)0.001,(double)0.001000,(double)0.666666,20));
    
    // INGRESO DATOS KLLS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //! CAMBIAR "archivoLecturaKLL" dependiendo de si se trabaja con datos reales o generados
    string archivoLecturaKLL;
    if(stoi(argv[2])!=0) archivoLecturaKLL = archivoActual+"Shuffle.txt";
    else archivoLecturaKLL = archivoActual+".txt";

    // int setHpp = 0;
    // for(int i=numMrl;i<klls.size();i++){
    //     if(klls.at(i).getH_pp()<setHpp) klls.at(i).setH_pp(setHpp); 
    // }

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

     // Imprimir los datos almacenados en el mapa
    // pair<uint64_t, uint64_t> par;
    // for (const auto& par : data) {
    //     std::cout << "Payload: " << par.first << ", Flujo: " << par.second << '\n';
    // }

    // OBTENCION TOP-K's y frecuencias
    for(int i=0;i<klls.size();i++){

        vector<pair<int64_t,int64_t>> estimaciones;
        estimaciones = klls.at(i).getTopFlows();
        klls.at(i).print();
        almacenarDatos(klls.at(i).binarySaveName(archivoActual),estimaciones,data,klls.at(i));
    }


    return 0;
}