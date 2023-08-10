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
KLL::KLL(uint64_t numElements, double epsilonParam, double deltaParam, double cParam, int minKp){

    isMrl = false;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    minK = minKp;
    n = numElements;
    epsilon = epsilonParam;
    delta = deltaParam;
    c = cParam;

    H = 1.5*log(epsilon*n); // O(log(epsilon*n))
    k = 1.5*(double)(1/epsilon) * log(log((1/delta)));
    s = log(log((1/delta)));

    
    //H = 1.5*log2(epsilon*n); // O(log(epsilon*n))
    //k = 1.5*(double)(1/epsilon) * log2(log2((1/delta)));
    //s = log2(log2((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - log(k)
    if(H_pp<0) H_pp = 0;
    wH_pp = pow(2,H_pp);

    mascara = pow(2,H_pp);
    sampleElement=0;
    sampleWeight=0;

    numArreglos = (H - H_pp+1);
    numElementosRevisados = 0;
    numTotalElementos = 0;
    unsigned long long espacioOcupado = 0;

    vector<long> sizeTemp;
    for(int i=H_pp;i<=H;i++){
        unsigned long long cantElementos;
        if(i>(H-s)) cantElementos = max(k,(long)minK);
        else cantElementos = max((int)(k*pow(c,H-i-1)),(int)minK);
        cout << i << " El: " << cantElementos << endl;
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
    }

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
    
    for(int i=0;i<sketch.size();i++){
        cout << "Compactor en nivel " << H_pp+i << " tiene capacidad: " << sketch.at(i).first.size() << endl;
    }

    cout << endl;

    //cout << "Cant elementos en Sketch: " << espacioOcupado << " Espacio ocupado: " << espacioOcupado*sizeof(long) << " bytes" <<  endl;
    
    //print();
    //cout << "fin inicialización" << endl;
}

KLL::KLL(uint64_t minKP){
    isMrl = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;
    c=1;
    
    wH_pp = pow(2,0);

    mascara = pow(2,0);
    sampleElement=0;
    sampleWeight=0;

    numArreglos = 1;
    numElementosRevisados = 0;
    numTotalElementos = 0;
    minK = minKP;
    k = minK;
    unsigned long long espacioOcupado = 0;

    vector<long> sizeTemp;
        unsigned long long cantElementos;
        cantElementos = minK;
        //cout << "cantElementos en arreglo " << i+1 << ": " << cantElementos << endl;
        sizeTemp.push_back(cantElementos);
    

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
    }

    H = 0; 
    H_p = 0;
    H_pp = 0;
    //cout << "PRINTTTTTTTTTTTTTTTTTTT" << endl;
    //print();
}

KLL::KLL(uint64_t espacioMaximo, uint64_t numElementosParam, double cParam, int minKp, uint32_t maxKp){
    cout << "Espacio Maximo: " << espacioMaximo << " NumElementosAAbarcar: " << numElementosParam << endl;

    isMrl = false;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    minK = minKp;
    //n = numElements;
    //epsilon = epsilonParam;
    //delta = deltaParam;
    c = cParam;

    // cambiar a log2 (base2)
    H = 1.5*log(epsilon*n); // O(log(epsilon*n))
    k = 1.5*(double)(1/epsilon) * log(log((1/delta)));
    s = log(log((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - log(k)
    if(H_pp<0) H_pp = 0;
    wH_pp = pow(2,H_pp); // HACER SHIFTING A LA DERECHA

    mascara = pow(2,H_pp);
    sampleElement=0;
    sampleWeight=0;

    numArreglos = (H - H_pp+1);
    numElementosRevisados = 0;
    numTotalElementos = 0;
    unsigned long long espacioOcupado = 0;

    vector<long> sizeTemp;
    for(int i=H_pp;i<=H;i++){
        unsigned long long cantElementos;
        if(i>(H-s)) cantElementos = max(k,(long)minK);
        else cantElementos = max((int)(k*pow(c,H-i-1)),(int)minK);
        cout << i << " El: " << cantElementos << endl;
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
    }

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
    
    for(int i=0;i<sketch.size();i++){
        cout << "Compactor en nivel " << H_pp+i << " tiene capacidad: " << sketch.at(i).first.size() << endl;
    }

    cout << endl;
}

KLL::KLL(uint64_t espacioMaximo, uint64_t numElementosParam){
    cout << "Espacio Maximo: " << espacioMaximo << " NumElementosAAbarcar: " << numElementosParam << endl;
    isMrl = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;
    c=1;
    
    wH_pp = pow(2,0);

    mascara = pow(2,0);
    sampleElement=0;
    sampleWeight=0;

    numArreglos = 1;
    numElementosRevisados = 0;
    numTotalElementos = 0;

    uint64_t numNivelesNecesarios = -1;
    uint64_t minKNecesario = 0;
    uint64_t numElementosP = numElementosParam;
    uint64_t espacioOcupado = espacioMaximo;
    uint64_t cantElementosPorNivel;
    bool parametrosEncontrados = false;

    espacioOcupado-=sizeInBytes();
    uint64_t espacioIteracion = 0;
    uint64_t espacioPorNivel;
    while(!parametrosEncontrados){
        numNivelesNecesarios++;

        uint64_t cantElementosIteracion;
        cantElementosIteracion = espacioOcupado-sizeof(long)*(numNivelesNecesarios+1);
        cantElementosIteracion /= ((numNivelesNecesarios+1) * sizeof(double));

        if(cantElementosIteracion < 2){
            cerr << "ERROR EN LA CREACION DEL MRL" << endl;
            return;
        }

        espacioPorNivel = (cantElementosIteracion*sizeof(double))+sizeof(long);
        uint64_t numElementosAbarcados = 0;

        espacioIteracion=0;
        for(int i = 0; i<= numNivelesNecesarios;i++){
            espacioIteracion+=espacioPorNivel;
        }
        numElementosAbarcados = cantElementosIteracion*pow(2,numNivelesNecesarios)-1;
        cerr << "NumNiveles: " << numNivelesNecesarios+1 << " NumElementosAbarcados: " << numElementosAbarcados << "/" << numElementosParam<<endl;
        cerr << "\t Espacio ocupado: " << espacioIteracion  << "/" << espacioOcupado << " Cantidad de elementos por nivel: " << cantElementosIteracion << " Espacio por nivel: " << espacioPorNivel << " ... " << espacioPorNivel*(numNivelesNecesarios+1) << endl;

        cantElementosPorNivel = cantElementosIteracion;
        if(numElementosAbarcados>=numElementosParam){
            parametrosEncontrados = true;
        }
    }

    cerr << "Num niveles: " << numNivelesNecesarios+1 << endl;
    cerr << "cant de elementos por nivel: " << cantElementosPorNivel << endl;

    minK = cantElementosPorNivel;
    k = minK;

    vector<long> sizeTemp;
        unsigned long long cantElementos;
        cantElementos = minK;
        cout << "cantElementos en arreglo : " << cantElementos << endl;
        sizeTemp.push_back(cantElementos);
    

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
    }

    H = 0; 
    H_p = 0;
    H_pp = 0;
    //cout << "PRINTTTTTTTTTTTTTTTTTTT" << endl;
    //print();

    sizeInBytes();
}

KLL::KLL(double epsilonEntregado ,uint64_t numElementosParam){
    double cotaEspacio = 1.0/epsilonEntregado * log2(1.0/epsilonEntregado) * log2(1.0/epsilonEntregado);
    uint64_t espacioMaximo = ceil(cotaEspacio);
    cout << "Epsilon Entregado: " << epsilonEntregado << " Espacio Maximo: " << espacioMaximo << " NumElementosAAbarcar: " << numElementosParam << endl;
    //desde aqui es el mismo constructor que  KLL(uint64_t espacioMaximo, uint64_t numElementosParam)
    isMrl = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;
    c=1;
    
    wH_pp = pow(2,0);

    mascara = pow(2,0);
    sampleElement=0;
    sampleWeight=0;

    numArreglos = 1;
    numElementosRevisados = 0;
    numTotalElementos = 0;

    uint64_t numNivelesNecesarios = -1;
    uint64_t minKNecesario = 0;
    uint64_t numElementosP = numElementosParam;
    uint64_t espacioOcupado = espacioMaximo;
    uint64_t cantElementosPorNivel;
    bool parametrosEncontrados = false;

    espacioOcupado-=sizeInBytes();
    uint64_t espacioIteracion = 0;
    uint64_t espacioPorNivel;
    while(!parametrosEncontrados){
        numNivelesNecesarios++;

        uint64_t cantElementosIteracion;
        cantElementosIteracion = espacioOcupado-sizeof(long)*(numNivelesNecesarios+1);
        cantElementosIteracion /= ((numNivelesNecesarios+1) * sizeof(double));

        if(cantElementosIteracion < 2){
            cerr << "ERROR EN LA CREACION DEL MRL" << endl;
            return;
        }

        espacioPorNivel = (cantElementosIteracion*sizeof(double))+sizeof(long);
        uint64_t numElementosAbarcados = 0;

        espacioIteracion=0;
        for(int i = 0; i<= numNivelesNecesarios;i++){
            espacioIteracion+=espacioPorNivel;
        }
        numElementosAbarcados = cantElementosIteracion*pow(2,numNivelesNecesarios)-1;
        cerr << "NumNiveles: " << numNivelesNecesarios+1 << " NumElementosAbarcados: " << numElementosAbarcados << "/" << numElementosParam<<endl;
        cerr << "\t Espacio ocupado: " << espacioIteracion  << "/" << espacioOcupado << " Cantidad de elementos por nivel: " << cantElementosIteracion << " Espacio por nivel: " << espacioPorNivel << " ... " << espacioPorNivel*(numNivelesNecesarios+1) << endl;

        cantElementosPorNivel = cantElementosIteracion;
        if(numElementosAbarcados>=numElementosParam){
            parametrosEncontrados = true;
        }
    }

    cerr << "Num niveles: " << numNivelesNecesarios+1 << endl;
    cerr << "cant de elementos por nivel: " << cantElementosPorNivel << endl;

    minK = cantElementosPorNivel;
    k = minK;

    vector<long> sizeTemp;
        unsigned long long cantElementos;
        cantElementos = minK;
        cout << "cantElementos en arreglo : " << cantElementos << endl;
        sizeTemp.push_back(cantElementos);
    

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
    }

    H = 0; 
    H_p = 0;
    H_pp = 0;
    //cout << "PRINTTTTTTTTTTTTTTTTTTT" << endl;
    //print();

    sizeInBytes();
}

KLL::~KLL(){
    
}

void KLL::insertElement(long nivel,double &element){
    //cout << "insert " << element << endl;
    long posAInsertar = sketch.at(nivel).second;
    //cout << "pos a insert: " << posAInsertar << endl;
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;
}

void KLL::insertCompactionElement(long nivel,double &element, bool updating){
    
    // if(updating) cout << "inicio insert " << element << endl;
    long posAInsertar = sketch.at(nivel).second;
    if(nivel+1==numArreglos && posAInsertar == sketch.at(nivel).first.size()) cerr << "error" << endl;
    
    sketch.at(nivel).first.at(posAInsertar) = element;
    sketch.at(nivel).second++;
    
    if(posAInsertar+1==sketch.at(nivel).first.size()){
        if(nivel+1==numArreglos && updating){
            //Agregar nuevo compactor de tamaño k
            long numElementosInsert = isMrl? minK: sketch.at(nivel).first.size();
            vector<double> toInsert(numElementosInsert,-2);
            sketch.push_back(make_pair(toInsert,0));
            numArreglos++;
            H++;
        }
        compaction(nivel,updating);
        posAInsertar = sketch.at(nivel).second;
    }
}

bool KLL::compaction(long nivel, bool updating){
    //cout << "Compaction: " << nivel <<  endl;
    //print();

    long numElementosOcupados = sketch.at(nivel).second;
    long numElementosTotales = sketch.at(nivel).first.size();
    unsigned char elementosPares = 0;
    if(numElementosOcupados>=numElementosTotales){
        if(nivel+1==numArreglos) return true;
        /* Caso que se quiera agregar más elementos al sketch
        if(nivel+1==numArreglos) {
            vector<double> vectorAtLvlI(sketch.at(nivel).first.size(),-2); 
            pair<vector<double>,long> toInsert;
            toInsert.first=vectorAtLvlI;
            toInsert.second=0; // representa el num de elementos ocupados en el arreglo
            sketch.push_back(toInsert);
            cerr << "!!! " << sketch.size()-1 << " ... numElementos:" << numTotalElementos << endl;
            numArreglos++;
            H++;
        };
        */
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

        return false;
        //nivel++;
        //numElementosOcupados = sketch.at(nivel).second;
        //numElementosTotales = sketch.at(nivel).first.size();
    }
}

bool KLL::sample(double element){
    if(rand()%wH_pp==0) return true;
    return false;
}

bool KLL::murmurHashSample(double element){
    // se hace hash, en caso de que sea seleccionado, se indica que se prosiga
    long hashVal = (murmur64(element)); 
    if(hashVal%wH_pp==0) return true;
    return false;
}

bool KLL::reservoirKLLSample(double element, uint64_t elementWeight){
    //sampleElement=element;
    //return true;

    uint64_t currentWeight = sampleElement;
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
        double heavyElement;
        // sampler discards the heavier item and keeps the lighter item with weight min{w,v}
        if(elementWeight<currentWeight){
            heavyElement = sampleElement;
            sampleWeight = elementWeight;
            sampleElement = element;
        } else {
            heavyElement = element;
            sampleWeight = currentWeight;
        }
        //with probability max{w,v}/2^h it also outputs the heavier item with weight 2^h
        double probIngresoElementoPesado = (double) max(currentWeight,elementWeight)/(double) sampleWeight;
        if((double) (rand()/RAND_MAX) <= probIngresoElementoPesado) addv(heavyElement);
        
    }

    return false;
}

bool KLL::add(double element){
    numTotalElementos++;
    //if(!sample(element)) return;
    //if(!murmurHashSample(element)) return;
    //insertElement(0,element);
    
    if(element<minElement) minElement = element;
    else if (element>maxElement) maxElement = element;

    if(!reservoirKLLSample(element,1)) return;
    insertElement(0,sampleElement);
    //cout << element << endl;
    numElementosRevisados++; // para metodo quantile
    return compaction((long) 0, false);

    //return;
}

void KLL::add(double element, uint32_t elementWeight){
    numTotalElementos+=elementWeight;
    //if(!sample(element)) return;
    //if(!murmurHashSample(element)) return;
    //insertElement(0,element);
    
    if(element<minElement) minElement = element;
    else if (element>maxElement) maxElement = element;

    if(!reservoirKLLSample(element,elementWeight)) return;
    insertElement(0,sampleElement);
    //cout << element << endl;
    numElementosRevisados+=elementWeight; // para metodo quantile
    compaction((long) 0, false);

    return;
}

// add utilizado cuando se hacer merge de KLL
void KLL::addv(double element){
    insertElement(0,element);
    compaction((long) 0, false);

    return;
}

uint64_t KLL::rank(double element){
    uint64_t rank = 0;

    vector<double> actual;

    for(int nivel=0;nivel< numArreglos;nivel++){ // por cada arreglo
        actual = sketch.at(nivel).first;

        sort(actual.begin(),actual.end());
	    //sketch.at(nivel).first = actual;
        for(int i=0;i<actual.size();i++){ // por cada item dentro del arreglo
            if(actual.at(i) < 0) continue;
            if(element >= actual.at(i)){ // comparo el num elementos menores
                rank += pow(2,nivel); // en caso de que existan menores sumar acordemente segun el nivel
            }
        }    
    }

    return pow(2,H_pp)*rank;
}

vector<uint64_t> KLL::rank(vector<double> elements){
    // Procedimiento similar a select.
    // 1. Se agregan todos los elementos de manera (Elemento, frecuencia)
    // 2. Se realiza un sort en el vector donde se almacenan
    // 3. Se itera para responder el rank de cada elemento
    // 4. Recordar que el rank se multiplica por 2^H_pp

    //for(int i=0;i<elements.size();i++){
    //    cerr << "Element " << i << ": " << elements[i] << endl;
    //}

    vector<uint64_t> ranks;

    vector<double> actual;

    vector<pair<double,uint64_t>> vectorElementos; // par(elemento,peso) 
    // llenar el vector con los Elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	    if(sketch.at(i).first.at(j) < 0)continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            vectorElementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(vectorElementos.begin(),vectorElementos.end());

    uint64_t actualPair = 0;
    uint64_t actualRank = 0;
    uint64_t vectorElementosSize = vectorElementos.size();

    // una vez tenemos el vector con los pares ordenados, podemos obtener el rank de los elementos
    for(int i=0;i<elements.size();i++){
        //cerr << i << " " << actualPair << endl;
        while(actualPair < vectorElementosSize && elements.at(i) >= vectorElementos.at(actualPair).first){ // comparo el num Elementos menores
            actualRank += vectorElementos.at(actualPair).second; // en caso de que existan menores sumar acordemente segun el nivel
            actualPair++;
        }
        ranks.push_back(pow(2,H_pp)*actualRank);
    }

    if(ranks.size()!=elements.size()){
        ranks.push_back(pow(2,H_pp)*actualRank);
    }

    return ranks;
}

double KLL::select(uint64_t rank){

    vector<pair<double,uint64_t>> elementos; // par(elemento,peso) 
    
    cout << "Num arreglos: " << numArreglos << endl;
    // llenar el vector con los elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        cout << i << endl;
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	        if(sketch.at(i).first.at(j) < 0) continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            elementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(elementos.begin(),elementos.end());

    uint64_t rankActual = 0;
    //retornar segun la suma de elementos
    for(int i=0;i<elementos.size();i++){
	if(elementos.at(i).first < 0) continue;
        rankActual+=elementos.at(i).second;
        if(rank<=rankActual) return elementos.at(i).first;
    }

    /*
    if(rank <= elementos.at(0).second/2){
        cerr << "min element, rank consultando:" << rank << ", rank primer elemento:" << elementos.at(0).second << endl;
        return minElement;
    } 
    else if (rankActual-rank <= elementos.at(elementos.size()-1).second/2){
        cerr << "max element, rank consultando:" << rank << ", rank ultimo elemento:" << elementos.at(elementos.size()-1).second << endl;
        return maxElement;
    } 
    */

    return elementos.at(elementos.size()-1).first;
}

pair<double, bool> KLL::selectMinMax(uint64_t rank){

    vector<pair<double,uint64_t>> elementos; // par(elemento,peso) 
    bool isMinMax = false;
    double toReturn;
    
    cout << "Num arreglos: " << numArreglos << endl;
    // llenar el vector con los elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        cout << i << endl;
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	        if(sketch.at(i).first.at(j) < 0) continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            elementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(elementos.begin(),elementos.end());

    uint64_t rankActual = 0;
    //retornar segun la suma de elementos
    for(int i=0;i<elementos.size();i++){
	    if(elementos.at(i).first < 0) continue;
        rankActual+=elementos.at(i).second;
        if(rank<=rankActual) toReturn = elementos.at(i).first;
    }

    if(rank <= elementos.at(0).second/2){
        cerr << "min element, rank consultando:" << rank << ", rank primer elemento:" << elementos.at(0).second << endl;
        toReturn = minElement;
        isMinMax = true;
    } 
    else if (rankActual-rank <= elementos.at(elementos.size()-1).second/2){
        cerr << "max element, rank consultando:" << rank << ", rank ultimo elemento:" << elementos.at(elementos.size()-1).second << endl;
        toReturn = maxElement;
        isMinMax = true;
    } 
    
    return pair<double, bool>(toReturn, isMinMax);
}

vector<double> KLL::select(vector<uint64_t> ranks){
    vector<double> selected;

    cerr << "Ranks consultados: " << ranks.size() << endl;
    for(int i=0;i<ranks.size();i++){
        cerr << ranks[i] << " ";
    }
    cerr << endl << endl;

    vector<pair<double,uint64_t>> vectorElementos; // par(elemento,peso) 
    
    // llenar el vector con los Elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	        if(sketch.at(i).first.at(j) < 0) continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            vectorElementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(vectorElementos.begin(),vectorElementos.end());

    //for(int i=0;i<vectorElementos.size();i++){
    //    cerr << "("<< vectorElementos[i].first << ";" << vectorElementos[i].second << ")" << endl;
    //}

    uint64_t actualPair = 0;
    uint64_t actualRank = 0;
    double actualElement = vectorElementos.at(0).first;

    uint64_t vectorElementosSize = vectorElementos.size();
    //!Revisar
    for(int i=0;i<ranks.size();i++){
        //cerr << "i: " <<  i << endl;
        while(actualRank < ranks.at(i) && actualPair < vectorElementosSize){ // comparo el num Elementos menores
            actualRank += vectorElementos.at(actualPair).second; // en caso de que existan menores sumar acordemente segun el nivel
            actualElement = vectorElementos.at(actualPair).first;
            actualPair++;
        }
        selected.push_back(actualElement);
        continue;
        
    }

    while(ranks.size()!=selected.size()){
        ranks.push_back(pow(2,H_pp)*actualRank);
    }
    
    /*
    for(int i=0;i<ranks.size();i++){
        if(ranks.at(i)>vectorElementos.at(0).second/2) break;
        selected.at(i) = minElement;
    }

    for(int i=ranks.size()-1;i>=0;i--){
        cerr << "fin" << endl;
        cerr << "ranks size " << ranks.size() << " selected size " << selected.size() << " vectorElementosSize " << vectorElementosSize << " vector.size " << vectorElementos.size() << endl;
        if(actualRank-ranks.at(i)>vectorElementos.at(vectorElementosSize-1).second/2) break;
        selected.at(i) = maxElement;
    }
    */

    return selected;
}

vector<pair<double,bool>> KLL::selectMinMax(vector<uint64_t> ranks){
    vector<pair<double,bool>> selected;

    cerr << "Ranks consultados: " << ranks.size() << endl;
    for(int i=0;i<ranks.size();i++){
        cerr << ranks[i] << " ";
    }
    cerr << endl << endl;

    vector<pair<double,uint64_t>> vectorElementos; // par(elemento,peso) 
    
    // llenar el vector con los Elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	        if(sketch.at(i).first.at(j) < 0) continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            vectorElementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(vectorElementos.begin(),vectorElementos.end());

    //for(int i=0;i<vectorElementos.size();i++){
    //    cerr << "("<< vectorElementos[i].first << ";" << vectorElementos[i].second << ")" << endl;
    //}

    uint64_t actualPair = 0;
    uint64_t actualRank = 0;
    double actualElement = vectorElementos.at(0).first;

    uint64_t vectorElementosSize = vectorElementos.size();
    //!Revisar
    for(int i=0;i<ranks.size();i++){
        //cerr << "i: " <<  i << endl;
        while(actualRank < ranks.at(i) && actualPair < vectorElementosSize){ // comparo el num Elementos menores
            actualRank += vectorElementos.at(actualPair).second; // en caso de que existan menores sumar acordemente segun el nivel
            actualElement = vectorElementos.at(actualPair).first;
            actualPair++;
        }
        selected.push_back(pair<double,bool>(actualElement,false));
        continue;
        
    }

    while(ranks.size()!=selected.size()){
        ranks.push_back(pow(2,H_pp)*actualRank);
    }
    
    for(int i=0;i<ranks.size();i++){
        if(ranks.at(i)>vectorElementos.at(0).second/2) break;
        selected.at(i).first = minElement;
        selected.at(i).second = true;
    }

    for(int i=ranks.size()-1;i>=0;i--){
        cerr << "fin" << endl;
        cerr << "ranks size " << ranks.size() << " selected size " << selected.size() << " vectorElementosSize " << vectorElementosSize << " vector.size " << vectorElementos.size() << endl;
        if(actualRank-ranks.at(i)>vectorElementos.at(vectorElementosSize-1).second/2) break;
        selected.at(i).first = maxElement;
        selected.at(i).second = true;
    }

    return selected;
}

double KLL::quantile(double q){
    // q pertenece (0,100]
    q = q/100.0;
    //cout<<" q "<<q<<" numElementosRevisados "<<numElementosRevisados<<"\n";
    return select(floor(q*numElementosRevisados));
}

vector<double> KLL::quantile(vector<double> q){
    // q pertenece (0,100]
    vector<uint64_t> ranks; 
    //cerr <<  "Quantile vector size: " << q.size() << endl;
    for(int i=0;i<q.size();i++)
        ranks.push_back(floor(q[i]/100.0 * numElementosRevisados));
    //cerr <<  "Rank vector size: " << ranks.size() << endl;
    return select(ranks);
}

long KLL::height(){
    return numArreglos;
}

void KLL::print(){
    cout << "H: " << H << ", s: " << s << ", H'': " << H_pp << endl;
    cout << "numElementosRevisados: " << numElementosRevisados << " numTotal: " << numTotalElementos << endl;
    for(int i=0; i<sketch.size();i++){
        cout << "Nivel " << i+H_pp << ": (" << sketch.at(i).second << "/" << sketch.at(i).first.size() << ")" << endl;
        vector<double> nivelI = sketch.at(i).first;
        for(int j=0;j<nivelI.size();j++){
            printf("%lf ",nivelI.at(j));
            //cout << nivelI.at(j) << " ";
        }
        cout << endl;
    }
}

pair<vector<double>, long> KLL::sketchAtLevel(long nivel){
    if(nivel<0||nivel>=numArreglos) return make_pair(vector<double>(),-1);
    return sketch.at(nivel);
}

void KLL::addToSampler(double element,uint64_t weight){
    if(!reservoirKLLSample(element,weight)) return;
    insertElement(0,sampleElement);
    return;
}

pair<uint64_t,uint64_t> KLL::getNumElementsSeen(){
    pair<uint64_t,uint64_t> toReturn;
    toReturn.first = numElementosRevisados;
    toReturn.second = numTotalElementos;
    return toReturn;
}

uint64_t KLL::numElementosRango(double a, double b){
    vector<pair<double,uint64_t>> vectorElementos; // par(elemento,peso) 
    
    // llenar el vector con los Elementos del sketch 
    for(int i=0;i<numArreglos;i++){
        uint64_t peso = pow(2,i);
        //for(int j=0;j<sketch.at(i).second;j++){
        for(int j=0;j<sketch.at(i).first.size();j++){
	        if(sketch.at(i).first.at(j) < 0) continue;
            pair<double, uint64_t> toInsert;
            toInsert.first = sketch.at(i).first.at(j);
            toInsert.second = peso;
            vectorElementos.push_back(toInsert);
        }
    }

    // realizar el sort
    sort(vectorElementos.begin(),vectorElementos.end());

    uint64_t numElementosRepetidos = 0;
    for(int i=0;i<vectorElementos.size();i++){
        if(vectorElementos.at(i).first < a) continue;
        else if (vectorElementos.at(i).first > b) break;
        numElementosRepetidos+=vectorElementos.at(i).second;
    }

    return numElementosRepetidos*pow(2,H_pp);
}

void KLL::update(KLL kll2){
    pair<double,double> minMaxOtherKLL = kll2.getMinMaxElement();
    if(minElement<minMaxOtherKLL.first) minElement = minMaxOtherKLL.first;
    if(maxElement>minMaxOtherKLL.second) maxElement = minMaxOtherKLL.second;
    pair<uint64_t,uint64_t> numElementsOtherKLL = kll2.getNumElementsSeen();
    numElementosRevisados += numElementsOtherKLL.first;
    numTotalElementos += numElementsOtherKLL.second;

    // ACORDARSE DE QUE kll1.height()>=kll2.height()
    if(isMrl){ // KLL 1 es Mrl
        if(kll2.isAnMrl()){ // KLL 2 es MRL
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
        }
        else{ // KLL 2 es KLL Tradicional
            pair<unsigned long,double> kll2sample = kll2.getCurrentSample();
            bool isPowerOf2 = false;
            unsigned long baseOf2=0;
            unsigned long kll2SampleWeight = kll2sample.first;
            if (kll2SampleWeight == 0) isPowerOf2 = false;
            while (kll2SampleWeight % 2 == 0) {
                kll2SampleWeight /= 2;
                baseOf2++;
            }
            if (kll2SampleWeight == 1) isPowerOf2=true;
            if(isPowerOf2) insertCompactionElement(baseOf2, kll2sample.second, true);
            else {

            }
        }
    } else { // KLL 1 es KLL Tradicional
        if(kll2.isAnMrl()){ // KLL 2 es un MRL
            // ingresamos elementos asociados al sampler
            int nivel = 0;
            for(nivel; nivel<H_pp;nivel++){
                if(nivel>=kll2.height()) break; //
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                uint64_t pesoNivel = pow(2,nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    addToSampler(kll2pair.first.at(i),pesoNivel);
                }
            }

            // ingresamos el resto de los elementos
            for(nivel;nivel<kll2.height(); nivel++){
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel,kll2pair.first.at(i),true);
                }
            }
        }
        else{ // KLL 2 es KLL tradicional
            //! PROBLEMA A RESOLVER: QUE OCURRE SI EL SAMPLER DE KLL2 TIENE UN PESO >= A 2**H_PP+1 del sampler KLL1
            int nivel = 0;
            
            // agregamos los elementos del sampler kll2 al sampler del kll1
            if(H_pp<kll2.getH_pp()+2){ // ReservoirSampling acepta pesos desde [1,2^(H_pp+1)[
                pair<unsigned long,double> sampleOtherKLL = kll2.getCurrentSample();
                addToSampler(sampleOtherKLL.second,sampleOtherKLL.first);
            } else { //!CASO PROBLEMA
                
            }
            
            // ingresar elementos de KLL 2 a Sampler de KLL 1
            if(H_pp>kll2.getH_pp()){
                for(nivel=0; nivel<H_pp;nivel++){
                    if(nivel>=kll2.height()) break; //
                    pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                    uint64_t pesoNivel = pow(2,nivel);
                    for(int i=0; i < kll2pair.second;i++){
                        if(kll2pair.first.at(i)<0) continue;
                        addToSampler(kll2pair.first.at(i),pesoNivel);
                    }
                }
            }

            int diffH_pp = H_pp - kll2.getH_pp();
            // ingresar los elementos de los compactor de KLL 2 a los compactor de KLL1
            for(nivel=0; nivel<kll2.height();nivel++){
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel-diffH_pp,kll2pair.first.at(i),true);
                }
            }
        }
    }
    
    return;
}

pair<unsigned long, double> KLL::getCurrentSample(){
    pair<unsigned long, double> toReturn;
    toReturn.first = sampleWeight;
    toReturn.second = sampleElement;
    return toReturn;
}

void KLL::setSeconds(vector<long> seconds){
    for(int i=0;i<numArreglos;i++){
        //sketch.at(i).second = seconds.at(i);
        cerr << sketch.at(i).second << endl;
    }
    
    return;
}

uint32_t KLL::getH_pp(){
    return H_pp;
}

uint32_t KLL::getH(){
    return H;
}

uint32_t KLL::getFirstLevelK(){
    return sketch.at(0).first.size();
}

KLL KLL::kllMerge(KLL &kll2){ 
    if(isMrl!=kll2.isAnMrl()){
        cerr << "Los KLL son incompatibles" << endl;
        return KLL(200);
    }

    uint64_t heightKLL1, heightKLL2;
    heightKLL1 = height();
    heightKLL2 = kll2.height();

    // SE ME OCURRIO CREAR UN METODO PARA EL CASO QUE NO SEA MRL, EN DONDE SE AGREGUEN LOS ELEMENTOS
    // AL RESERVOIR SAMPLING HASTA QUE SE LLEGUE AL NIVEL DE HPP EN EL KLL DE MAYOR NIVEL

    // Creamos una copia del kll a devolver
    if(isMrl){
        cerr << "isMrl" << endl;
        // Se verifica el MRL que tenga mayor altura para definir cual utilizar como base
        // en caso de misma altura se definira mediante cual tenga mayor minK
        if(heightKLL1 < heightKLL2){
            cerr << "kll 2 has higher height" << endl;
            KLL kllCopy2 = kll2.copy();
            kllCopy2.update(*this);
            return kllCopy2;
        } 
        else if(heightKLL1 > heightKLL2){
            cerr << "kll 1 has higher height" << endl;
            KLL kllCopy1 = copy();
            kllCopy1.update(kll2);
            return kllCopy1;
        } else if(getFirstLevelK() < kll2.getFirstLevelK()){
            cerr << "same height and kll 2 has higher minK" << endl;
            KLL kllCopy2 = kll2.copy();
            kllCopy2.update(*this);
            return kllCopy2;
        } else {
            cerr << "same height and kll 1 has higher or same minK" << endl;
            KLL kllCopy1 = copy();
            kllCopy1.update(kll2);
            return kllCopy1;
        }
    } else {
        cerr << "isKLL" << endl;
        if(heightKLL1 < heightKLL2){
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
}

bool KLL::isAnMrl(){
    return isMrl;
}

pair<double, double> KLL::getMinMaxElement(){
    pair<double,double> toReturn;
    toReturn.first = minElement;
    toReturn.second = maxElement;
    return toReturn;
}

// COPIA KLL TRADICIONAL
KLL::KLL(uint64_t nCopy, double epsilonCopy, double deltaCopy, double cCopy, uint32_t minKCopy, uint64_t numElementosRevisadosCopy, uint64_t numTotalElementosCopy, KLL* toCopy){
    n = nCopy;
    c = cCopy;
    epsilon = epsilonCopy;
    delta = deltaCopy;
    minK = minKCopy;
    numElementosRevisados = numElementosRevisadosCopy;
    numTotalElementos = numTotalElementosCopy;
    isMrl = false;

    pair<double, double> minMaxElement = toCopy->getMinMaxElement();
    minElement = minMaxElement.first;
    maxElement = minMaxElement.second;

    H = 1.5*log(epsilon*n); // O(log(epsilon*n))
    k = 1.5*(double)(1/epsilon) * log(log((1/delta)));
    s = log(log((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - log(k)
    if(H_pp<0) H_pp = 0;
    wH_pp = pow(2,H_pp);

    mascara = pow(2,H_pp);
    pair<unsigned long, double> samplePair = toCopy->getCurrentSample();
    sampleElement=samplePair.second;
    sampleWeight=samplePair.first;

    H = toCopy->getH();
    numArreglos = H-H_pp+1;

    cerr << "COPY KLL TRADICIONAL" << endl;
    cerr << "Num Elementos: " << n << endl;
    cerr << "K: " << k << endl;
    cerr << "c: " << c << endl;
    cerr << endl;
 
    // copiar los valores del kll a copiar
    for(int i=0;i<numArreglos;i++){
        sketch.push_back(toCopy->sketchAtLevel(i));
    }
}

// COPIA MRL
KLL::KLL(uint32_t minKCopy, uint64_t numTotalElementosCopy, KLL* toCopy){
    isMrl = true;
    
    pair<double, double> minMaxElement = toCopy->getMinMaxElement();
    minElement = minMaxElement.first;
    maxElement = minMaxElement.second;

    c=1;
    
    pair<unsigned long, double> samplePair = toCopy->getCurrentSample();
    sampleElement=samplePair.second;
    sampleWeight=samplePair.first;

    mascara = pow(2,0);

    H = toCopy->getH();
    numArreglos = H+1;
    numElementosRevisados = numTotalElementosCopy;
    numTotalElementos = numTotalElementosCopy;
    minK = minKCopy;
    k = minK;

    // copiar los valores del kll a copiar
    for(int i=0;i<numArreglos;i++){
        sketch.push_back(toCopy->sketchAtLevel(i));
    }

    H_p = 0;
    H_pp = 0;
}

bool KLL::areEqual(KLL toCompare){
    if(isMrl != toCompare.isAnMrl()) return false;
    cerr << "ISMRL " << isMrl << endl;
    if(H_pp!=toCompare.getH_pp()) return false;
    if(H!=toCompare.getH()) return false;
    cerr << "DATOS BASICOS IGUALES " << isMrl << endl;
    for(int i=0;i<numArreglos;i++){
        cerr << "Nivel: " << i << endl;
        pair<vector<double>,long> toCompareLvl = toCompare.sketchAtLevel(i);
        pair<vector<double>,long> actualLvl = sketchAtLevel(i);
        if(actualLvl.first.size()!=toCompareLvl.first.size()) return false;
        else if (actualLvl.second!=toCompareLvl.second) return false;
        for(int j=0;j<actualLvl.first.size();j++){
            if(actualLvl.first.at(j)==-1) actualLvl.first.at(j)=-2;
            if(toCompareLvl.first.at(j)==-1) toCompareLvl.first.at(j)=-2; 
            if(actualLvl.first.at(j)!=toCompareLvl.first.at(j)) return false;
        }
    }
    return true;
}

KLL KLL::copy(){
    //! INGRESAR ISMRL EN LA CREACION DE LA COPIA DE ESPACIO
    KLL copia = isMrl ? KLL(minK, numTotalElementos,this) : KLL(n,epsilon,delta,c,minK,numElementosRevisados,numTotalElementos,this);
    //KLL copia(n,epsilon,c,numElementosRevisados,this);
    return copia;
}

uint64_t KLL::sizeInBytes(){
    uint64_t totalSize = 0;
    uint64_t sketchSize = 0;

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
    totalSize+=sizeof(minK);
    totalSize+=sizeof(s);
    totalSize+=sizeof(mascara);
    totalSize+=sizeof(wH_pp);
    totalSize+=sizeof(debug);
    totalSize+=sizeof(isMrl);
    totalSize+=sizeof(minElement);
    totalSize+=sizeof(maxElement);

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
    parametros.push_back(minK);
    parametros.push_back(isMrl);
    return parametros;
}

uint64_t KLL::saveData(string outputFileName){
    //FORMATO:
    // KLL TRADICIONAL: isMRL, minElement, maxElement, n, epsilon, delta, c, minK, numTotalElementos, 
    //                       , sampleWeight, sampleElement
    //                       , numNiveles, numElementosRevisados, vector<posInicialNivel>, vector<elementos> 
    // MRL: isMrl, ,minElement, maxElement, minK, numNiveles, numElementosRevisados, vector<posInicialNivel>,
    //                       , vector<elementos> 


    // Abrir el archivo en modo binario
    std::ofstream archivo(outputFileName, std::ios::binary);
    uint64_t numBytes = 0;

    if (archivo) {
        // Almacenamos el tipo de estructura asociado
        archivo.write(reinterpret_cast<const char *>(&isMrl), sizeof(isMrl));
        numBytes += sizeof(isMrl);
        archivo.write(reinterpret_cast<const char *>(&minElement), sizeof(minElement));
        numBytes += sizeof(minElement);
        archivo.write(reinterpret_cast<const char *>(&maxElement), sizeof(maxElement));
        numBytes += sizeof(maxElement);
        if(isMrl){
            archivo.write(reinterpret_cast<const char *>(&minK), sizeof(minK));
    
            numBytes += sizeof(minK);
        } else {
            archivo.write(reinterpret_cast<const char *>(&n), sizeof(n));
            archivo.write(reinterpret_cast<const char *>(&epsilon), sizeof(epsilon));
            archivo.write(reinterpret_cast<const char *>(&delta), sizeof(delta));
            archivo.write(reinterpret_cast<const char *>(&c), sizeof(c));
            archivo.write(reinterpret_cast<const char *>(&minK), sizeof(minK));
            archivo.write(reinterpret_cast<const char *>(&numTotalElementos), sizeof(numTotalElementos));
            archivo.write(reinterpret_cast<const char *>(&sampleWeight), sizeof(sampleWeight));
            archivo.write(reinterpret_cast<const char *>(&sampleElement), sizeof(sampleElement));
            
            numBytes += sizeof(n);
            numBytes += sizeof(epsilon);
            numBytes += sizeof(delta);
            numBytes += sizeof(c);
            numBytes += sizeof(minK);
            numBytes += sizeof(numTotalElementos);
            numBytes += sizeof(sampleWeight);
            numBytes += sizeof(sampleElement);
        }

        // Almacenar datos necesarios numero de niveles y H_pp
        // H_pp: indica el peso asociado al primer nivel del sketch, necesario para consultas de RANK
        uint32_t numNiveles = sketch.size();
        uint64_t elementosRevisados = numElementosRevisados;

        archivo.write(reinterpret_cast<const char *>(&numNiveles), sizeof(numNiveles));
        archivo.write(reinterpret_cast<const char *>(&elementosRevisados), sizeof(elementosRevisados));
        numBytes += sizeof(numNiveles);
        numBytes += sizeof(elementosRevisados);

        // Almacenar el indice donde inicia el elemento siguiente del sketch respectivo
        uint32_t posInicialNivelActual = 0; 
        for(int i=0;i<sketch.size();i++){
            cerr << "posInicial: " << posInicialNivelActual << endl;
            archivo.write(reinterpret_cast<const char *>(&posInicialNivelActual), sizeof(posInicialNivelActual));
            posInicialNivelActual+=sketch.at(i).second;
            numBytes += sizeof(posInicialNivelActual);
        }
        archivo.write(reinterpret_cast<const char *>(&posInicialNivelActual), sizeof(posInicialNivelActual));
        numBytes += sizeof(posInicialNivelActual);

        // Almacenar los elementos existentes del sketch
        // Se almacena desde el elemento 0
        for(int i=0;i<sketch.size();i++){
            vector<double> nivelActual = sketch.at(i).first;
            for(int j=0;j<sketch.at(i).second;j++){
                double elementoAGuardar = nivelActual.at(j);
                archivo.write(reinterpret_cast<const char *>(&elementoAGuardar), sizeof(elementoAGuardar));
            }
        }

        // Cerrar el archivo
        archivo.close();

        std::cout << "Vector guardado en el archivo: " << outputFileName << std::endl;
    } else {
        std::cerr << "No se pudo abrir el archivo: " << outputFileName << std::endl;
        return 0;
    }

    //! PARA VER EL TAMAÑO DEL ARCHIVO
    streampos begin,end;
    ifstream myfile (outputFileName, ios::binary);
    begin = myfile.tellg();
    myfile.seekg (0, ios::end);
    end = myfile.tellg();
    myfile.close();
    cout << "size primera parte: " << numBytes << " bytes. \n";
    cout << "size is: " << (end-begin) << " bytes.\n";
    
    //print();
    numBytes= (end-begin);

    return numBytes;
}

// Creacion de MRL a partir de los datos proporcionados en readData
KLL::KLL(uint32_t minKRead, uint32_t numElementosRevisadosRead, vector<vector<double>> niveles, double minElementRead, double maxElementRead){
    uint32_t numNiveles = niveles.size();
    numArreglos = numNiveles;
    numElementosRevisados = numElementosRevisadosRead;
    numTotalElementos = numElementosRevisadosRead;
    minK = minKRead;
    k = minK;
    isMrl = true;
    H = numNiveles-1;
    H_pp = 0;
    s = numNiveles;
    minElement = minElementRead;
    maxElement = maxElementRead;

    double valorElementoVacio = -2;
    for(int i=0;i<niveles.size();i++){
        pair<vector<double>,long> par;
        par.first = niveles.at(i);
        par.second = niveles.at(i).size();
        for(int j=par.second;j<minK;j++){ // rellenar con elementos vacios
            par.first.push_back(valorElementoVacio);
        }
        cout << "par second: " << par.second << " de " << par.first.size() << endl;
        sketch.push_back(par);
    }
}

// Creacion de KLL tradicional a partir de los datos proporcionados en readData
KLL::KLL(uint64_t nRead, double epsilonRead,double deltaRead,double cRead, uint32_t minKRead,uint64_t numTotalElementosRead,unsigned long sampleWeightRead,double sampleElementRead, uint64_t numElementosRevisadosRead,vector<vector<double>> niveles, double minElementRead, double maxElementRead){
    isMrl = false;
    minElement = minElementRead;
    maxElement = maxElementRead;

    minK = minKRead;
    n = nRead;
    epsilon = epsilonRead;
    delta = deltaRead;
    c = cRead;

    H = 1.5*log(epsilon*n); // O(log(epsilon*n))
    k = 1.5*(double)(1/epsilon) * log(log((1/delta)));
    s = log(log((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - log(k)
    if(H_pp<0) H_pp = 0;
    wH_pp = pow(2,H_pp);

    mascara = pow(2,H_pp);
    sampleElement=sampleElementRead;
    sampleWeight=sampleWeightRead;

    numArreglos = (H - H_pp+1);
    numElementosRevisados = numElementosRevisadosRead;
    numTotalElementos = numTotalElementosRead;

    vector<long> sizeTemp;
    for(int i=H_pp;i<=H;i++){
        unsigned long long cantElementos;
        if(i>(H-s-1)) cantElementos = max(k,(long)minK);
        else cantElementos = max((int)(k*pow(c,H-i-1)),(int)minK);
        cout << i << " El: " << cantElementos << endl;
        //cout << "cantElementos en arreglo " << i+1 << ": " << cantElementos << endl;
        sizeTemp.push_back(cantElementos);
    }
    if(sizeTemp.size() < niveles.size()){
        for(int i=sizeTemp.size(); i<niveles.size();i++){
            sizeTemp.push_back(max(k,(long)minK));
            H++;
            s++;
        }
    }
        
    double valorElementoVacio = -2.0;
    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<niveles.size();i++){
        pair<vector<double>,long> par;
        par.first = niveles.at(i);
        par.second = niveles.at(i).size();
        for(int j=par.second;j<sizeTemp.at(i);j++){ // rellenar con elementos vacios
            par.first.push_back(valorElementoVacio);
        }
        cout << "par second: " << par.second << " de " << par.first.size() << endl;
        sketch.push_back(par);
    }

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
    
    for(int i=0;i<sketch.size();i++){
        cout << "Compactor en nivel " << H_pp+i << " tiene capacidad: " << sketch.at(i).first.size() << endl;
    }

    cout << endl;
}

KLL KLL::readData(string inputFileName){
    //FORMATO:
    // KLL TRADICIONAL: isMRL, n, epsilon, delta, c, minK, numTotalElementos, sampleWeight, sampleElement
    //                       , numNiveles, numElementosRevisados, vector<posInicialNivel>, vector<elementos> 
    // MRL: isMrl, minK, numNiveles, numElementosRevisados, vector<posInicialNivel>, vector<elementos> 

    std::ifstream archivo(inputFileName, std::ios::binary);
    if (!archivo) {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        return 1;
    } 

    // variables asociadas a la lectura de MRL o KLL tradicional
    // MRL y KLL tradicional:
    bool isMrlRead;
    double minElementRead, maxElementRead;
    uint32_t minKRead;
    uint32_t numNiveles;
    uint64_t numElementosRevisadosRead;
    vector<uint32_t> posInicialNivel; // almacena las posiciones asociadas, es auxiliar
    vector<vector<double>> niveles; // almacena los elementos guardados asociados al archivo en binario
    // exclusivos KLL tradicional
    uint64_t nRead; 
    uint64_t numTotalElementosRead; 
    double epsilonRead, deltaRead, cRead;
    unsigned long sampleWeightRead;
    double sampleElementRead;
    
    // Lectura de variables del archivo en binario
    archivo.read(reinterpret_cast<char*>(&isMrlRead), sizeof(isMrlRead));
    cerr << "isMrl " << isMrl << endl;
    archivo.read(reinterpret_cast<char*>(&minElementRead), sizeof(minElementRead));
    cerr << "minElement " << minElementRead << endl;
    archivo.read(reinterpret_cast<char*>(&maxElementRead), sizeof(maxElementRead));
    cerr << "maxElement " << maxElementRead << endl;
    if(isMrlRead){
        archivo.read(reinterpret_cast<char*>(&minKRead), sizeof(minKRead));
        cerr << "minKRead " << minKRead << endl;
    } else {
        archivo.read(reinterpret_cast<char*>(&nRead), sizeof(nRead));
        cerr << "nRead " << nRead << endl;
        archivo.read(reinterpret_cast<char*>(&epsilonRead), sizeof(epsilonRead));
        cerr << "epsilonRead " << epsilonRead << endl;
        archivo.read(reinterpret_cast<char*>(&deltaRead), sizeof(deltaRead));
        cerr << "deltaRead " << deltaRead << endl;
        archivo.read(reinterpret_cast<char*>(&cRead), sizeof(cRead));
        cerr << "cRead " << cRead << endl;
        archivo.read(reinterpret_cast<char*>(&minKRead), sizeof(minKRead));
        cerr << "minKRead " << minKRead << endl;
        archivo.read(reinterpret_cast<char*>(&numTotalElementosRead), sizeof(numTotalElementosRead));
        cerr << "numTotalElementosRead " << numTotalElementosRead << endl;
        archivo.read(reinterpret_cast<char*>(&sampleWeightRead), sizeof(sampleWeightRead));
        cerr << "sampleWeightRead " << sampleWeightRead << endl;
        archivo.read(reinterpret_cast<char*>(&sampleElementRead), sizeof(sampleElementRead));
        cerr << "sampleElementRead " << sampleElementRead << endl;
    }

    archivo.read(reinterpret_cast<char*>(&numNiveles), sizeof(numNiveles));
    cerr << "numNiveles " << numNiveles << endl;
    archivo.read(reinterpret_cast<char*>(&numElementosRevisadosRead), sizeof(numElementosRevisadosRead));
    cerr << "numElementosRevisadosRead " << numElementosRevisadosRead << endl;
    for(int i=0;i<numNiveles+1;i++){
        uint32_t posActual;
        archivo.read(reinterpret_cast<char*>(&posActual), sizeof(posActual));
        posInicialNivel.push_back(posActual);
    }
    
    cout << "Num Niveles: " << numNiveles << endl;
    cout << "Pos Iniciales:" << endl;
    for(int i=0;i<posInicialNivel.size();i++) cout << posInicialNivel.at(i) << " ";

    for(int i=0;i<numNiveles;i++){
        vector<double> nivelActual;
        for(int j=posInicialNivel.at(i);j<posInicialNivel.at(i+1);j++){
            double elementoActual;
            archivo.read(reinterpret_cast<char*>(&elementoActual), sizeof(elementoActual));
            nivelActual.push_back(elementoActual);
        }
        niveles.push_back(nivelActual);
    }

    if(isMrl) 
        return KLL(minKRead, numElementosRevisadosRead, niveles, minElementRead, maxElementRead);
    else 
        return KLL(nRead,epsilonRead,deltaRead,cRead,minKRead,numTotalElementosRead, sampleWeightRead, sampleElementRead, numElementosRevisadosRead, niveles, minElementRead, maxElementRead);
}