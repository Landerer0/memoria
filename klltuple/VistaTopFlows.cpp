// g++ VistaTopFlows.cpp minheap.cpp klltuple.cpp -o VistaTopFlows
// ./VistaTopFlows archivoTraza  esTraza         // GENERAL
// ./VistaTopFlows paresBiasSimpleEscalon.txt 1
// ./VistaTopFlows flujosMendeley.txt 0
// ./VistaTopFlows flujosChicago2015.txt 0

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
#include <cstdlib>

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

void almacenarDatos(string nombreBinario, vector<pair<int64_t,int64_t>> estimacionTopk, KLLTuple kll){

    string nombreArchivo="";

    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "estimacionFrecuencias/";

    string archivoGuardado = nombreCarpeta+"H_pp"+to_string(kll.getH_pp())+"Espacio"+to_string(kll.sizeInBytes())+nombreBinario+".txt";
    string archivoGuardadoParametros = nombreCarpeta+"H_pp"+to_string(kll.getH_pp())+"parametrosEspacio"+to_string(kll.sizeInBytes())+nombreBinario+".txt";
    // Nombre del archivo de salida
    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(archivoGuardado);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        // formato: [Flujo PayloadReal PayloadEstimado] 
        for (const auto& element : estimacionTopk) {
            outfileParametros << element.first << " " << element.second << endl;
        }
        //outfileParametros << kll.sizeInBytes();
        outfileParametros << std::endl;
        // Cerrar el archivo
        outfileParametros.close();
        std::cout << "Las estimaciones de frecuencia se han almacenado correctamente en el archivo " << archivoGuardado << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << archivoGuardado << " para escritura." << std::endl;
    }
    

    cout << "a" << endl;
    std::ofstream outfileParametrosParam(archivoGuardadoParametros);

    if (outfileParametrosParam.is_open()) {
    cout << "b" << endl;
        vector<double> params = kll.getAllParameters();
    cout << params.size() << endl;
        outfileParametrosParam << "H: " << params.at(0) << endl;
        outfileParametrosParam << "H_pp: " << params.at(1) << endl;
        outfileParametrosParam << "epsilon: " << params.at(2) << endl;
        outfileParametrosParam << "delta: " << params.at(3) << endl;
        outfileParametrosParam << "c: " << params.at(4) << endl;
        outfileParametrosParam << "minK: " << params.at(5) << endl;
        outfileParametrosParam << "numArreglos: " << params.at(6) << endl;
        for(int i=7;i<params.size();i++){
            outfileParametrosParam << "k" << to_string(i-7) <<": " << params.at(i) << endl;
        }
            
        outfileParametrosParam.close();
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

    std::srand(42);
    uint64_t n = numLineasArchivo(archivoActualTxt);

    // CREACION KLLS
    // klls.push_back(KLLTuple(100));
    // klls.push_back(KLLTuple(150));
    // klls.push_back(KLLTuple(200));
    // klls.push_back(KLLTuple(250));
    // klls.push_back(KLLTuple(300));
    int numMrl = klls.size();
    
    // klls.push_back(KLLTuple(n/8,(double)0.05,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n/8,(double)0.01,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n/8,(double)0.005,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n/8,(double)0.0075,(double)0.001000,(double)0.5,20));
    // klls.push_back(KLLTuple(n/8,(double)0.001,(double)0.001000,(double)0.5,20));

    // klls.push_back(KLLTuple(n/8,(double)0.05,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n/8,(double)0.01,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n/8,(double)0.005,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n/8,(double)0.0075,(double)0.001000,(double)0.666666,20));
    // klls.push_back(KLLTuple(n/8,(double)0.001,(double)0.001000,(double)0.666666,20));

    int H_pp1 = 2;
    vector<int64_t> sizekll1 = {100, 100, 100};
    vector<int64_t> sizekll2 = {500, 500, 500};
    klls.push_back(KLLTuple(sizekll1,H_pp1));
    klls.push_back(KLLTuple(sizekll2,H_pp1));
    
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

    // Calcular e impŕimir estimaciones
    for(int i=0;i<klls.size();i++){
        
        vector<pair<int64_t,int64_t>> estimaciones, estimacionHeap, estimacionKll;
        cout << "CalculoEstimaciones:" << endl;
        estimaciones = klls.at(i).getTopFlows(estimacionHeap,estimacionKll);
        klls.at(i).print();
        //klls.at(i).print();
        cout << "H'': " << klls.at(i).getH_pp() << endl;
        cout << "ESTIMACIONES HEAP" << endl;
        cout << "Payload_Estimado\tFlujo\n";
        for(int j=0;j<estimacionHeap.size();j++){
            cout  << estimacionHeap.at(j).first<< "\t" << estimacionHeap.at(j).second  << endl;
        }
        cout << "ESTIMACIONES KLL" << endl;
        cout << "Payload_Estimado\tFlujo\n";
        for(int j=0;j<estimacionKll.size();j++){
            cout  << estimacionKll.at(j).first << "\t"<< estimacionKll.at(j).second  << endl;
        }
        cout << "ESTIMACIONES GENERALES" << endl;
        cout << "Payload_Estimado\tFlujo\n";
        cout << estimaciones.size() << endl;
        for(int j=0;j<estimaciones.size();j++){
            cout  << estimaciones.at(j).first<< "\t" << estimaciones.at(j).second<< endl;
        }
        klls.at(i).print();
        almacenarDatos(klls.at(i).binarySaveName(archivoActual),estimaciones,klls.at(i));
    }


    return 0;
}