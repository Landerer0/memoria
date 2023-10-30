// g++ main.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o main
// ./main 0 Chicago-20080319.txt Chicago-20080515.txt Chicago-20110608.txt Chicago-20150219.txt Chicago-20160121.txt
// ./main 0 Mawi-20161201.txt Mawi-20171101.txt Mawi-20181201.txt Mawi-20191102.txt Mawi-20200901.txt
// ./main 0 Mendeley.txt Sanjose-20081016.txt
// ./main 0 Chicago-20080319.txt Mendeley.txt
// nohup ./main 0 Chicago-20080319.txt Chicago-20080515.txt Chicago-20110608.txt Chicago-20150219.txt Chicago-20160121.txt Mawi-20161201.txt Mawi-20171101.txt Mawi-20181201.txt Mawi-20191102.txt Mawi-20200901.txt Mendeley.txt Sanjose-20081016.txt &
// nohup ./main &
// nohup ./main 0 Chicago-20080319.txt Mendeley.txt &
// nohup ./main 0 Chicago-20160121.txt Sanjose-20081016.txt Mawi-20181201.txt Mendeley.txt &
// gdb -ex=r --args main 0 Mendeley.txt
// gdb -ex=r --args main 0 Mawi-20200901.txt

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
#include <ctime>

#include <fstream>
#include <sstream>

#include "streamsketch.hpp"

using namespace std;

// Parametros globales
unsigned long numElements = pow(2,22); // numero de elementos totales
unsigned long numElements2 = pow(2,7); // numero de elementos del segundo archivo

unsigned long rankMayorEpsilon = 0;
unsigned long quantileMayorEpsilon = 0;

double epsilon = 0.05; // tamaño del arreglo mas grande
uint64_t minK = 20;
uint64_t mrlKmin = 200;
double numC=(double)2/(double)3; // factor por el que va disminuyendo el tamaño de cada arreglo a medida pasan los niveles.
bool manualLectura = false;

string archivoActual = "";
string archivoActualTxt = "";
string archivoActualMuestra = "";
string archivoActualSort = "";
string archivoActualSortData = "";

string determinedEpsilonPrefix = "";
bool limiteEspacio = false;
vector<uint32_t> valoresEspacioLimite = {5000, 7500, 10000, 15000, 20000};

void quantiles(KLL &kll, int numQuantiles){
    if(numQuantiles <= 0 || numQuantiles>100) return;
    for(int i=0;i<numQuantiles;i++){
        int quantil = 100/numQuantiles*i;
        cout << " quantil " << quantil << " : " << kll.quantile(quantil) << endl;
    }
}

vector<long> crearVectorElements(long numElementosCreate){
    vector<long> elementos;
    for(int i=0;i<numElementosCreate;i++){
        // elementos.push_back(i);
        elementos.push_back(rand()%(10*numElementosCreate));
    }

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(elementos), std::end(elementos), rng);

    return elementos;
}

int countwords(char* filename){
	ifstream fin;
	fin.open(filename);
	char word[30];
	int count=0;
	while(!fin.eof())
	{
		fin>>word;
		count++;
	}
	fin.close();
    return count;
}

void quantileTruthKLL(KLL &kll, vector<double> truth, double cuantil, double n, vector<double> &error){
    long posTruth = n*cuantil/100;
    if(posTruth>0) posTruth-=1;
    //cout << posTruth << " " << cuantil << endl;
    //cout << kll.quantile(cuantil) << endl;
    //cout << truth.at(posTruth) << endl;

    double kllEstimate = kll.quantile(cuantil);
    double truthValue = truth.at(posTruth);
    double errorValue;
    if(truthValue!=0) errorValue = (double)abs(truthValue-kllEstimate)/(double)truthValue;
    else errorValue = (double)abs(truthValue-kllEstimate)/(double)1;

    //cout<<" quantile " << (double)cuantil/(double)100 << ": KLL: " <<kllEstimate << " Truth: " << truthValue 
    //<< " diferencia |q(truth)-q(kll)|: " << abs(kllEstimate-truthValue)<< " ,  error: " << errorValue <<  endl;
    
    if(errorValue>epsilon){
        //cout << "QUANTILE ERROR mayor a epsilon en cuantil " << cuantil << endl; 
        quantileMayorEpsilon++;
    } 
    error.push_back(errorValue);
}

void rankTruthKLL(KLL &kll, vector<double> truth, double cuantil, double n, vector<double> &error){
    long posTruth = n*cuantil/100;
    if(posTruth>0) posTruth-=1;

    double elemento = truth.at(posTruth);

    double kllEstimate = kll.rank(elemento);
    double truthValue = posTruth;
    double errorValue = (double)abs(posTruth-kllEstimate)/n;


    //cout<<"rank de " << elemento << "(cuantil "<< cuantil << ") : KLL: " <<kll.rank(elemento) << " posTruth: "<< posTruth
    //<< " diferencia |r(truth)-r(kll)|: " << abs(truthValue-kllEstimate) << " ,  error: " << errorValue <<  endl;
        
    if(errorValue>epsilon){
        //cout << "RANK ERROR mayor a epsilon en cuantil " << cuantil << endl;  
        rankMayorEpsilon++;
    }   
    error.push_back(errorValue);
}

void pruebaKLL(KLL kll, vector<double> arrTruth, double epsilon, double delta, unsigned long n,vector<double> cuantilesAConsultar,vector<double> &rankError,vector<double> &quantilesError){
    {
        for(int i=0;i<cuantilesAConsultar.size();i++){
            quantileTruthKLL(kll,arrTruth,(long)cuantilesAConsultar.at(i),n,quantilesError);
        }
    }

    {
        for(int i=0;i<cuantilesAConsultar.size();i++){
            rankTruthKLL(kll,arrTruth,(long)cuantilesAConsultar.at(i),n, rankError);
        }
    }

    return;
}

double buscarElementoEnArchivo(uint64_t posicion, string nombreArchivo) {
    std::ifstream inputFile(nombreArchivo);

    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return -1.0;
    }

    std::string line;
    uint64_t lineNumber = 0;
    bool found = false;
    double elemento;

    while (getline(inputFile, line)) {
        //if (lineNumber == posicion) {
            std::istringstream iss(line);
            if (!(iss >> elemento)) {
                std::cerr << "No se pudo convertir el elemento a double." << std::endl;
                return -1.0;
            }
            //std::cout << "El elemento en la posición " << posicion << " es: " << line << std::endl;
            //cout << elemento << endl;
            if(lineNumber == posicion) return elemento;
        //}
        lineNumber++;
    }

    inputFile.close();

    if (!found) {
        std::cout << "No se encontró ningún elemento en la posición " << posicion << "." << std::endl;
    }
    return -1.0;
}

void rankTruthKLLPreprocesado(KLL &kll, double cuantil, double n, vector<double> &error){
    long posTruth = n*cuantil/100;
    if(posTruth>0) posTruth-=1;

    //! Buscar elemento en posicion posTruth en el arreglo
    double elemento = buscarElementoEnArchivo(posTruth,archivoActualSort);

    double kllEstimate = kll.rank(elemento);
    double truthValue = posTruth;
    double errorValue = (double)abs(posTruth-kllEstimate)/n;


    //cout<<"rank de " << elemento << "(cuantil "<< cuantil << ") : KLL: " <<kll.rank(elemento) << " posTruth: "<< posTruth
    //<< " diferencia |r(truth)-r(kll)|: " << abs(truthValue-kllEstimate) << " ,  error: " << errorValue <<  endl;
        
    if(errorValue>epsilon){
        //cout << "RANK ERROR mayor a epsilon en cuantil " << cuantil << endl;  
        rankMayorEpsilon++;
    }   
    error.push_back(errorValue);
}

void quantileTruthKLLPreprocesado(KLL &kll, double cuantil, double n, vector<double> &error){
    long posTruth = n*cuantil/100;
    if(posTruth>0) posTruth-=1;

    double kllEstimate = kll.quantile(cuantil);
    double truthValue = buscarElementoEnArchivo(posTruth,archivoActualSort);
    double errorValue;
    if(truthValue!=0) errorValue = (double)abs(truthValue-kllEstimate)/(double)truthValue;
    else errorValue = (double)abs(truthValue-kllEstimate)/(double)1;

    cerr << "Truth: " << truthValue << ", KLL: " << kllEstimate << endl;

    //cout<<" quantile " << (double)cuantil/(double)100 << ": KLL: " <<kllEstimate << " Truth: " << truthValue 
    //<< " diferencia |q(truth)-q(kll)|: " << abs(kllEstimate-truthValue)<< " ,  error: " << errorValue <<  endl;
    
    if(errorValue>epsilon){
        //cout << "QUANTILE ERROR mayor a epsilon en cuantil " << cuantil << endl; 
        quantileMayorEpsilon++;
    } 
    error.push_back(errorValue);
}

void pruebaKLLPreprocesado(KLL &kll, unsigned long n,vector<double> cuantilesAConsultar,vector<double> &rankError,vector<double> &quantilesError, vector<double> elementosConsulta, vector<uint64_t> truthRank, vector<double> truthQuantiles){
    vector<double> kllquantile = kll.quantile(cuantilesAConsultar);
    vector<uint64_t> kllrank = kll.rank(elementosConsulta);
    /*
    cerr << "consultas kll listos" << endl;

    for(int i=0;i<cuantilesAConsultar.size();i++){
        cerr << elementosConsulta.at(i) << endl;
    }

    for(int i=0;i<cuantilesAConsultar.size();i++){
        cerr << kllrank.at(i) << endl;
        cerr << kllquantile.at(i) << endl;
        cerr << truthRank.at(i) << endl;
        cerr << truthQuantiles.at(i) << endl;
    }
    */

    for(int i=0;i<cuantilesAConsultar.size();i++){
        //cerr << i << endl;
        double kllEstimateRank = kllrank.at(i);
        double kllEstimateQuantile = kllquantile.at(i);
        double truthValueRank = truthRank.at(i);
        double truthValueQuantile = truthQuantiles.at(i);
        double errorValueRank = (double)abs(truthValueRank-kllEstimateRank)/n;
        double errorValueQuantile;
        if(truthValueQuantile!=0) errorValueQuantile = (double)abs(truthValueQuantile-kllEstimateQuantile)/(double)truthValueQuantile;
        else errorValueQuantile = (double)abs(truthValueQuantile-kllEstimateQuantile)/(double)1.0;
        rankError.push_back(errorValueRank);
        quantilesError.push_back(errorValueQuantile);
    }

    return;
}

void pruebaKLLPreprocesado(StreamSketch kll, unsigned long n,vector<double> cuantilesAConsultar,vector<double> &rankError,vector<double> &quantilesError, vector<double> elementosConsulta, vector<uint64_t> truthRank, vector<double> truthQuantiles){
    vector<double> kllquantile = kll.quantile(cuantilesAConsultar);
    vector<uint64_t> kllrank = kll.rank(elementosConsulta);
    /*
    cerr << "consultas kll listos" << endl;

    for(int i=0;i<cuantilesAConsultar.size();i++){
        cerr << elementosConsulta.at(i) << endl;
    }

    for(int i=0;i<cuantilesAConsultar.size();i++){
        cerr << kllrank.at(i) << endl;
        cerr << kllquantile.at(i) << endl;
        cerr << truthRank.at(i) << endl;
        cerr << truthQuantiles.at(i) << endl;
    }
    */

    for(int i=0;i<cuantilesAConsultar.size();i++){
        //cerr << i << endl;
        double kllEstimateRank = kllrank.at(i);
        double kllEstimateQuantile = kllquantile.at(i);
        double truthValueRank = truthRank.at(i);
        double truthValueQuantile = truthQuantiles.at(i);
        double errorValueRank = (double)abs(truthValueRank-kllEstimateRank)/n;
        double errorValueQuantile;
        if(truthValueQuantile!=0) errorValueQuantile = (double)abs(truthValueQuantile-kllEstimateQuantile)/(double)truthValueQuantile;
        else errorValueQuantile = (double)abs(truthValueQuantile-kllEstimateQuantile)/(double)1.0;
        rankError.push_back(errorValueRank);
        quantilesError.push_back(errorValueQuantile);
    }

    return;
}

vector<uint64_t> ranksEnArchivo(vector<double> elementos){
    vector<uint64_t> ranks;
    std::ifstream archivo(archivoActualSort);

    cerr << "Archivo Actual Sort: " <<  archivoActualSort << endl;

    if (!archivo){
        std::cerr << "No se pudo abrir el archivo " << archivoActualSort << std::endl;
        return vector<uint64_t>(); // Valor de retorno indicando error
    }

    double elemento;
    double ultimoElemento = -1.0;
    string lineaActual;
    uint64_t linea = 0;
    uint64_t i = 0;

    if(archivo.is_open()) {
        while (std::getline(archivo, lineaActual)) {
            linea++;
            std::istringstream iss(lineaActual);
            if (iss >> elemento) {
                if (elementos[i] < elemento){
                    //cerr << "Elemento " << elemento << "Linea " << linea << endl;
                    ranks.push_back(linea);
                    i++;
                }
            }
        }
        archivo.close();
    }

    while(ranks.size()<elementos.size()){
        ranks.push_back(linea);
    }

    return ranks;
}

vector<double> pruebaKLLMuestraPreprocesado(KLL kll, unsigned long n,vector<double> &rankError){
    std::string rutaArchivo = archivoActualMuestra;

    vector<double> elementosMuestra;
    vector<double> rankAConsultar;

    std::ifstream archivo(rutaArchivo);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                elementosMuestra.push_back(dato);
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << rutaArchivo << std::endl;
        return vector<double> ();
    }

    rankAConsultar.push_back(elementosMuestra.at(0));
    for(int i=0;i<elementosMuestra.size()-1;i++){
        if(elementosMuestra[i]!=elementosMuestra[i+1]) rankAConsultar.push_back(elementosMuestra[i+1]);
    }

    vector<uint64_t> rankObtenidos;
    rankObtenidos = kll.rank(rankAConsultar);

    cerr << "Num de elementos Consultados: " << rankAConsultar.size() << " de " << elementosMuestra.size() << " elementos." << endl;
    //for(int i =0; i<rankAConsultar.size();i++){
    //    cerr << "Rank de " << rankAConsultar[i] << ": " << rankObtenidos[i] << endl;
    //}
    //cerr << "Num de elementos Consultados: " << rankAConsultar.size() << " de " << elementosMuestra.size() << " elementos." << endl;

    // Consultar por los ranks de los elementos en el archivo
    vector<uint64_t> realRank = ranksEnArchivo(rankAConsultar);

    for(int i=0;i<rankAConsultar.size();i++){
        rankError.push_back( abs((double)(realRank[i]-rankObtenidos[i])/(double)n) );
        cerr << "Rank de " << (uint64_t)rankAConsultar[i] << ": " << realRank[i] << ", estimado: " << rankObtenidos[i] << endl;
    }

    return rankAConsultar;
}

void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, vector<double> &consultaCuantiles,int numRepeticiones
                    , vector<vector<double>> &rank, vector<vector<double>> &quantile, double tiempo){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultados/";
    // Nombre del archivo de salida
    std::string filename = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Resultados.txt";

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

// void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, vector<double> &consultaCuantiles,int numRepeticiones
//                     , vector<vector<double>> &rank, vector<vector<double>> &quantile){
//     // se define ubicacion resultante de los archivos generados por las pruebas
//     std::string nombreCarpeta = "resultados/";
//     // Nombre del archivo de salida
//     std::string filenameParametros = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Parametros.txt";
//     std::string filenameConsulta = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Consulta.txt";
//     std::string filenameRank = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Rank.txt";
//     std::string filenameQuantile = nombreCarpeta+determinedEpsilonPrefix+"datos"+filenameEspecifications+"Quantile.txt";

//     // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
//     std::ofstream outfileParametros(filenameParametros);
//     if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
//         for (const auto& element : parametrosKLL) {
//             outfileParametros << element << " ";
//         }
//         outfileParametros << std::endl;
//         // Cerrar el archivo
//         outfileParametros.close();
//         std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << filenameParametros << std::endl;
//     } else {
//         std::cerr << "No se pudo abrir el archivo Parametros " << filenameParametros << " para escritura." << std::endl;
//     }
//     std::ofstream outfileConsulta(filenameConsulta); // consulta (cuantiles consultados)
//     if (outfileConsulta.is_open()) {
//         for (const auto& element : consultaCuantiles) {
//             outfileConsulta << element << " ";
//         }
//         outfileConsulta << std::endl;
//         outfileConsulta.close();
//         std::cout << "Los datos Consulta se han almacenado correctamente en el archivo " << filenameConsulta << std::endl;
//     } else {
//         std::cerr << "No se pudo abrir el archivo Consulta " << filenameConsulta << " para escritura." << std::endl;
//     }
//     if(rank.size()!=0){
//         std::ofstream outfileRank(filenameRank); // rankError obtenido de los experimentos
//         if (outfileRank.is_open()) {
//             for (const auto& inner_vector : rank) {
//                 for (const auto& element : inner_vector) {
//                     outfileRank << element << " ";
//                 }
//                 outfileRank << std::endl; 
//             }
//             outfileRank.close();
//             std::cout << "Los datos rank se han almacenado correctamente en el archivo " << filenameRank << std::endl;
//         } else {
//             std::cerr << "No se pudo abrir el archivo rank " << filenameRank << " para escritura." << std::endl;
//         }
//     }
    
//     if(quantile.size()!=0){
//         std::ofstream outfileQuantile(filenameQuantile); // errorRelativo obtenido de los experimentos
//         if (outfileQuantile.is_open()) {
//             for (const auto& inner_vector : quantile) {
//                 for (const auto& element : inner_vector) {
//                     outfileQuantile << element << " ";
//                 }
//                 outfileQuantile << std::endl;
//             }
//             outfileQuantile.close();
//             std::cout << "Los datos Quantile se han almacenado correctamente en el archivo " << filenameQuantile << std::endl;
//         } else {
//             std::cerr << "No se pudo abrir el archivo Quantile " << filenameQuantile << " para escritura." << std::endl;
//         }    
//     }
    

//     // imprimir por pantalla los resultados de esta sesion de experimentos, tanto para rank como quantile
//     if(rank.size()!=0){
//         for(int i=0;i<consultaCuantiles.size();i++){ // rank
//             cout << "resultados de consulta de RANK cuantil: " << consultaCuantiles.at(i) << endl;
//             double rankAverage = 0;
//             for(int j=0;j<numRepeticiones;j++){
//                 rankAverage+= rank.at(j).at(i);
//                 cout << rank.at(j).at(i) << " ";
//             }
//             if(numRepeticiones>1){
//                 cout << ". ---- Average: " << rankAverage/numRepeticiones;
//             }
//             cout << endl;
//         }
//     }
    
//     if(quantile.size()!=0){
//         for(int i=0;i<consultaCuantiles.size();i++){ // quantile
//             cout << "resultados de consulta de QUANTILE cuantil: " << consultaCuantiles.at(i) << endl;
//             double quantileAverage = 0;
//             for(int j=0;j<numRepeticiones;j++){
//                 quantileAverage+= quantile.at(j).at(i);
//                 cout << quantile.at(j).at(i) << " ";
//             }
//             if(numRepeticiones>1){
//                 cout << ". ---- Average: " << quantileAverage/numRepeticiones;
//             }
//             cout << endl;
//         }
//     }
// }

void almacenarDatosMuestreados(string filenameEspecifications,vector<double> ranksConsultados , vector<double> &parametrosKLL, vector<vector<double>> &rank){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultados/";
    // Nombre del archivo de salida
    std::string filenameParametros = nombreCarpeta+"datos"+filenameEspecifications+"Parametros.txt";
    std::string filenameConsulta = nombreCarpeta+"datos"+filenameEspecifications+"Consulta.txt";
    std::string filenameRank = nombreCarpeta+"datos"+filenameEspecifications+"Rank.txt";

    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfileParametros(filenameParametros);
    if (outfileParametros.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : parametrosKLL) {
            outfileParametros << element << " ";
        }
        outfileParametros << std::endl;
        // Cerrar el archivo
        outfileParametros.close();
        std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << filenameParametros << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << filenameParametros << " para escritura." << std::endl;
    }
    std::ofstream outfileConsulta(filenameConsulta); // consulta (cuantiles consultados)
    if (outfileConsulta.is_open()) {
        for (const auto& element : ranksConsultados) {
            outfileConsulta << element << " ";
        }
        outfileConsulta << std::endl;
        outfileConsulta.close();
        std::cout << "Los datos Consulta se han almacenado correctamente en el archivo " << filenameConsulta << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Consulta " << filenameConsulta << " para escritura." << std::endl;
    }
    if(rank.size()!=0){
        std::ofstream outfileRank(filenameRank); // rankError obtenido de los experimentos
        if (outfileRank.is_open()) {
            for (const auto& inner_vector : rank) {
                for (const auto& element : inner_vector) {
                    outfileRank << element << " ";
                }
                outfileRank << std::endl; 
            }
            outfileRank.close();
            std::cout << "Los datos rank se han almacenado correctamente en el archivo " << filenameRank << std::endl;
        } else {
            std::cerr << "No se pudo abrir el archivo rank " << filenameRank << " para escritura." << std::endl;
        }
    }  

    /*
    for(int i=0;i<ranksConsultados.size();i++){ // rank
        cout << "resultados de consulta de RANK cuantil: " << ranksConsultados.at(i) << endl;
        double rankAverage = 0;
        for(int j=0;j<1;j++){
            rankAverage+= rank.at(j).at(i);
            cout << rank.at(j).at(i) << " ";
        }
        cout << endl;
    }
    */
    
}

void prueba(unsigned long n, double epsilon, double delta, double c, vector<double> distribucion, vector<double> consultaCuantiles,int numRepeticiones){    
    if(distribucion.size()==0||consultaCuantiles.size()==0){
        cout << "no indica distribución o cuantiles a consultar." << endl;
        return;
    }

    std::default_random_engine generator;
    generator.seed(0);

    // 0 es distribución normal, 1 es gamma, 2 es exponencial
    double tipoDistribucion = distribucion.at(0);

    rankMayorEpsilon=0;
    quantileMayorEpsilon=0;

    // vector que almacena los resultados por repeticion
    //      cada repeticion corresponde a un vector que almacenara los resultados de los cuantiles proporcionados
    //              cada cuantil almacenara el resultado de los 
    vector<double> parametrosKLL;
    vector<vector<double>> rank; 
    vector<vector<double>> quantile;
    vector<vector<double>> rankkmin; 
    vector<vector<double>> quantilekmin;

    if(distribucion.at(0)==3){
        if(distribucion.at(1)==1) n = 1000000;
        else if(distribucion.at(1)==2) n = 2000000;
        numRepeticiones = 1;
    } else if(distribucion.at(0)==-1){
        fstream in(archivoActualTxt);
        int lines = 0;
        char endline_char = '\n';
        while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
        {
            ++lines;
        }
        n = lines-1;
        cout << "En en el archivo " << archivoActual << " hay " << n << " lineas" << endl;
    }

    // Se define nombre del archivo resultante a partir del archivo o distribución indicada
    string filenameEspecifications;
    string filenameEspecificationskmin;
    if(distribucion.at(0)==0){
        filenameEspecifications = filenameEspecifications + "Normal";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(1));
        filenameEspecifications = filenameEspecifications + "y";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(2));
    } else if(distribucion.at(0)==1) {
        filenameEspecifications = filenameEspecifications + "Gamma";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(1));
        filenameEspecifications = filenameEspecifications + "y";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(2));
    } else if (distribucion.at(0)==2) {
        filenameEspecifications = filenameEspecifications + "Exponencial";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(1));
    } else if (distribucion.at(0)==3) {
        filenameEspecifications = filenameEspecifications + "Power_law";
        filenameEspecifications = filenameEspecifications + "Archivo" + to_string(distribucion.at(1));
    } else if (distribucion.at(0)==4) {
        filenameEspecifications = filenameEspecifications + "Uniforme";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(1));
        filenameEspecifications = filenameEspecifications + "y";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(2));
    } else if (distribucion.at(0)==5) {
        filenameEspecifications = filenameEspecifications + "Geometrica";
        filenameEspecifications = filenameEspecifications + to_string(distribucion.at(1));
    } else if (distribucion.at(0)==-1) {
        if(distribucion.size()==2) filenameEspecifications = filenameEspecifications + "preprocesado"; 
        filenameEspecifications = filenameEspecifications + archivoActual;
    }
    
    filenameEspecificationskmin = filenameEspecifications + "kmin";
    // se agrega identificadores tanto de epsilon como de delta al nombre del archivo para poder diferenciar
    // las pruebas con variaciones de estos parametros pero con la misma distribucion (y parametros)
    filenameEspecifications = filenameEspecifications + "e" + to_string(epsilon) + "d" + to_string(delta);


    // ahora se procede de manera distinta dependiendo de si se trata de un fichero con trazas o no
    // esto con el fin de ahorrar tiempo de ejecución

    if(distribucion.at(0)==-1 && distribucion.size()>1){
        KLL kll1(n,epsilon,delta,c,minK);

        std::ifstream archivo(archivoActualTxt);
        std::string linea;
        if(archivo.is_open()) {
            while (std::getline(archivo, linea)) {
                double dato;
                std::istringstream iss(linea);
                if (iss >> dato) 
                    kll1.add(dato);
            }
            archivo.close();
        } else {
            std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
            return;
        }

        vector<double> quantilesError1;
        vector<double> rankError1;
        vector<double> ranksConsultados;

        
        //if(distribucion.size()== 2) pruebaKLLPreprocesado(kll1,n,consultaCuantiles,rankError1,quantilesError1);
        //else if(distribucion.size()==3) ranksConsultados = pruebaKLLMuestraPreprocesado(kll1,n,rankError1);
        if(distribucion.size()==3) ranksConsultados = pruebaKLLMuestraPreprocesado(kll1,n,rankError1);

        parametrosKLL = kll1.parametros();
        // INCORPORAR UNA MANERA DE DETERMINAR EL RANGO (RESTAR LA ULTIMA LINEA Y EL PRIMER ELEMENTO)
        parametrosKLL.push_back(buscarElementoEnArchivo(0,archivoActualSortData));
        // INCORPORAR UNA MANERA DE DETERMINAR EL NUMERO DE ELEMENTOS REPETIDOS/DISTINTOS
        parametrosKLL.push_back(buscarElementoEnArchivo(1,archivoActualSortData));

        rank.push_back(rankError1);
        quantile.push_back(quantilesError1);

        if(distribucion.size()==3){
            string filenameEspecificationsMuestra;
            filenameEspecificationsMuestra = filenameEspecificationsMuestra + "muestra";
            filenameEspecificationsMuestra = filenameEspecificationsMuestra + "preprocesado";
            filenameEspecificationsMuestra = filenameEspecificationsMuestra + archivoActual;
        

            // se agrega identificadores tanto de epsilon como de delta al nombre del archivo para poder diferenciar
            // las pruebas con variaciones de estos parametros pero con la misma distribucion (y parametros)
            filenameEspecificationsMuestra = filenameEspecificationsMuestra + "e" + to_string(epsilon) + "d" + to_string(delta);

            almacenarDatosMuestreados(filenameEspecificationsMuestra, ranksConsultados, parametrosKLL, rank);

            return;
        }
        
    } else {
        for(int i=0;i<numRepeticiones;i++){
            KLL kll1(n,epsilon,delta,c,minK);
            KLL kllkmin(200);
            vector<double> arrTruth1;
            cout << "Repeticion: " << i+1 << endl;
            //      cout << "SizeinBytes: " << kll1.sizeInBytes() << endl;
            //      return; //! *******************************************************************
            switch((int)tipoDistribucion){
                case -1:
                {
                    if(distribucion.size()!=1) return;
                    std::ifstream archivo(archivoActualTxt);
                    std::string linea;
                    if(archivo.is_open()) {
                        while (std::getline(archivo, linea)) {
                            double dato;
                            std::istringstream iss(linea);
                            if (iss >> dato) {
                                kll1.add(dato);
                                if(i+1==numRepeticiones) kllkmin.add(dato);
                                arrTruth1.push_back(dato);
                            }
                        }
                        archivo.close();
                    } else {
                        std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
                        return;
                    }
                    break;
                }
                case 0: //normal
                {
                    cerr << "distribucion normal" << endl;
                    if(distribucion.size()!=3) return;
                    std::normal_distribution<> d0{distribucion.at(1), distribucion.at(2)};
                    for(int i=0;i<n;i++){
                        double toAdd = d0(generator);
                        kll1.add(toAdd);
                        arrTruth1.push_back(toAdd);
                    }
                    break;
                }
                case 1: //gamma
                {
                    if(distribucion.size()!=3) return;
                    std::gamma_distribution<> d1(distribucion.at(1), distribucion.at(2));
                    for(int i=0;i<n;i++){
                        double toAdd = round(d1(generator));
                        kll1.add(toAdd);
                        arrTruth1.push_back(toAdd);
                    }
                    break;
                }
                case 2: //exponencial
                {
                    //cout << "Exponencial" << endl;
                    if(distribucion.size()!=2) return;
                    std::exponential_distribution<> d2(distribucion.at(1)); 
                    for(int i=0;i<n;i++){
                        double toAdd = round(1.0+d2(generator));
                        kll1.add(toAdd);
                        arrTruth1.push_back(toAdd);
                    }
                    break;
                }
                case 3: //power law
                {
                    if(distribucion.size()!=2) return;
                    std::string rutaArchivo = "muestra-plaw-" + to_string((int)distribucion.at(1)) + "m-1.86-xmin1000.txt";
                    std::ifstream archivo(rutaArchivo);
                    std::string linea;
                    if(archivo.is_open()) {
                        while (std::getline(archivo, linea)) {
                            double dato;
                            std::istringstream iss(linea);
                            if (iss >> dato) {
                                kll1.add(dato);
                                arrTruth1.push_back(dato);
                            }
                        }
                        archivo.close();
                    } else {
                        std::cout << "No se pudo abrir el archivo: " << rutaArchivo << std::endl;
                        return;
                    }
                    break;
                }
                case 4: // uniforme
                {
                    if(distribucion.size()!=3) return;
                    std::uniform_real_distribution<> d4(distribucion.at(1), distribucion.at(2));
                    for(int i=0;i<n;i++){
                        double toAdd = round(d4(generator));
                        kll1.add(toAdd);
                        arrTruth1.push_back(toAdd);
                    }
                    break;
                }
                case 5: //geometrica
                {
                    //cout << "Geometrica" << endl;
                    if(distribucion.size()!=2) return;
                    std::exponential_distribution<> d5(distribucion.at(1)); 
                    for(int i=0;i<n;i++){
                        double toAdd = d5(generator);
                        kll1.add(toAdd);
                        arrTruth1.push_back(toAdd);
                    }
                    break;
                }
                default:
                    cout << "error." << endl;
                    return;
            }

            if(i+1==numRepeticiones){
                for(int j=0;j<arrTruth1.size();j++){
                    kllkmin.add(arrTruth1[j]);
                }
            }

            sort(arrTruth1.begin(), arrTruth1.end());

            vector<double> quantilesError1;
            vector<double> rankError1;
            vector<double> quantilesErrorkmin;
            vector<double> rankErrorkmin;

            pruebaKLL(kll1,arrTruth1,epsilon,delta,n,consultaCuantiles,rankError1,quantilesError1);
            rank.push_back(rankError1);
            quantile.push_back(quantilesError1);

            if(i+1==numRepeticiones){
                // agregaremos un par de datos más, rango del ultimo experimento y cantidad de elementos distintos
                parametrosKLL = kll1.parametros();
                parametrosKLL.push_back(abs(arrTruth1[n-1]-arrTruth1[0])); // rango de valores
                double numElementosRepetidos=0;
                for(int j=0; j<(n-1);j++){
                    if(arrTruth1[j]==arrTruth1[j+1]) numElementosRepetidos++;
                }
                parametrosKLL.push_back(numElementosRepetidos); // num de valores repetidos
                
                pruebaKLL(kllkmin,arrTruth1,epsilon,delta,n,consultaCuantiles,rankErrorkmin, quantilesErrorkmin);

                string puntoBin = ".bin";
                string carpetaBin = "kllbin/";
                parametrosKLL.push_back(kllkmin.saveData(carpetaBin+filenameEspecificationskmin+"kmin"));

                rankkmin.push_back(rankErrorkmin);
                quantilekmin.push_back(quantilesErrorkmin);

                almacenarDatos(filenameEspecificationskmin, parametrosKLL, consultaCuantiles, 1, rankkmin, quantilekmin, 0);

                parametrosKLL.at(parametrosKLL.size()-1) = kll1.saveData(carpetaBin+filenameEspecifications);
            } 
            
            
        }
    }
    
    cout << "Quantile: " << endl;
    for(int i=0;i<consultaCuantiles.size();i++){
        cout << "Quantile de cuantil " << consultaCuantiles[i]
             << ": " << quantile.at(0).at(i)
             << endl;
    }

    cerr << "RANK!!!!:" << endl;
    cerr << "QUANTILE!!!!:" << endl;

    almacenarDatos(filenameEspecifications, parametrosKLL, consultaCuantiles, numRepeticiones, rank, quantile, 0);

    //cout << "Cantidad de veces que el error supero la proporcion entregada por epsilon." << endl
    //     << "Rank: " << rankMayorEpsilon << endl
    //     << "Quantile: " << quantileMayorEpsilon << endl << endl;
    return;
}

// aqui se definen funciones para generar las distintas consultas
// se utiliza principalmente para la creacion de distribuciones
vector<double> generadorConsultaArchivo(double contieneTrazas){
    vector<double> resultado;
    resultado.push_back(-1);   
    if(contieneTrazas==0) resultado.push_back(0);
    else if (contieneTrazas == 1){
        resultado.push_back(0);
        resultado.push_back(0);
    }
    return resultado;
}
vector<double> generadorConsultaVectorNormal(double media, double desviacionEstandar){
    vector<double> resultado;
    resultado.push_back(0);
    resultado.push_back(media);
    resultado.push_back(desviacionEstandar);
    return resultado;
}
vector<double> generadorConsultaVectorGamma(double alfa, double beta){
    vector<double> resultado;
    resultado.push_back(1);
    resultado.push_back(alfa);
    resultado.push_back(beta);
    return resultado;
}
vector<double> generadorConsultaVectorExponencial(double lamda){
    vector<double> resultado;
    resultado.push_back(2);
    resultado.push_back(lamda);
    return resultado;
}
vector<double> generadorConsultaPowerLaw(double numArchivo){
    vector<double> resultado;
    resultado.push_back(3);
    resultado.push_back(numArchivo);
    return resultado;
}
vector<double> generadorConsultaUniforme(double limiteInferior, double limiteSuperior){
    vector<double> resultado;
    resultado.push_back(4);
    resultado.push_back(limiteInferior);
    resultado.push_back(limiteSuperior);
    return resultado;
}

vector<double> generadorConsultaVectorGeometrica(double lamda){
    vector<double> resultado;
    resultado.push_back(5);
    resultado.push_back(lamda);
    return resultado;
}

// metodo para realizar pruebas con distintos epsilon y deltas.
void probarVariacionDistribucion(unsigned long n, double epsilon, double delta, double c, vector<double> distribucion, vector<double> consultaCuantiles, long numRepeticiones){
    
   
    cout << "VARIACIONES DELTA:" << endl; // epsilon constante = 0.05
    prueba(n,0.05,0.010000,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.001000,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000100,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000010,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000001,c,distribucion,consultaCuantiles,numRepeticiones);
    cout << "VARIACIONES EPSILON:" << endl << endl; // delta constante = 0.0001
    prueba(n,0.100,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.050,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.025,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.010,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.005,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    
    return;
}

void pruebaPreprocesado(uint64_t n, double epsilon, double delta, double c, vector<double> distribucion, vector<double> consultaCuantiles, vector<double> elementosConsulta, vector<uint64_t> truthRank, vector<double> truthQuantile, bool isMrl, bool epsilonDetermined){
    if(distribucion.size()==0||consultaCuantiles.size()==0){
        cout << "no indica distribución o cuantiles a consultar." << endl;
        return;
    }

    clock_t inicio, fin;
    double tiempo;
    inicio = clock();

    std::default_random_engine generator;
    generator.seed(0);

    rankMayorEpsilon=0;
    quantileMayorEpsilon=0;

    // vector que almacena los resultados por repeticion
    //      cada repeticion corresponde a un vector que almacenara los resultados de los cuantiles proporcionados
    //              cada cuantil almacenara el resultado de los 
    vector<double> parametrosKLL;
    vector<vector<double>> rank; 
    vector<vector<double>> quantile;

    
    KLL kll1 = epsilonDetermined ? isMrl ? KLL(epsilon, n) : KLL(n, epsilon, c, minK) : isMrl ? KLL(mrlKmin) : KLL(n,epsilon,delta,c,minK);

    std::ifstream archivo(archivoActualTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) {
                kll1.add(dato); 
            }
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << archivoActualTxt << std::endl;
        return;
    }

    vector<double> quantilesError1;
    vector<double> rankError1;

    cerr << "Prueba KLL Preprocesado INICIO" << endl;
 
    pruebaKLLPreprocesado(kll1,n,consultaCuantiles,rankError1,quantilesError1,elementosConsulta, truthRank, truthQuantile);    
    
    cerr << "Prueba KLL Preprocesado FIN" << endl;
    
    parametrosKLL = kll1.parametros();
    parametrosKLL.push_back(buscarElementoEnArchivo(0,archivoActualSortData)); // rango
    parametrosKLL.push_back(buscarElementoEnArchivo(1,archivoActualSortData)); // numElementosRepetidos
    parametrosKLL.push_back(n-parametrosKLL.at(parametrosKLL.size()-1)); // numElementosDistintos
    // posteriormente se incorpora el espacio ocupado de los datos en binario

    rank.push_back(rankError1);
    quantile.push_back(quantilesError1);


    // Se define nombre del archivo resultante a partir del archivo
    string filenameEspecifications;
    filenameEspecifications = filenameEspecifications + "preprocesado"; 
    filenameEspecifications = filenameEspecifications + archivoActual;
    if(isMrl) filenameEspecifications = filenameEspecifications + "kmin";
    else filenameEspecifications = filenameEspecifications + "e" + to_string(epsilon) + "d" + to_string(delta);

    cerr << "RANK!!!!:" << endl;
    cerr << "QUANTILE!!!!:" << endl;

    string puntoBin = ".bin";
    string carpetaBin = "kllbin/";
    parametrosKLL.push_back(kll1.saveData(carpetaBin+filenameEspecifications)); // espacioBinario
    vector<uint64_t> tiempos = kll1.getTimes();
    for(int i=0;i<tiempos.size();i++){
        parametrosKLL.push_back(tiempos.at(i));
    }

    fin = clock();
    tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;
    almacenarDatos(filenameEspecifications, parametrosKLL, consultaCuantiles, 1, rank, quantile, tiempo);
    
    //kll1.print();

    return;
}

void obtenerTruthRankQuantileArchivoPreprocesado(uint64_t n, vector<double> consultaCuantiles, vector<double> &elementosConsulta, vector<uint64_t> &rank, vector<double> &quantile){
    // obtener las posiciones a buscar a partir de n y los valores en consultaCuantiles
    vector<uint64_t> posicionesArchivo;
    for(int i=0;i<consultaCuantiles.size();i++){
        uint64_t pos;
        pos = n*(consultaCuantiles.at(i)/100.0);
        if(pos>0) pos-=1;
        posicionesArchivo.push_back(pos);
    }

    // se abre el archivo ordenado (actual+Sort)
    std::ifstream inputFile(archivoActualSort);
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return;
    }

    std::string line;
    uint64_t lineNumber = 0;
    double elemento;
    double elementoConsultaAnterior;
    uint64_t posActual = 0;
    uint64_t posMaxima = posicionesArchivo.at(posicionesArchivo.size()-1);
    vector<double> elementoAux;
    uint32_t numElementosIguales = 0;

    while (getline(inputFile, line) && lineNumber<=posMaxima) {
        //if (lineNumber == posicion) {
            std::istringstream iss(line);
            if (!(iss >> elemento)) {
                std::cerr << "No se pudo convertir el elemento a double." << std::endl;
                return;
            }
            //std::cout << "El elemento en la posición " << posicion << " es: " << line << std::endl;
            //cout << elemento << endl;
            if(numElementosIguales>0&&elementoConsultaAnterior<elemento){
                cout << elementoConsultaAnterior << " " << elemento << " " << lineNumber << endl;
                for(int j=0; j<numElementosIguales;j++) rank.push_back(lineNumber);
                numElementosIguales=0;
            }
            if(lineNumber == posicionesArchivo.at(posActual)){
                elementoConsultaAnterior = elemento;
                numElementosIguales++;
                quantile.push_back(elemento);
                elementosConsulta.push_back(elemento);
                posActual++;
            } 
        //}
        lineNumber++;
    }

    inputFile.close();

    if(rank.size()==consultaCuantiles.size()-1) rank.push_back(lineNumber-1);

    /*
    for(int i=0;i<consultaCuantiles.size();i++){
        cout << "cuantil " << consultaCuantiles.at(i) << ": rank " << rank.at(i) << " quantile " << quantile.at(i) << endl;
    }
    */

    return;
}

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

void pruebaVariacionArchivoPreprocesado(vector<double> distribucionArchivoTraza, vector<double> consultaCuantiles){
    uint32_t n = numLineasArchivo(archivoActualTxt);
    
    cout << "N: " << n << endl;
    
    double c = 2.0/3.0;

    vector<uint64_t> truthRank;
    vector<double> truthQuantile;
    vector<double> elementosConsulta;
    bool isNotMrl = false;
    bool isMrl = true;

    obtenerTruthRankQuantileArchivoPreprocesado(n, consultaCuantiles, elementosConsulta, truthRank, truthQuantile);

    if(limiteEspacio){
        // // Pruebas KLL con Espacio Limitado
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,valoresEspacioLimite.at(0));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,valoresEspacioLimite.at(1));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,valoresEspacioLimite.at(2));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,valoresEspacioLimite.at(3));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,valoresEspacioLimite.at(4));
        // // pruebas MRL con Espacio Limitado
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,valoresEspacioLimite.at(0));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,valoresEspacioLimite.at(1));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,valoresEspacioLimite.at(2));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,valoresEspacioLimite.at(3));
        // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,valoresEspacioLimite.at(4));

        return;
    }
    
    bool notEpsilonDetermined = false;
    // cout << "prueba preprocesado" << endl;
    // cout << "VARIACIONES DELTA:" << endl; // epsilon constante = 0.05
    // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.05,0.001000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.05,0.000100,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.05,0.000010,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.05,0.000001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // cout << "VARIACIONES EPSILON:" << endl << endl; // delta constante = 0.0001
    // pruebaPreprocesado(n,0.100,0.0001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.050,0.0001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.025,0.0001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.010,0.0001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // pruebaPreprocesado(n,0.005,0.0001,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl,notEpsilonDetermined);
    // cout << "VARIACIONES MRL:" << endl << endl; // MRL con k=mrlMink especificado en variable global
    // pruebaPreprocesado(n,0.05,0.010000,c,distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl,0);

    //! prueba con epsilon proporcionado
    double epsilon;
    bool epsilonDetermined = true;
    c=2.0/3.0;
    determinedEpsilonPrefix = "epsilonDetermined";

    c=1.0/2.0;
    
    // determinedEpsilonPrefix = "epsilonDetermined0.05";
    // epsilon = 0.05;
    // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    // // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    // determinedEpsilonPrefix = "epsilonDetermined0.025";
    // epsilon = 0.025;
    // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.01";
    epsilon = 0.01;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.0075";
    epsilon = 0.0075;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.006";
    epsilon = 0.006;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.005";
    epsilon = 0.005;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.004";
    epsilon = 0.004;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.003";
    epsilon = 0.003;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.0025";
    epsilon = 0.0025;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    determinedEpsilonPrefix = "epsilonDetermined0.002";
    epsilon = 0.002;
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    // determinedEpsilonPrefix = "epsilonDetermined0.001";
    // epsilon = 0.001;
    // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isMrl, epsilonDetermined);
    // pruebaPreprocesado(n, epsilon, 0.0, c, distribucionArchivoTraza, consultaCuantiles, elementosConsulta, truthRank, truthQuantile, isNotMrl, epsilonDetermined);
    
}

int main(int argc, char*argv[]){
    // DEFINICION DE PARAMETROS
    unsigned long n= pow(2,20);
    double epsilon = 0.05;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    // Generación de vectores para consultas de distintas distribuciones
    vector<double> distribucionNormal1000y100 = generadorConsultaVectorNormal(1000,100);
    vector<double> distribucionNormal50y10 = generadorConsultaVectorNormal(50,10);
    vector<double> distribucionNormal500000y100000 = generadorConsultaVectorNormal(500000,100000);
    vector<double> distribucionNormal500000y1000000 = generadorConsultaVectorNormal(500000,1000000);
    vector<double> distribucionGamma5y1 = generadorConsultaVectorGamma(5,1);
    vector<double> distribucionGamma3y2 = generadorConsultaVectorGamma(3,2);
    vector<double> distribucionGamma4y20 = generadorConsultaVectorGamma(4,20);
    vector<double> distribucionExponencial0p5 = generadorConsultaVectorExponencial(0.5);
    vector<double> distribucionExponencial1p0 = generadorConsultaVectorExponencial(1.0);
    vector<double> distribucionExponencial1p5 = generadorConsultaVectorExponencial(1.5);
    vector<double> distribucionPowerLaw1 = generadorConsultaPowerLaw(1);
    vector<double> distribucionPowerLaw2 = generadorConsultaPowerLaw(2);
    vector<double> distribucionUniforme1y100 = generadorConsultaUniforme(1,100);
    vector<double> distribucionUniforme50000y10000000 = generadorConsultaUniforme(50000,10000000);
    vector<double> distribucionGeometrica0p5 = generadorConsultaVectorGeometrica(0.5);
    vector<double> distribucionGeometrica1p0 = generadorConsultaVectorGeometrica(1.0);
    vector<double> distribucionGeometrica1p5 = generadorConsultaVectorGeometrica(1.5);

    // creación de vectores para consulta de cuantiles
    vector<double> consultaCuantiles;
    for(int i=0;i<=100;i++){
        consultaCuantiles.push_back(i);
    }
    //vector<double> consultaCuantiles = {2,5,10,20,30,40,50,60,70,80,90,95,98};
    //vector<double> consultaCuantiles = {0,10,20,30,40,50,60,70,80,90,100};
    {
    vector<double> consultaCuantil02 = {2};
    vector<double> consultaCuantil05 = {5};
    vector<double> consultaCuantil10 = {10};
    vector<double> consultaCuantil15 = {15};
    vector<double> consultaCuantil20 = {20};
    vector<double> consultaCuantil25 = {25};
    vector<double> consultaCuantil30 = {30};
    vector<double> consultaCuantil35 = {35};
    vector<double> consultaCuantil40 = {40};
    vector<double> consultaCuantil45 = {45};
    vector<double> consultaCuantil50 = {50};
    vector<double> consultaCuantil55 = {55};
    vector<double> consultaCuantil60 = {60};
    vector<double> consultaCuantil65 = {65};
    vector<double> consultaCuantil70 = {70};
    vector<double> consultaCuantil75 = {75};
    vector<double> consultaCuantil80 = {80};
    vector<double> consultaCuantil85 = {85};
    vector<double> consultaCuantil90 = {90};
    vector<double> consultaCuantil95 = {95};
    vector<double> consultaCuantil98 = {98};
    }

    vector<double> consultaDeciles = {10, 20, 30, 40, 50, 60, 70, 80, 90};

    int numRepeticiones = 2;
    
    // en caso de proporcionar ficheros, se iran procesando de uno en uno
    if(argc>2){
        // si argv[1] == 0 corresponde a un fichero de trazas previamente preprocesado
        // si argv[1] == 1 corresponde a un fichero de trazas previamente preprocesado y con muestra
        // si argv[1] != 0||1 se hara sort de los elementos del fichero (tomara más tiempo)
        vector<double> distribucionArchivoTraza = generadorConsultaArchivo(stof(argv[1]));

        string carpetaPreprocesada = "data/";
        string carpetaPreprocesadaOrdenada = "orden/";
        for(int i=2; i<argc; i++){
            cout << argv[i] << endl;

            // Definición de nombres de archivos a utilizar
            archivoActualTxt = argv[i];
            archivoActual = archivoActualTxt;
            size_t posicion = archivoActual.find(".txt");
            if (posicion != std::string::npos) {
                archivoActual.erase(posicion, 4); // Eliminamos 4 caracteres: .txt
            }
            cout << archivoActualTxt << endl;
            archivoActualSort = carpetaPreprocesada;
            archivoActualSort.append(carpetaPreprocesadaOrdenada);
            archivoActualSort.append("sort");
            archivoActualSort.append(archivoActualTxt);
            archivoActualSortData = carpetaPreprocesada;
            archivoActualSortData.append(carpetaPreprocesadaOrdenada);
            archivoActualSortData.append("sort");
            archivoActualSortData.append(archivoActual);
            archivoActualSortData.append("Data.txt");
            archivoActualMuestra = carpetaPreprocesada;
            archivoActualMuestra.append(carpetaPreprocesadaOrdenada);
            archivoActualMuestra.append("muestrasort");
            archivoActualMuestra.append(archivoActualTxt);

            archivoActualTxt = carpetaPreprocesada;
            archivoActualTxt = archivoActualTxt.append(archivoActual);
            archivoActualTxt = archivoActualTxt.append(".txt");

            if(distribucionArchivoTraza.size()==2 && distribucionArchivoTraza.at(1)==0){
                pruebaVariacionArchivoPreprocesado(distribucionArchivoTraza,consultaCuantiles);
            }else{
                probarVariacionDistribucion(n,0.05,0.01,c,distribucionArchivoTraza,consultaCuantiles,1);
            }

            cout << "archivo: " << archivoActual << endl;
            cout << "archivo Txt: " << archivoActualTxt << endl;
            cout << "archivo Muestra: " << archivoActualMuestra << endl;
            cout << "archivo Sort: " << archivoActualSort << endl;
            cout << "archivo SortData: " << archivoActualSortData << endl;
        }
    }

    
    // Realizar pruebas con los datos sinteticos
    /*
    cout << "MEDIA: 1000, DESVIACIÓN ESTANDAR: 100" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    cout << "MEDIA: 50, DESVIACIÓN ESTANDAR: 10" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    cout << "MEDIA: 500000, DESVIACIÓN ESTANDAR: 100000" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal500000y100000,consultaCuantiles,numRepeticiones);
    cout << "MEDIA: 500000, DESVIACIÓN ESTANDAR: 1000000" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal500000y1000000,consultaCuantiles,numRepeticiones);
    
    
    cout << "ALFA: 5, BETA: 1" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    cout << "ALFA: 3, BETA: 2" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma3y2,consultaCuantiles,numRepeticiones);
    cout << "ALFA: 4, BETA: 20" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma4y20,consultaCuantiles,numRepeticiones);
    
    cout << "LAMDA: 0.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial0p5,consultaCuantiles,numRepeticiones);
    cout << "LAMDA: 1.0" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial1p0,consultaCuantiles,numRepeticiones);
    cout << "LAMDA: 1.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial1p5,consultaCuantiles,numRepeticiones);
    

    cout << "Power Law: 1" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionPowerLaw1,consultaCuantiles,numRepeticiones);
    cout << "Power Law: 2" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionPowerLaw2,consultaCuantiles,numRepeticiones);

    cout << "Uniforme limite inferior:1, limite superior:100 " << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionUniforme1y100,consultaCuantiles,numRepeticiones);
    cout << "Uniforme limite inferior:50000, limite superior:10000000 " << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionUniforme50000y10000000,consultaCuantiles,numRepeticiones);

    cout << "GEOMETRICA LAMDA: 0.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGeometrica0p5,consultaCuantiles,numRepeticiones);
    cout << "GEOMETRICA LAMDA: 1.0" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGeometrica1p0,consultaCuantiles,numRepeticiones);
    cout << "GEOMETRICA LAMDA: 1.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGeometrica1p5,consultaCuantiles,numRepeticiones);
    */

    return 0;
}
