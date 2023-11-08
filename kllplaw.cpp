// g++ -o kllplaw kllplaw.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -std=c++11 -I/usr/local/include/DataSketches/
// g++ -o kllplaw kllplaw.cpp streamsketch.hpp streamsketch.cpp kll.hpp kll.cpp -std=c++11 -I/media/sf_shared/memoria/include/DataSketches/
//         k        numFractions  Archivo
// ./kllplaw 200      10            Chicago-20080319.txt
// ./kllplaw 200      10            Mendeley.txt
// ./kllplaw 200      10            Mawi-20161201.txt
// gdb -ex=r --args kllplaw 200 10 Mendeley.txt

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
  

    std::vector<double> v;

    std::string line;
    string fichero = "data/";
    fichero = fichero + argv[3];
    std::ifstream myfile (fichero);
    int totalelems=0;
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
		//float x = std::stof( line );
		//float x = atof( line.c_str() );
		std::stringstream s(line);
		//printf("x %s %f %d \n", line.c_str(), x, *ending);
		double x;
		s>>x;
		//printf("x %s %lf \n", line.c_str(), x);
		v.push_back(x);
		totalelems++;
    	}
    	myfile.close();
    }

    int numFrac = stoi(argv[2]);
    double x = 1.0;
    vector<double> consulta;
    for(int i=0;i<=numFrac;i++){
        consulta.push_back((i*100.0/(double)numFrac));
        cout << consulta.at(i) << endl;
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

    //KLL kll(n,0.05,0.001,2.0/3.0,20);
    //KLL kllkmin(200);
    
    double epsilon = 0.05;
    double delta = 0.001;
    double c=2.0/3.0;
    uint64_t lineaActual = 0;
    KLL kllUno(n,epsilon,delta,c,20);
    vector<KLL> kllParticionado;
    for(int i=0;i<4;i++) kllParticionado.push_back(KLL(n/4,epsilon,delta,c,20));

    // StreamSketch klleps(n/5,0.01,2.0/3.0,2);
    // cerr << "KLLEPS" << endl;
    // StreamSketch mrleps(0.01,n/5);

    std::ifstream archivo(fichero);
    std::string linea;
    if(archivo.is_open()) {
        while (std::getline(archivo, linea)) {
            double dato;
            std::istringstream iss(linea);
            if (iss >> dato) 
                //cerr << dato;
                //kll.add(dato);
                //kllkmin.add(dato);
                // klleps.add(dato);
                // mrleps.add(dato);
                kllUno.add(dato);
                if(lineaActual<(n/4)) kllParticionado.at(0).add(dato);
                else if(lineaActual<(n/2)) kllParticionado.at(1).add(dato);
                else if(lineaActual<(3*n/4)) kllParticionado.at(2).add(dato);
                else if(lineaActual<(n)) kllParticionado.at(3).add(dato);
                lineaActual++;
        }
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo: " << argv[2] << std::endl;
        return 0;
    }

    KLL kllFinal = kllParticionado.at(0).kllMerge(kllParticionado.at(1));
    kllFinal = kllFinal.kllMerge(kllParticionado.at(2));
    kllFinal = kllFinal.kllMerge(kllParticionado.at(3));

    //kll.saveData("kll.bin");
    //KLL kllres = kll.readData("kll.bin");
    //kllkmin.saveData("kllkmin.bin");
    //KLL kllkminres = kllkmin.readData("kllkmin.bin");

    vector<uint64_t> rankElements, rankkminElements;
    vector<double> quantileElements, quantilekminElements;

    // quantileElements = kll.quantile(consulta);
    // rankElements = kll.rank(quantileElements);
    // quantilekminElements = kllkmin.quantile(consulta);
    // rankkminElements = kllkmin.rank(quantilekminElements);
    // quantileElements = klleps.quantile(consulta);
    // rankElements = klleps.rank(quantileElements);
    // quantilekminElements = mrleps.quantile(consulta);
    // rankkminElements = mrleps.rank(quantilekminElements);
    quantileElements = kllUno.quantile(consulta);
    rankElements = kllUno.rank(quantileElements);
    quantilekminElements = kllFinal.quantile(consulta);
    rankkminElements = kllFinal.rank(quantilekminElements);

    //cout << "KLL: " << endl << kll.sizeInBytes() << endl;
    //kll.print();
    //cout << "KLL_KMIN: " << endl << kllkmin.sizeInBytes() << endl;
    //kllkmin.print();

    /*
    for(int i=0;i<consulta.size();i++){
        cout << "(normal)" << i+1 << " quantile" << ": " << (uint64_t)quantileElements.at(i) << " (decil " << consulta.at(i) << "), rank de " 
        << (uint64_t)quantileElements.at(i) << ": " << (uint64_t)rankElements.at(i) << ", normalized rank: " << (uint64_t)rankElements.at(i)/(double)n << endl;
        cout << "(k_min)" << i+1 << " quantile" << ": " << (uint64_t)quantilekminElements.at(i) << " (decil " << consulta.at(i) << "), rank de " 
        << (uint64_t)quantilekminElements.at(i) << ": " << (uint64_t)rankkminElements.at(i) << ", normalized rank: " << (uint64_t)rankkminElements.at(i)/(double)n << endl;
    }

    std::cout<<" i quantile(fractions) value_in_sortedstream_for_quantile\n";
    for(int i=0; i<=numFrac; i++){
	   printf("(normal)%d\t(%lf)\t%lf \n",i,consulta.at(i)/100.0,quantileElements.at(i)); 
	   printf("(k_min)%d\t(%lf)\t%lf \n",i,consulta.at(i)/100.0,quantilekminElements.at(i)); 
    }

    std::cout<<" i quantile rank(fractions)\n";
    for(int i=0; i<=numFrac; i++){
	   printf("(normal)%d\t(%lf)\t%lf \n",i,quantileElements.at(i),rankElements.at(i)/(double)n); 
	   printf("(k_min)%d\t(%lf)\t%lf \n",i,quantilekminElements.at(i),rankkminElements.at(i)/(double)n); 
    }
    */

    datasketches::kll_sketch<double> sketch1(atoi(argv[1])); // default k=200
    for (int i = 0; i < v.size(); i++) {
      sketch1.update(v[i]); // mean=0, stddev=1
    }

    std::ofstream os1("kll_sketch_plaw.bin");
    sketch1.serialize(os1);
    // std::cout<<"sketch data "<<sketch1.to_string(true,true)<<"\n";

  

  // this section deserializes the sketches, produces a union and prints some results
  
    int nfrac = atoi(argv[2]);
    std::ifstream is1("kll_sketch_plaw.bin");
    auto sketch11 = datasketches::kll_sketch<double>::deserialize(is1);
    double fractions[nfrac+1];
    double step = 1.0/double(nfrac);
    for(int i=0;i<=nfrac;i++){
        fractions[i]= 1.0*i/(double)nfrac;
    }
    auto qt1 = sketch11.get_quantiles(fractions, nfrac+1);
    
    vector<uint64_t> truthRank;
    vector<double> truthQuantile;
    vector<double> elementosConsulta;

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

    if(incluirTruthValues){
      for(int i=0;i<truthRank.size();i++){
        cerr << i << " " << truthRank.at(i) << endl;
      }
    }

    auto rank  = sketch1.get_rank(3232235891);
    auto rank2 = sketch1.get_rank(3232235890);
    cout << setprecision(5) << rank*2668026 << endl;
    cout << setprecision(5) << rank2*2668026 << endl;
    cout << setprecision(5) << (rank-rank2)*2668026 << endl;

  cout << "sketch data: " << sketch1.to_string(true,true) << endl;

    //printf("rank: %lld ,normalizado: %lf ", ,rank);

    // cout << "QUANTILE:" << endl;
    // for(int i=0;i<nfrac+1;i++){
    //   if(!incluirTruthValues) 
    //     printf("%d\t cuantil %lf\t kll %lf\t kllmerge %lf\t datasketches %lf\n",i,fractions[i], quantileElements[i], quantilekminElements[i], qt1[i]);
    //   else
    //     printf("%d\t cuantil %lf\t kll %lf\t kllmerge %lf\t datasketches %lf\t truth %lf\n",i,fractions[i], quantileElements[i], quantilekminElements[i], qt1[i], truthQuantile.at(i));
      
    // }

    // cout << endl << "RANK:" << endl;
    // for(int i=0;i<nfrac+1;i++){
    //   if(!incluirTruthValues) 
    //     printf("%d\t cuantil %lf\t kll %lf\t kllmerge %lf \t datasketches %lf\n",i,fractions[i], (double)rankElements[i]/(double)n, (double)rankkminElements[i]/(double)n, sketch11.get_rank(qt1[i]));
    //   else 
    //     printf("%d\t cuantil %lf\t kll %lf\t kllmerge %lf \t datasketches %lf\t truth %lf\n",i,fractions[i], (double)rankElements[i]/(double)n, (double)rankkminElements[i]/(double)n, sketch11.get_rank(qt1[i]), (double)truthRank.at(i)/(double)n);
    // }

    // cout << "KLL UNO:" << endl;
    // kllUno.print();
    // cout << endl << endl << "KLL Merged:" << endl;
    // kllFinal.print();
    // klleps.printNumSketches();
    // mrleps.printNumSketches();

    /*
    std::cout<<" i quantile(fractions) value_in_sortedstream_for_quantile\n";
    for(int i=0; i<nfrac+1; i++){
	   printf("%d\t(%lf)\t%lf \n",i,fractions[i],qt1[i]); 
    }

    std::cout<<" i quantile rank(fractions)\n";
    for(int i=0; i<nfrac+1; i++){
	   printf("%d\t(%lf)\t%lf \n",i,qt1[i],sketch1.get_rank(qt1[i])); 
    }

    cout << sketch1.get_serialized_size_bytes(is1) << endl;
    */

   
    /*
    std::vector<double> split_points;
    std::vector<double> points;
    split_points.push_back(0.0);
    points.push_back(0.0);
    int spt=0;
    std::cout<<" What element is in this quantile\n";
    std::cout<<" i value_in_quantile rank(number of elems less than value) \n";
    for(int i=0; i<nfrac; i++){
	   printf("%d %lf %lf\n",i,qt1[i],sketch1.get_rank(qt1[i])); 
    }
    for(int i=0; i<nfrac; i++){
	   //printf("%d %f %f\n",i,fractions[i],qt1[i]); 
	   if(split_points[spt] != qt1[i]){
		   split_points.push_back(qt1[i]);
		   points.push_back(qt1[i]);
		   spt++;
	   }
    }

     
    double *sp = split_points.data();
    //auto pmf = sketch1.get_PMF(sp, spt); 
    auto pmf = sketch1.get_CDF(sp, spt); 
    for(int i=0; i<pmf.size(); i++){
	   printf("%d %f %f\n",i,points[i],pmf[i]); 
    }

    */

    /*
    std::cout << "Min, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.98, Max values sketch 1" << std::endl;
    const double fractions1[13] {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.98, 0.99, 1};
    auto qt1 = sketch1.get_quantiles(fractions1, 13);
    std::cout << qt1[0] << ", " << qt1[1] << ", " << qt1[2] << ", "<< qt1[3]<<", "<<qt1[4]<<", "<<qt1[5]<<", "<<qt1[6]<<", "<<qt1[7]<<", "<<qt1[8]<<", "<<qt1[9]<<", "<<qt1[10]<<", "<<qt1[11]<<", "<< qt1[12]<<std::endl;
    float median = sketch1.get_quantile(0.5);
    //std::cout<<" median "<<median<<"\n";
    double rank1000 = sketch1.get_rank(1000);
    std::cout<<" median "<<median<<", rank 1000 "<<rank1000<<" rank median "<<sketch1.get_rank(median)<<" rank median chicago-2008 "<<sketch1.get_rank(2068452590)<<"\n";
    */
    /*
    std::cout << "Probability Histogram: estimated probability mass in 4 bins: (-inf, -2), [-2, 0), [0, 2), [2, +inf)" << std::endl;
    const float split_points[] {0, 1, 2, 3};
    const int num_split_points = 4;
    auto pmf = sketch1.get_PMF(split_points, num_split_points);
    std::cout << pmf[0] << ", " << pmf[1] << ", " << pmf[2] << ", " << pmf[3] <<", "<<pmf[4]<< std::endl;
    std::cout << "Frequency Histogram: estimated number of original values in the same bins" << std::endl;
    */
    /*
    const float split_points[] {10, 1015, 2030, 3028, 4041, 5007, 5994, 7001, 8033, 9012, 9808, 9917, 9994};
    std::cout<<" PMF \n";
    auto pmf = sketch1.get_PMF(split_points, 13);
    for (int i = 0; i < 13; i++) {
	    std::cout<<pmf[i]<<", ";
    }
    std::cout<<"\n";
    std::cout<<" CDF \n";
    auto cdf = sketch1.get_CDF(split_points, 13);
    for (int i = 0; i < 13; i++) {
	    std::cout<<cdf[i]<<", ";
    }
    std::cout<<"\n";
    //const int num_bins = num_split_points + 1;
    const int num_bins = 14;
    int histogram[num_bins];
    for (int i = 0; i < num_bins; i++) {
      histogram[i] = pmf[i] * sketch1.get_n(); // scale the fractions by the total count of values
      std::cout<<histogram[i]<<", ";
    }
    std::cout<<"\n";
    //std::cout << histogram[0] << ", " << histogram[1] << ", " << histogram[2] << ", " << histogram[3] << std::endl;

    */
  

  return 0;
}
