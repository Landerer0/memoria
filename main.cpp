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

string archivoActualTxt = "";
string archivoActual = "";


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

    long kllEstimate = kll.quantile(cuantil);
    long truthValue = truth.at(posTruth);
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

    long elemento = truth.at(posTruth);

    long kllEstimate = kll.rank(elemento);
    long truthValue = posTruth;
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
    // parametros 1
    for(int i=0;i<numRepeticiones;i++){
        KLL kll1(n,epsilon,delta,c);
        vector<double> arrTruth1;

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
        
    // Calculo de estadisticas
    double rankMayor1,rankMenor1;
    rankMayor1=-1;
    rankMenor1=10000000;
    vector<double> rankAverage1;
    double quantileMayor1,quantileMenor1;
    quantileMayor1=-1;
    quantileMenor1=10000000;
    vector<double> quantileAverage1;

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
    } else if (distribucion.at(0)==-1) {
        filenameEspecifications = filenameEspecifications + archivoActual;
    }

    // se agrega identificadores tanto de epsilon como de delta al nombre del archivo para poder diferenciar
    // las pruebas con variaciones de estos parametros pero con la misma distribucion (y parametros)
    filenameEspecifications = filenameEspecifications + "e" + to_string(epsilon) + "d" + to_string(delta);


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

    // imprimir por pantalla los resultados de esta sesion de experimentos, tanto para rank como quantile
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

    cout << "Cantidad de veces que el error supero la proporcion entregada por epsilon." << endl
         << "Rank: " << rankMayorEpsilon << endl
         << "Quantile: " << quantileMayorEpsilon << endl << endl;
    return;
}

// aqui se definen funciones para generar las distintas consultas
// se utiliza principalmente para la creacion de distribuciones
vector<double> generadorConsultaArchivo(){
    vector<double> resultado;
    resultado.push_back(-1);   
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

int main(int argc, char*argv[]){
    // DEFINICION DE PARAMETROS
    unsigned long n= pow(2,20);
    double epsilon = 0.05;
    double delta = 0.001;
    double c = (double) 2/(double) 3;

    // Generación de vectores para consultas de distintas distribuciones
    // datos entregados mediante ficheros
    vector<double> distribucionArchivo = generadorConsultaArchivo();
    // datos creados mediante distribuciones
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

    // creación de vectores para consulta de cuantiles
    vector<double> consultaCuantiles = {2,5,10,20,30,40,50,60,70,80,90,95,98};
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

    int numRepeticiones = 50;
    
    // en caso de proporcionar ficheros, se iran procesando de uno en uno
    if(argc>1){
        for(int i=1; i<argc; i++){
            cout << argv[i] << endl;
            archivoActualTxt = argv[i];
            cout << archivoActualTxt << endl;
            archivoActual = archivoActualTxt;
            size_t posicion = archivoActual.find(".txt");
            
            if (posicion != std::string::npos) {
                archivoActual.erase(posicion, 4); // Eliminamos 4 caracteres: .txt
            }
            probarVariacionDistribucion(n,0.05,0.01,c,distribucionArchivo,consultaCuantiles,1);
            cout << archivoActual << endl;
        }
    }


    /*
    // Realizar pruebas con los datos sinteticos
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
    */

    return 0;
}
