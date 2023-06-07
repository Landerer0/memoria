#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "kll.hpp"
#include "murmurhash.hpp"

void debugLetra(string string){
    cout << string << endl;
}

using namespace std;
KLL::KLL(unsigned long numElements, double epsilonParam, double deltaParam, double cParam){
    n = numElements;
    epsilon = epsilonParam;
    delta = deltaParam;
    c = cParam;

    H = 1.5*log(epsilon*n); // O(log(epsilon*n))
    k = 1.5*(double)(1/epsilon) * log(log((1/delta)));
    s = log(log((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - 2s - log(k)
    if(H_pp<0) H_pp = 0;
    wH_pp = pow(2,H_pp);

    mascara = pow(2,H_pp);
    sampleElement=0;
    sampleWeight=0;

    
    cout << "Con N = " << n << " Epsilon = " << epsilon << " Delta = " 
        << delta << " y 'c' = " << c << " se obtienen los valores:" << endl;
    cout << "Cantidad de elementos que pasaron la etapa de muestreo (aprox): " << n/(pow(2,H_pp)) << endl;
    cout << "H: " << H << endl 
        << "s: " << s << endl
        << "k: " << k << endl
        << "H': " << H_p << endl
        << "H'': " << H_pp << endl
        << "Num Arreglos: " << (H - H_pp) << endl
        << "w_H'': " << wH_pp << endl;
    
    cout << endl;
    

    numArreglos = (H - H_pp);
    numElementosRevisados = 0;
    numTotalElementos = 0;
    numArreglosOcupados = H_pp;
    unsigned long long espacioOcupado = 0;

    vector<long> sizeTemp;
    for(int i=H_pp;i<H;i++){
        unsigned long long cantElementos;
        if(i>(H-s-1)) cantElementos = k;
        else cantElementos = max((int)(k*pow(c,H-i-1)),(int)2);
        //cout << "cantElementos en arreglo " << i+1 << ": " << cantElementos << endl;
        sizeTemp.push_back(cantElementos);
    }

    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<sizeTemp.size();i++){
        // el valor por defecto es -1, que indica "vacio"
        unsigned long long cantElementos = sizeTemp.at(i);
        espacioOcupado += cantElementos;
        //cerr << "Cantidad Elementos arreglo " << i << " (nivel " << i+H_pp+1 <<") :" << cantElementos<< endl;
        double valorElemento = -2;
        vector<double> vectorAtLvlI(cantElementos,valorElemento); 
        pair<vector<double>,long> toInsert;
        toInsert.first=vectorAtLvlI;
        toInsert.second=0; // representa el num de elementos ocupados en el arreglo
        sketch.push_back(toInsert);
        //sorted.push_back(false); // indicamos que no se encuentra sorteado el arreglo
    }

    //cout << "Cant elementos en Sketch: " << espacioOcupado << " Espacio ocupado: " << espacioOcupado*sizeof(long) << " bytes" <<  endl;
    
    //print();
    //cout << "fin inicialización" << endl;
}

KLL::~KLL(){
    
}

void KLL::insertElement(long nivel,double &element){
    //cout << "insert " << element << endl;
    long posAInsertar = sketch.at(nivel).second;
    //cout << "pos a insert: " << posAInsertar << endl;
    sketch.at(nivel).first.at(posAInsertar) = element;
    //debugLetra("a");
    sketch.at(nivel).second++;
    //debugLetra("b");
    //sorted.at(nivel)=false;
    //debugLetra("c");
}

void KLL::insertCompactionElement(long nivel,double &element, bool updating){
    
    // if(updating) cout << "inicio insert " << element << endl;

    long posAInsertar = sketch.at(nivel).second;
    if(posAInsertar==sketch.at(nivel).first.size()){
        if(nivel==numArreglos && updating){
            //Agregar nuevo compactor de tam k
            long numElementosInsert = 2;
            vector<double> toInsert(numElementosInsert,-2);
            sketch.push_back(make_pair(toInsert,0));
            numArreglos++;
        }
        compaction(nivel,updating);
        posAInsertar = sketch.at(nivel).second;
    }
    
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;

    // AVANCE2
    // if(updating){
    //     cout << endl << "Insert element: " << element << endl;
    //     print();
    // }
}

void KLL::compaction(long nivel, bool updating){
    //cout << "Compaction: " << nivel <<  endl;
    //print();

    long numElementosOcupados = sketch.at(nivel).second;
    long numElementosTotales = sketch.at(nivel).first.size();
    unsigned char elementosPares = 0;
    if(numElementosOcupados==numElementosTotales){
        //if(nivel==7){
        //    cout << numElementosRevisados << " de " << numTotalElementos <<  endl;
        //    print();
        //} 
        if(nivel+1==numArreglos) return;
        if(debug) cerr << endl << "compaction " << nivel+1 << endl;
        if(rand()%2==0) elementosPares = 0; // se mantienen los elementos pares
        else elementosPares = 1; // se mantienen los elementos impares

        // sort del arreglo
        sort(sketch.at(nivel).first.begin(), sketch.at(nivel).first.end());
        
        if(debug) print();
        
        sketch.at(nivel).second = 0;
        // insertar la mitad de elementos (los pares o impares) en el sgte. nivel
        for(int i=elementosPares;i<numElementosTotales;i+=2){
            insertCompactionElement(nivel+1,sketch.at(nivel).first.at(i),updating);
        }

        long nullElement = -1;
        for(int i=0;i<numElementosTotales;i++){
            sketch.at(nivel).first.at(i) = nullElement;
        }

        if(debug) cout << "Fin de compaction " << nivel+1 << endl;
        if(debug) print();

        //nivel++;
        //numElementosOcupados = sketch.at(nivel).second;
        //numElementosTotales = sketch.at(nivel).first.size();
    }
}

bool KLL::sample(long element){
    if(rand()%wH_pp==0) return true;
    return false;
}

bool KLL::murmurHashSample(long element){
    // se hace hash, en caso de que sea seleccionado, se indica que se prosiga
    long hashVal = (murmur64(element)); 
    if(hashVal%wH_pp==0) return true;
    return false;
}

bool KLL::reservoirKLLSample(long element, long elementWeight){
    sampleWeight = elementWeight + sampleWeight;

    if(sampleWeight<=wH_pp){
        double probReemplazo = (double) elementWeight/(double) sampleWeight;
        if((double) (rand()/RAND_MAX) <= probReemplazo) sampleElement = element;
    }
    if(sampleWeight==wH_pp){
        sampleWeight = 0;
        return true;
    }
    
    // caso merge, revisar
    else if (sampleWeight>wH_pp){

    }

    return false;
}

void KLL::add(double element){
    numTotalElementos++;
    //if(!sample(element)) return;
    //if(!murmurHashSample(element)) return;
    //insertElement(0,element);
    
    if(!reservoirKLLSample(element,1)) return;
    insertElement(0,sampleElement);
    //cout << element << endl;
    numElementosRevisados++; // para metodo quantile
    compaction((long) 0, false);

    return;
}

void KLL::addv(double element){
    numElementosRevisados++; // para metodo quantile
    insertElement(0,element);
    compaction((long) 0, false);

    return;
}

unsigned long KLL::rank(double element){
    unsigned long rank = 0;

    vector<double> actual;

    for(int nivel=0;nivel< numArreglos;nivel++){ // por cada arreglo
        actual = sketch.at(nivel).first;
        //if(!sorted.at(nivel)){
            sort(actual.begin(),actual.end());
	    sketch.at(nivel).first = actual;
         //   sorted.at(nivel) = true;
        //} 
        for(int i=0;i<actual.size();i++){ // por cada item dentro del arreglo
            if(actual.at(i) < 0) continue;
            if(element >= actual.at(i)){ // comparo el num elementos menores
                rank += pow(2,nivel); // en caso de que existan menores sumar acordemente segun el nivel
            }
        }    
    }

    return pow(2,H_pp)*rank;
}

long KLL::select(long rank){
    vector<pair<long,unsigned long long>> elementos; // par(elemento,peso) 
    
    // llenar el vector con los elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        unsigned long long peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	    if(sketch.at(i).first.at(j) < 0)continue;
            pair<long, unsigned long long> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            elementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(elementos.begin(),elementos.end());

    long rankActual = 0;
    //retornar segun la suma de elementos
    for(int i=0;i<elementos.size();i++){
	if(elementos.at(i).first < 0)continue;
        rankActual+=elementos.at(i).second;
        if(rank<=rankActual) return elementos.at(i).first;
    }

    return elementos.at(elementos.size()-1).first;
}

long KLL::quantile(double q){
    q = q/100.0;
    //cout<<" q "<<q<<" numElementosRevisados "<<numElementosRevisados<<"\n";
    return select(floor(q*numElementosRevisados));
}

long KLL::height(){
    return numArreglos;
}

void KLL::print(){
    cout << "H: " << H << ", s: " << s << ", H'': " << H_pp << endl;
    cout << "numElementosRevisados: " << numElementosRevisados << " numTotal: " << numTotalElementos << endl;
    for(int i=0; i<sketch.size();i++){
        cout << "Nivel " << i+1 << ":" << endl;
        vector<double> nivelI = sketch.at(i).first;
        for(int j=0;j<nivelI.size();j++){
            cout << nivelI.at(j) << " ";
        }
        cout << endl;
    }
}

pair<vector<double>, long> KLL::sketchAtLevel(long nivel){
    if(nivel<0||nivel>=numArreglos) return make_pair(vector<double>(),-1);
    return sketch.at(nivel);
}

bool KLL::sortedAtLevel(long nivel){
    if(nivel<0||nivel>=numArreglos) return false;
    return sorted.at(nivel);
}

void KLL::update(KLL kll2){
    
    // kll2.setSeconds(vector<long>());
    // para cada nivel
    for(int nivel=0; nivel<kll2.height();nivel++){
        pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
        // cout << "nivel " << nivel << endl;
        // setSeconds(vector<long>());
        // cerr << "Elementos1 en nivel " << nivel << ": " << sketch.at(nivel).second << endl;
        // cerr << "Elementos2 en nivel " << nivel << ": " << kll2pair.second << endl;
        // para cada elemento dentro del compactor
        for(int i=0; i < kll2pair.second;i++){
            if(kll2pair.first.at(i)<0) continue;
            insertCompactionElement(nivel,kll2pair.first.at(i),true);
        }
    }

    return;
}


void KLL::setSeconds(vector<long> seconds){
    for(int i=0;i<numArreglos;i++){
        //sketch.at(i).second = seconds.at(i);
        cerr << sketch.at(i).second << endl;
    }
    
    return;
}

KLL KLL::kllMerge(KLL &kll2){ 
    // Creamos una copia del kll a devolver
    if(height() < kll2.height()){
        KLL kllCopy2 = kll2.copy();
        kllCopy2.update(*this);
        return kllCopy2;
    } 
    else{
        KLL kllCopy1 = copy();
        kllCopy1.update(kll2);
        return kllCopy1;
    } 

    
}



KLL::KLL(unsigned long numElements, double epsilonParam, double numC, unsigned long long elementosRevisados, KLL* toCopy){
// usar propiedad de log: log_b(a) = log(a)/log(b) 
    n = numElements;
    c = numC;
    epsilon = epsilonParam;
    numElementosRevisados = elementosRevisados;

    k = ceil(1.0/epsilon* ceil(log2(epsilon*n))) +1;
    if(k%2==1) k+=1; // k sera par

    cerr << "Num Elementos: " << numElements << endl;
    cerr << "K: " << k << endl;
    cerr << "c: " << c << endl;
    cerr << endl;
    numArreglos = ceil(log2(n/k*2));

    // copiar los valores del kll a copiar
    for(int i=0;i<numArreglos;i++){
        sketch.push_back(toCopy->sketchAtLevel(i));
        //sorted.push_back(toCopy->sortedAtLevel(i));
    }
}

KLL KLL::copy(){
    KLL copia(n,epsilon,c,numElementosRevisados,this);
    return copia;
}

long KLL::sizeInBytes(){
    unsigned long totalSize = 0;
    unsigned long sketchSize = 0;

    //calculo del tamaño de sketchSize
    for(int i = 0; i< sketch.size();i++){
        sketchSize+=sizeof(sketch.at(i).second); // entero que me indica numero de valores ocupados en el nivel i
        sketchSize+= (sketch.at(i).first.size()*sizeof(double));
    }

    //calculo de totalSize según las variables utilizadas
    totalSize+=sketchSize;
    totalSize+=sizeof(hashLong);
    totalSize+=sizeof(sampleWeight);
    totalSize+=sizeof(sampleElement);
    totalSize+=sizeof(numArreglos);
    totalSize+=sizeof(numArreglosOcupados);
    totalSize+=sizeof(bool)*sorted.size();
    totalSize+=sizeof(n);
    totalSize+=sizeof(numElementosRevisados);
    totalSize+=sizeof(numTotalElementos);
    totalSize+=sizeof(epsilon);
    totalSize+=sizeof(delta);
    totalSize+=sizeof(c);
    totalSize+=sizeof(H);
    totalSize+=sizeof(H_p);
    totalSize+=sizeof(H_pp);
    totalSize+=sizeof(k);
    totalSize+=sizeof(s);
    totalSize+=sizeof(mascara);
    totalSize+=sizeof(wH_pp);
    totalSize+=sizeof(debug);

    // imprimir los resultados
    cout << "Parametros:" << endl;
    cout << "N: " << n << ", epsilon: " << epsilon << ", delta: " << delta << ", c: " << c << endl;
    cout << "H: " << H << ", H': " << H_p << ", H''" << H_pp << ", k_H:" << k << ", s: " << s << endl;
    cout << "Tamaño del arreglo: " << sketchSize << ", Espacio Total KLL: " << totalSize << endl;
    return totalSize;
}

vector<double> KLL::parametros(){
    vector<double> parametros;
    parametros.push_back(n);
    parametros.push_back(epsilon);
    parametros.push_back(delta);
    parametros.push_back(c);
    parametros.push_back(H);
    parametros.push_back(H_p);
    parametros.push_back(H_pp);
    parametros.push_back(s);
    parametros.push_back(k);
    parametros.push_back(sizeInBytes());
    return parametros;
}