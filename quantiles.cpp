// g++ quantiles.cpp kll.cpp kll.hpp -o quantiles
// ./quantiles 10 Chicago-20080319.txt

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

#include "kll.hpp"

using namespace std;

// Parametros globales
unsigned long numElements = pow(2,22); // numero de elementos totales
unsigned long numElements2 = pow(2,7); // numero de elementos del segundo archivo

unsigned long rankMayorEpsilon = 0;
unsigned long quantileMayorEpsilon = 0;

double epsilon = 0.05; // tamaño del arreglo mas grande
double numC=(double)2/(double)3; // factor por el que va disminuyendo el tamaño de cada arreglo a medida pasan los niveles.
bool manualLectura = false;

string archivoActual = "";
string archivoActualTxt = "";
string archivoActualMuestra = "";
string archivoActualSort = "";
string archivoActualSortData = "";


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
    double errorValue = (double)abs(truthValue-kllEstimate)/(double)truthValue;

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
    double errorValue = (double)abs(truthValue-kllEstimate)/(double)truthValue;

    cerr << "Truth: " << truthValue << ", KLL: " << kllEstimate << endl;

    //cout<<" quantile " << (double)cuantil/(double)100 << ": KLL: " <<kllEstimate << " Truth: " << truthValue 
    //<< " diferencia |q(truth)-q(kll)|: " << abs(kllEstimate-truthValue)<< " ,  error: " << errorValue <<  endl;
    
    if(errorValue>epsilon){
        //cout << "QUANTILE ERROR mayor a epsilon en cuantil " << cuantil << endl; 
        quantileMayorEpsilon++;
    } 
    error.push_back(errorValue);
}

void pruebaKLLPreprocesado(KLL kll, unsigned long n,vector<double> cuantilesAConsultar,vector<double> &rankError,vector<double> &quantilesError){
    {
        for(int i=0;i<cuantilesAConsultar.size();i++){
            quantileTruthKLLPreprocesado(kll,(long)cuantilesAConsultar.at(i),n,quantilesError);
        }
    }

    {
        for(int i=0;i<cuantilesAConsultar.size();i++){
            rankTruthKLLPreprocesado(kll,(long)cuantilesAConsultar.at(i),n, rankError);
        }
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

void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, vector<double> &consultaCuantiles,int &numRepeticiones
                    , vector<vector<double>> &rank, vector<vector<double>> &quantile){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultados/";
    // Nombre del archivo de salida
    std::string filenameParametros = nombreCarpeta+"datos"+filenameEspecifications+"Parametros.txt";
    std::string filenameConsulta = nombreCarpeta+"datos"+filenameEspecifications+"Consulta.txt";
    std::string filenameRank = nombreCarpeta+"datos"+filenameEspecifications+"Rank.txt";
    std::string filenameQuantile = nombreCarpeta+"datos"+filenameEspecifications+"Quantile.txt";

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
        for (const auto& element : consultaCuantiles) {
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
    
    if(quantile.size()!=0){
        std::ofstream outfileQuantile(filenameQuantile); // errorRelativo obtenido de los experimentos
        if (outfileQuantile.is_open()) {
            for (const auto& inner_vector : quantile) {
                for (const auto& element : inner_vector) {
                    outfileQuantile << element << " ";
                }
                outfileQuantile << std::endl;
            }
            outfileQuantile.close();
            std::cout << "Los datos Quantile se han almacenado correctamente en el archivo " << filenameQuantile << std::endl;
        } else {
            std::cerr << "No se pudo abrir el archivo Quantile " << filenameQuantile << " para escritura." << std::endl;
        }    
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

    // ahora se procede de manera distinta dependiendo de si se trata de un fichero con trazas o no
    // esto con el fin de ahorrar tiempo de ejecución

    if(distribucion.at(0)==-1 && distribucion.size()>1){
        KLL kll1(n,epsilon,delta,c,2);

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

        
        if(distribucion.size()== 2) pruebaKLLPreprocesado(kll1,n,consultaCuantiles,rankError1,quantilesError1);
        else if(distribucion.size()==3) ranksConsultados = pruebaKLLMuestraPreprocesado(kll1,n,rankError1);
        

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
            KLL kll1(n,epsilon,delta,c,2);
            vector<double> arrTruth1;
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

            sort(arrTruth1.begin(), arrTruth1.end());

            vector<double> quantilesError1;
            vector<double> rankError1;

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
            } 
            
            
        }
    }

    // Se define nombre del archivo resultante a partir del archivo o distribución indicada
    string filenameEspecifications;
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
    
    cout << "Quantile: " << endl;
    for(int i=0;i<consultaCuantiles.size();i++){
        cout << "Quantile de cuantil " << consultaCuantiles[i]
             << ": " << quantile.at(0).at(i)
             << endl;
    }
    // se agrega identificadores tanto de epsilon como de delta al nombre del archivo para poder diferenciar
    // las pruebas con variaciones de estos parametros pero con la misma distribucion (y parametros)
    filenameEspecifications = filenameEspecifications + "e" + to_string(epsilon) + "d" + to_string(delta);

    almacenarDatos(filenameEspecifications, parametrosKLL, consultaCuantiles, numRepeticiones, rank, quantile);

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

//prueba(n,0.05,0.010000,c,distribucion,consultaCuantiles,numRepeticiones);


int main(int argc, char*argv[]){
    int numFrac = stoi(argv[1]);
    double x = 1.0;
    vector<double> consulta;
    for(int i=1;i<numFrac;i++){
        consulta.push_back((x*i*100/(double)numFrac));
        cout << consulta.at(i-1) << endl;
    }
    uint64_t n;

    string carpetaPreprocesada = "data/";
    string carpetaPreprocesadaOrdenada = "orden/";

            // Definición de nombres de archivos a utilizar
            archivoActualTxt = argv[2];
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

            cout << "archivo: " << archivoActual << endl;
            cout << "archivo Txt: " << archivoActualTxt << endl;
            cout << "archivo Muestra: " << archivoActualMuestra << endl;
            cout << "archivo Sort: " << archivoActualSort << endl;
            cout << "archivo SortData: " << archivoActualSortData << endl;

    int lines = 0;
    char endline_char = '\n';
    fstream in(archivoActualTxt);
        while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
        {
            ++lines;
        }
        n = lines-1;
        cout << "En en el archivo " << archivoActual << " hay " << n << " lineas" << endl;

    KLL kll(n,0.05,0.001,2.0/3.0,2);
    KLL kllkmin(200);
    std::ifstream archivo(archivoActualTxt);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) 
                //cerr << dato;
                kll.add(dato);
                kllkmin.add(dato);
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << argv[2] << std::endl;
        return 0;
    }

    vector<uint64_t> rankElements, rankkminElements;
    vector<double> quantileElements, quantilekminElements;

    vector <double> consultaCuantiles = {10,20,30,40,50,60,70,80,90};
    vector<double> rankError1, rankkminError1, quantilesError1, quantileskminError1;

    cout << "PRUEBA----------------------------------------------------------------------" << endl;
    pruebaKLLPreprocesado(kll,n,consultaCuantiles,rankError1,quantilesError1);
    cout << "FINPRUEBA----------------------------------------------------------------------" << endl;
    cout << "PRUEBAKMIN----------------------------------------------------------------------" << endl;
    pruebaKLLPreprocesado(kllkmin,n,consultaCuantiles,rankkminError1,quantileskminError1);
    cout << "FINPRUEBAKMIN----------------------------------------------------------------------" << endl;



    quantileElements = kll.quantile(consulta);
    rankElements = kll.rank(quantileElements);
    quantilekminElements = kllkmin.quantile(consulta);
    rankkminElements = kllkmin.rank(quantilekminElements);

    cout << "KLL: " << kll.sizeInBytes() << endl;
    //kll.print();
    cout << "KLL_KMIN: " << kllkmin.sizeInBytes() << endl;
    //kllkmin.print();

    for(int i=0;i<consulta.size();i++){
        cout << "(normal)" << i+1 << " quantile" << ": " << (uint64_t)quantileElements.at(i) << " (decil " << consulta.at(i) << "), rank de " 
        << (uint64_t)quantileElements.at(i) << ": " << (uint64_t)rankElements.at(i) << ", normalized rank: " << (uint64_t)rankElements.at(i)/(double)n << endl;
        cout << "(k_min)" << i+1 << " quantile" << ": " << (uint64_t)quantilekminElements.at(i) << " (decil " << consulta.at(i) << "), rank de " 
        << (uint64_t)quantilekminElements.at(i) << ": " << (uint64_t)rankkminElements.at(i) << ", normalized rank: " << (uint64_t)rankkminElements.at(i)/(double)n << endl;
    }

    return 0;
}