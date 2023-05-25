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

void quantileTruthKLL(KLL &kll, vector<long> truth, double cuantil, double n, vector<double> &error){
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

void rankTruthKLL(KLL &kll, vector<long> truth, double cuantil, double n, vector<double> &error){
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

void pruebaKLL(KLL kll, vector<long> arrTruth, double epsilon, double delta, unsigned long n,vector<double> cuantilesAConsultar,vector<double> &rankError,vector<double> &quantilesError){
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
    // parametros 1
    for(int i=0;i<numRepeticiones;i++){
        KLL kll1(n,epsilon,delta,c);
        vector<long> arrTruth1;

        switch((int)tipoDistribucion){
            case 0: //normal
            {
                if(distribucion.size()!=3) return;
                std::normal_distribution<> d0{distribucion.at(1), distribucion.at(2)};
                for(int i=0;i<n;i++){
                    long toAdd = (long) d0(generator);
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
                    long toAdd = (long) d1(generator);
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
                    long toAdd = (long) (d2(generator)*100);
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
        /*
        cout << "------------------RESULTADOS------------------------" << endl;
        cout << "Rank:" << endl;
        cout << "Error más alto: "<< *max_element(rankError1.begin(), rankError1.end()) << endl;
        cout << "Error promedio: " << accumulate( rankError1.begin(), rankError1.end(), 0.0)/rankError1.size() << endl;
        cout << "Error más bajo: " << *min_element(rankError1.begin(), rankError1.end()) << endl;
        cout << "Quantile:" << endl;
        cout << "Error más alto: "<< *max_element(quantilesError1.begin(), quantilesError1.end()) << endl;
        cout << "Error promedio: " << accumulate( quantilesError1.begin(), quantilesError1.end(), 0.0)/quantilesError1.size() << endl;
        cout << "Error más bajo: " << *min_element(quantilesError1.begin(), quantilesError1.end()) << endl;
        */
        rank.push_back(rankError1);
        quantile.push_back(quantilesError1);

        if(i+1==numRepeticiones)  parametrosKLL = kll1.parametros();
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
    }

    filenameEspecifications = filenameEspecifications + "e" + to_string(epsilon) + "d" + to_string(delta);


    std::string nombreCarpeta = "resultados/";
    // Nombre del archivo de salida
    std::string filenameParametros = nombreCarpeta+"datos"+filenameEspecifications+"Parametros.txt";
    std::string filenameConsulta = nombreCarpeta+"datos"+filenameEspecifications+"Consulta.txt";
    std::string filenameRank = nombreCarpeta+"datos"+filenameEspecifications+"Rank.txt";
    std::string filenameQuantile = nombreCarpeta+"datos"+filenameEspecifications+"Quantile.txt";

    // Abrir el archivo en modo de escritura
    std::ofstream outfileParametros(filenameParametros);

    if (outfileParametros.is_open()) {
        for (const auto& element : parametrosKLL) {
            // Escribir cada elemento en el archivo
            outfileParametros << element << " ";
        }
        outfileParametros << std::endl; // Nueva línea para separar los vectores internos

        // Cerrar el archivo
        outfileParametros.close();

        std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << filenameParametros << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Parametros " << filenameParametros << " para escritura." << std::endl;
    }

    // Abrir el archivo en modo de escritura
    std::ofstream outfileConsulta(filenameConsulta);

    if (outfileConsulta.is_open()) {
        for (const auto& element : consultaCuantiles) {
            // Escribir cada elemento en el archivo
            outfileConsulta << element << " ";
        }
        outfileConsulta << std::endl; // Nueva línea para separar los vectores internos

        // Cerrar el archivo
        outfileConsulta.close();

        std::cout << "Los datos Consulta se han almacenado correctamente en el archivo " << filenameConsulta << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Consulta " << filenameConsulta << " para escritura." << std::endl;
    }


    // Abrir el archivo en modo de escritura
    std::ofstream outfileRank(filenameRank);

    if (outfileRank.is_open()) {
        // Recorrer los elementos del vector<vector<double>>
        for (const auto& inner_vector : rank) {
            // Recorrer los elementos del vector<double>
            for (const auto& element : inner_vector) {
                // Escribir cada elemento en el archivo
                outfileRank << element << " ";
            }
            outfileRank << std::endl; // Nueva línea para separar los vectores internos
        }

        // Cerrar el archivo
        outfileRank.close();

        std::cout << "Los datos rank se han almacenado correctamente en el archivo " << filenameRank << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo rank " << filenameRank << " para escritura." << std::endl;
    }

    // Abrir el archivo en modo de escritura
    std::ofstream outfileQuantile(filenameQuantile);

    if (outfileQuantile.is_open()) {
        // Recorrer los elementos del vector<vector<double>>
        for (const auto& inner_vector : quantile) {
            // Recorrer los elementos del vector<double>
            for (const auto& element : inner_vector) {
                // Escribir cada elemento en el archivo
                outfileQuantile << element << " ";
            }
            outfileQuantile << std::endl; // Nueva línea para separar los vectores internos
        }

        // Cerrar el archivo
        outfileQuantile.close();

        std::cout << "Los datos Quantile se han almacenado correctamente en el archivo " << filenameQuantile << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo Quantile " << filenameQuantile << " para escritura." << std::endl;
    }


    for(int i=0;i<consultaCuantiles.size();i++){
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

    for(int i=0;i<consultaCuantiles.size();i++){
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

    /*
    for(int i=0; i<rank.size();i++){
        for(int j=0;j<rank.at(i).size();j++){
            double quantileActualMayor = *max_element(quantile.at(i).begin(), quantile.at(i).end());
            double quantileActualMenor = *min_element(quantile.at(i).begin(), quantile.at(i).end());
            if(quantileActualMayor>quantileMayor1) quantileMayor1 = quantileActualMayor;
            if(quantileActualMenor<quantileMenor1) quantileMenor1 = quantileActualMenor;
            quantileAverage1.push_back(accumulate(quantile.at(i).begin(), quantile.at(i).end(), 0.0)/quantile.at(i).size());

            double rankActualMayor = *max_element(rank.at(i).begin(), rank.at(i).end());
            double rankActualMenor = *min_element(rank.at(i).begin(), rank.at(i).end());
            if(rankActualMayor>rankMayor1) rankMayor1 = rankActualMayor;
            if(rankActualMenor<rankMenor1) rankMenor1 = rankActualMenor;
            rankAverage1.push_back(accumulate(rank.at(i).begin(), rank.at(i).end(), 0.0)/rank.at(i).size());
        }
    }

    cout << "Cuantiles consultados: " << endl;
    for(int i=0;i<consultaCuantiles.size();i++){
        cout << consultaCuantiles.at(i) << " ";
    }
    cout << endl;
    cout << "Estadisticas de " << numRepeticiones << " repeticiones. Epsilon: " << epsilon << ", Delta: " << delta << endl;
    cout << "Rank" << endl;
    cout << "Error más grande: " << rankMayor1 << endl;
    cout << "Error promedio: " << accumulate(rankAverage1.begin(), rankAverage1.end(), 0.0)/rankAverage1.size() << endl;
    cout << "Error más pequeño: " << rankMenor1 << endl;
    cout << "Quantile" << endl;
    cout << "Error más grande: " << quantileMayor1 << endl;
    cout << "Error promedio: " << accumulate(quantileAverage1.begin(), quantileAverage1.end(), 0.0)/quantileAverage1.size() << endl;
    cout << "Error más pequeño: " << quantileMenor1 << endl;

    */

    cout << "Cantidad de veces que el error supero la proporcion entregada por epsilon." << endl
         << "Rank: " << rankMayorEpsilon << endl
         << "Quantile: " << quantileMayorEpsilon << endl << endl;
    return;
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

void probarVariacionDistribucion(unsigned long n, double epsilon, double delta, double c, vector<double> distribucion, vector<double> consultaCuantiles, long numRepeticiones){
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.00001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000001,c,distribucion,consultaCuantiles,numRepeticiones);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.005,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.0005,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    prueba(n,0.00005,0.0001,c,distribucion,consultaCuantiles,numRepeticiones);
    
    return;
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
    vector<double> distribucionNormal100000y100 = generadorConsultaVectorNormal(100000,100);
    
    vector<double> distribucionGamma5y1 = generadorConsultaVectorGamma(5,1);
    vector<double> distribucionGamma3y2 = generadorConsultaVectorGamma(3,2);
    vector<double> distribucionGamma4y20 = generadorConsultaVectorGamma(4,20);

    vector<double> distribucionExponencial0p5 = generadorConsultaVectorExponencial(0.5);
    vector<double> distribucionExponencial1p0 = generadorConsultaVectorExponencial(1.0);
    vector<double> distribucionExponencial1p5 = generadorConsultaVectorExponencial(1.5);

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
    /*
    cout << "MEDIA: 1000, DESVIACIÓN ESTANDAR: 100" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    cout << "MEDIA: 50, DESVIACIÓN ESTANDAR: 10" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    
    cout << "ALFA: 5, BETA: 1" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    cout << "ALFA: 3, BETA: 2" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma3y2,consultaCuantiles,numRepeticiones);
    cout << "ALFA: 4, BETA: 20" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionGamma4y20,consultaCuantiles,numRepeticiones);
    */
    cout << "LAMDA: 0.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial0p5,consultaCuantiles,numRepeticiones);
    cout << "LAMDA: 1.0" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial1p0,consultaCuantiles,numRepeticiones);
    cout << "LAMDA: 1.5" << endl;
    probarVariacionDistribucion(n,0.05,0.01,c,distribucionExponencial1p5,consultaCuantiles,numRepeticiones);
/*
    cout << "MEDIA: 1000, DESVIACIÓN ESTANDAR: 100" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.00001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.005,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.0005,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    prueba(n,0.00005,0.0001,c,distribucionNormal1000y100,consultaCuantiles,numRepeticiones);
    
    cout << "MEDIA: 50, DESVIACIÓN ESTANDAR: 10" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.00001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.005,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.0005,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
    prueba(n,0.00005,0.0001,c,distribucionNormal50y10,consultaCuantiles,numRepeticiones);
   
    cout << "ALFA: 5, BETA: 1" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.00001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.000001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.05,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.005,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.0005,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
    prueba(n,0.00005,0.0001,c,distribucionGamma5y1,consultaCuantiles,numRepeticiones);
   
    cout << "ALFA: 3, BETA: 2" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,3,2);
    prueba(n,0.05,0.001,c,3,2);
    prueba(n,0.05,0.0001,c,3,2);
    prueba(n,0.05,0.00001,c,3,2);
    prueba(n,0.05,0.000001,c,3,2);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,3,2);
    prueba(n,0.05,0.0001,c,3,2);
    prueba(n,0.005,0.0001,c,3,2);
    prueba(n,0.0005,0.0001,c,3,2);
    prueba(n,0.00005,0.0001,c,3,2);

    cout << "ALFA: 4, BETA: 20" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,4,20);
    prueba(n,0.05,0.001,c,4,20);
    prueba(n,0.05,0.0001,c,4,20);
    prueba(n,0.05,0.00001,c,4,20);
    prueba(n,0.05,0.000001,c,4,20);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,4,20);
    prueba(n,0.05,0.0001,c,4,20);
    prueba(n,0.005,0.0001,c,4,20);
    prueba(n,0.0005,0.0001,c,4,20);
    prueba(n,0.00005,0.0001,c,4,20);

    cout << "LAMDA: 0.5" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,0.5);
    prueba(n,0.05,0.001,c,0.5);
    prueba(n,0.05,0.0001,c,0.5);
    prueba(n,0.05,0.00001,c,0.5);
    prueba(n,0.05,0.000001,c,0.5);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,0.5);
    prueba(n,0.05,0.0001,c,0.5);
    prueba(n,0.005,0.0001,c,0.5);
    prueba(n,0.0005,0.0001,c,0.5);
    prueba(n,0.00005,0.0001,c,0.5);
    
    cout << "LAMDA: 1.5" << endl;
    cout << "VARIACIONES DELTA:" << endl;
    prueba(n,0.05,0.01,c,1.5);
    prueba(n,0.05,0.001,c,1.5);
    prueba(n,0.05,0.0001,c,1.5);
    prueba(n,0.05,0.00001,c,1.5);
    prueba(n,0.05,0.000001,c,1.5);
    cout << "VARIACIONES EPSILON:" << endl << endl;
    prueba(n,0.5,0.0001,c,1.5);
    prueba(n,0.05,0.0001,c,1.5);
    prueba(n,0.005,0.0001,c,1.5);
    prueba(n,0.0005,0.0001,c,1.5);
    prueba(n,0.00005,0.0001,c,1.5);
    */

    return 0;
}
