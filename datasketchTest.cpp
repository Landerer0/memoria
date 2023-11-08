// g++ -o datasketchtest datasketchtest.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -std=c++11 -I/usr/local/include/DataSketches/ -o datasketch
// g++ -o datasketchtest datasketchtest.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -std=c++11 -I/media/sf_shared/memoria/include/DataSketches/ -o datasketch
//         k        numFractions  Archivo
// ./datasketch 200      100            Chicago-20160121.txt
// ./datasketch 200      100            Mendeley.txt
// ./datasketch 200      100            Mawi-20181201.txt
// ./datasketch 200      100            Sanjose-20081016.txt
// gdb -ex=r --args datasketchtest 200 10 Mendeley.txt

// Pasar el archivo al chome
// scp kllplaw.cpp lkraemer2018@chome.inf.udec.cl:/home/lkraemer2018/

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

#include <kll_sketch.hpp>

using namespace std;

bool incluirTruthValues = true;
string archivoActual = "";
string archivoActualTxt = "";
string archivoActualMuestra = "";
string archivoActualSort = "";
string archivoActualSortData = "";

void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, vector<double> &consultaCuantiles
                    , vector<double> &rank, vector<double> &quantile, double tiempo, int k){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultadosDatasketch/";
    string nombreK = "";
    if(k>0) nombreK = to_string(k);
    // Nombre del archivo de salida
    std::string filename = nombreCarpeta+"datos"+filenameEspecifications+"Resultados"+nombreK+".txt";

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
                for (const auto& element : rank) {
                    outfile << element << " ";
                }
                outfile << std::endl; 
            
            std::cout << "Los datos rank se han almacenado correctamente en el archivo " << filename << std::endl;
        }
        
        if(quantile.size()!=0){
                for (const auto& element : quantile) {
                    outfile << element << " ";
                }
                outfile << std::endl;
            
            std::cout << "Los datos Quantile se han almacenado correctamente en el archivo " << filename << std::endl;
        }
        
        outfile << tiempo << endl;

        outfile.close();
    }
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

int main(int argc, char **argv) {
    uint64_t buildTime = 0;
    uint64_t searchTime = 0;

    std::vector<double> v;

    std::string line;
    int numFractions = stoi(argv[2]);
    string nombreArchivo = argv[3];
    string fichero = "data/";
    fichero = fichero + nombreArchivo;
    std::ifstream myfile (fichero);
    int totalelems=0;
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
		std::stringstream s(line);
		double x;
		s>>x;
		v.push_back(x);
		totalelems++;
    	}
    	myfile.close();
    }

    double x = 1.0;
    vector<double> consulta;
    for(int i=0;i<=numFractions;i++){
        consulta.push_back((i*100.0/(double)numFractions));
    }
    uint64_t n = 0;
    int lines = 0;
    char endline_char = '\n';
    fstream in(fichero);
        while (in.ignore(numeric_limits<streamsize>::max(), in.widen(endline_char)))
        {
            ++lines;
        }
        n = lines-1;
        cout << "En en el archivo " << fichero << " hay " << n << " lineas" << endl;


    vector<uint64_t> truthRank;
    vector<double> truthQuantile;
    vector<double> elementosConsulta;
    vector<double> consultaCuantiles;

    for(int i=0;i<=numFractions;i++){
        consultaCuantiles.push_back(i);
    }

    if(incluirTruthValues){
      
      string carpetaPreprocesada = "data/";
      string carpetaPreprocesadaOrdenada = "orden/";

      archivoActualTxt = argv[3];
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

      obtenerTruthRankQuantileArchivoPreprocesado(n, consulta, elementosConsulta, truthRank, truthQuantile);
    }


    vector<int> kUsar= {100,150,200,250};
    for(int ks=0;ks<kUsar.size();ks++){

        int k = kUsar.at(ks);
        auto startTotal = std::chrono::high_resolution_clock::now();
        datasketches::kll_sketch<double> apacheSketch(k); // default k=200
        
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < v.size(); i++) {
        apacheSketch.update(v[i]); 
        }
        auto end = std::chrono::high_resolution_clock::now();
        buildTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::ofstream os1("kll_sketch_plaw.bin");
        apacheSketch.serialize(os1);
        // std::cout<<"sketch data "<<apacheSketch.to_string(true,true)<<"\n";

    

    // this section deserializes the sketches, produces a union and prints some results
        std::ifstream is1("kll_sketch_plaw.bin");
        auto datasketch = datasketches::kll_sketch<double>::deserialize(is1);
        double fractions[numFractions+1];
        double step = 1.0/double(numFractions);
        for(int i=0;i<=numFractions;i++){
            fractions[i]= 1.0*i/(double)numFractions;
        }
        start = std::chrono::high_resolution_clock::now();
        auto qt1 = datasketch.get_quantiles(fractions, numFractions+1);
        end = std::chrono::high_resolution_clock::now();
        searchTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        

        vector <double> rk1;

        start = std::chrono::high_resolution_clock::now();
        for(int i=0;i<truthQuantile.size();i++){
            rk1.push_back(datasketch.get_rank(truthQuantile.at(i)));
        }
        end = std::chrono::high_resolution_clock::now();
        searchTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();


        auto endTotal = std::chrono::high_resolution_clock::now();
        uint64_t tiempo = std::chrono::duration_cast<std::chrono::microseconds>(endTotal - startTotal).count();


        if(incluirTruthValues){
            cerr << "Truth Rank" << "\tDatasketch Rank" << endl;
            for(int i=0;i<truthRank.size();i++){
            cerr << i << " " << truthRank.at(i) << "\t" << rk1.at(i)*n << endl;
            }
            cerr << endl;
            cerr << "Truth Quantile" << "\tDatasketch Quantile" << endl;
            for(int i=0;i<truthQuantile.size();i++){
            cerr << i << " " << truthQuantile.at(i) << "\t" << qt1.at(i) << endl;
            }
        }

        vector<double> parametrosDatasketch;
        vector<double> rankError;
        vector<double> quantileError;
        for(int i=0;i<=numFractions;i++){
            rankError.push_back(abs((double)truthRank.at(i)/n-rk1.at(i)));

            double errorValueQuantile;
            if(truthQuantile.at(i)!=0) errorValueQuantile = (double)abs(truthQuantile.at(i)-qt1.at(i))/(double)truthQuantile.at(i);
            else errorValueQuantile = (double)abs(truthQuantile.at(i)-qt1.at(i))/(double)1.0;
            
            quantileError.push_back(errorValueQuantile);
        }
        parametrosDatasketch.push_back(n);
        parametrosDatasketch.push_back(k);
        parametrosDatasketch.push_back(buildTime);
        parametrosDatasketch.push_back(searchTime);

        almacenarDatos(nombreArchivo, parametrosDatasketch, consultaCuantiles, rankError , quantileError , tiempo, k);
    }
    // no se puede hacer por error: "Values must be unique and monotonically increasing" en .get_PMF()
    // vector<double> rk2 = datasketch.get_PMF(truthQuantile.data(),truthQuantile.size(),true);
    // for(int i=0;i<rk2.size();i++){
    //     cerr << i << "\t" << rk2.at(i) << endl;
    // }

  //cout << "sketch data: " << apacheSketch.to_string(true,true) << endl;
  

  return 0;
}
