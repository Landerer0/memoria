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

// kll sin espacio limitado
KLL::KLL(uint64_t numElementsParam, double epsilonParam, double deltaParam, double cParam, int minKp){
    isMrl = false;
    espacioCte = false;
    espacioLimitado = false;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    minK = minKp;
    n = numElementsParam;
    epsilon = epsilonParam;
    delta = deltaParam;
    c = cParam;

    H = 1.0*log2(epsilon*n); // O(log(epsilon*n))
    k = 1.0*(double)(1/epsilon) * log2(log2((1/delta)));
    s = log2(log2((1/delta)));

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
}

KLL::KLL(uint64_t numElementsParam, double epsilonParam, double cParam, int minKp){ 
    // KLL Tradicional con espacio determinado por epsilon, delta=0.01
    isMrl = false;
    espacioCte = false;
    espacioLimitado = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    minK = minKp;
    n = numElementsParam;
    epsilon = epsilonParam;
    delta = 0.01;
    c = cParam;

    H = 1.0*log2(epsilon*n); // O(log(epsilon*n))
    k = 1.0*(double)(1/epsilon) * log2(log2((1/delta)));
    s = log2(log2((1/delta)));

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

    // HACER CALCULO AQUI DEL NUMERO DE ELEMENTOS ABARCADOS, en caso que no abarque aumentar H y H_pp en 1 
    uint64_t numElementosAbarcados = 0;
    uint64_t numVecesLlenaCompactor = 1;
    for(int i=sizeTemp.size()-2;i>=0;i--){
        numVecesLlenaCompactor = ceil( ((double)sizeTemp.at(i+1)*(double)numVecesLlenaCompactor) / ceil((double)sizeTemp.at(i)/2.0) );
    }
    cerr << "numVeces que se llena nivel inferior: " << numVecesLlenaCompactor << endl;
    uint64_t numElementosTotaleskll = pow(2,H_pp)*numVecesLlenaCompactor;
    while(numElementosTotaleskll < numElementsParam){
        cerr << "H: " << H << " H_pp: " << H_pp << " numElementosAbarcados: " << numElementosTotaleskll << "/" << numElementsParam<< endl;
        H_pp++;
        H++;
        wH_pp = pow(2,H_pp);
        mascara = pow(2,H_pp);
        numElementosTotaleskll *= 2;
    }
    cerr << "H: " << H << " H_pp: " << H_pp << " numElementosAbarcados: " << numElementosTotaleskll << "/" << numElementsParam<< endl;
    
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

KLL::KLL(uint64_t numElementsParam, double epsilonParam, double deltaParam, double cParam, int minKp, bool samplerEnMenorNivel){
    isMrl = false;
    espacioCte = true;
    espacioLimitado = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    minK = minKp;
    n = numElementsParam;
    epsilon = epsilonParam;
    delta = deltaParam;
    c = cParam;

    H = 1.0*log2(epsilon*n); // O(log(epsilon*n))
    k = 1.0*(double)(1/epsilon) * log2(log2((1/delta)));
    s = log2(log2((1/delta)));

    H_p = H-s;
    H_pp = H - ceil((double)log(k) / (double)log((1/c)) ); // H_pp = H - log(k)
    cout << "VARIABLES ANTES DE MODIFICARLAS, H: " << H << ", H'': " << H_pp << endl;
    if(H_pp<0) H_pp = 0;
    if(samplerEnMenorNivel){
        H -= H_pp;
        H_pp = 0;
    }
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
        << "Num Arreglos: " << (numArreglos) << endl
        << "w_H'': " << wH_pp << endl;
    
    for(int i=0;i<sketch.size();i++){
        cout << "Compactor en nivel " << H_pp+i << " tiene capacidad: " << sketch.at(i).first.size() << endl;
    }

    cout << endl;
} // KLL con espacio constante, variable "samplerEnMenorNivel" indica que H'' debe ser 0
        

KLL::KLL(uint64_t minKP){ // MRL sin espacioLimitado
    espacioLimitado = false;
    espacioCte = false;
    isMrl = true;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;
    c=1;
    epsilon = -1;
    
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

KLL::KLL(uint64_t espacioMaximo, uint64_t numElementsParam){ // MRL que se le entrega el espacio
    espacioLimitado = true;
    espacioCte = false;
    cout << "Espacio Maximo: " << espacioMaximo << " NumElementosAAbarcar: " << numElementsParam << endl;
    isMrl = true;
    epsilon = -1;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;
    c=1;
    
    wH_pp = pow(2,0);

    mascara = pow(2,0);
    sampleElement=0;
    sampleWeight=0;

    numElementosRevisados = 0;
    numTotalElementos = 0;

    uint64_t numNivelesNecesarios = -1;
    uint64_t minKNecesario = 0;
    uint64_t numElementosP = numElementsParam;
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
        cerr << "NumNiveles: " << numNivelesNecesarios+1 << " NumElementosAbarcados: " << numElementosAbarcados << "/" << numElementsParam<<endl;
        cerr << "\t Espacio ocupado: " << espacioIteracion  << "/" << espacioOcupado << " Cantidad de elementos por nivel: " << cantElementosIteracion << " Espacio por nivel: " << espacioPorNivel << " ... " << espacioPorNivel*(numNivelesNecesarios+1) << endl;

        cantElementosPorNivel = cantElementosIteracion;
        if(numElementosAbarcados>=numElementsParam){
            parametrosEncontrados = true;
            n = numElementosAbarcados;
        }
    }

    cerr << "Num niveles: " << numNivelesNecesarios+1 << endl;
    cerr << "cant de elementos por nivel: " << cantElementosPorNivel << endl;

    minK = cantElementosPorNivel;
    k = minK;

    vector<long> sizeTemp;
    unsigned long long cantElementos;
    cantElementos = minK;
    for(int i=0;i<=numNivelesNecesarios;i++) sizeTemp.push_back(cantElementos);
    

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

    H = numNivelesNecesarios; 
    H_p = 0;
    H_pp = 0;
    numArreglos = H-H_pp+1;
    //cout << "PRINTTTTTTTTTTTTTTTTTTT" << endl;
    //print();

    sizeInBytes();
}

KLL::KLL(double epsilonParam ,uint64_t numElementsParam){
    espacioCte = false;
    double cotaEspacio = 1.0/epsilonParam * log2(1.0/epsilonParam) * log2(1.0/epsilonParam);
    uint64_t espacioMaximo = ceil(cotaEspacio);
    cout << "Epsilon Entregado: " << epsilonParam << " Espacio Maximo: " << espacioMaximo << " NumElementosAAbarcar: " << numElementsParam << endl;
    //desde aqui es el mismo constructor que  KLL(uint64_t espacioMaximo, uint64_t numElementsParam)
    espacioLimitado = true;
    isMrl = true;
    epsilon = epsilonParam;
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
    uint64_t numElementosP = numElementsParam;
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
        cerr << "NumNiveles: " << numNivelesNecesarios+1 << " NumElementosAbarcados: " << numElementosAbarcados << "/" << numElementsParam<<endl;
        cerr << "\t Espacio ocupado: " << espacioIteracion  << "/" << espacioOcupado << " Cantidad de elementos por nivel: " << cantElementosIteracion << " Espacio por nivel: " << espacioPorNivel << " ... " << espacioPorNivel*(numNivelesNecesarios+1) << endl;

        cantElementosPorNivel = cantElementosIteracion;
        if(numElementosAbarcados>=numElementsParam){
            parametrosEncontrados = true;
            n = numElementosAbarcados;
        }
    }

    cerr << "Num niveles: " << numNivelesNecesarios+1 << endl;
    cerr << "cant de elementos por nivel: " << cantElementosPorNivel << endl;

    minK = cantElementosPorNivel;
    k = minK;

    
    vector<long> sizeTemp;
    unsigned long long cantElementos;
    cantElementos = minK;
    for(int i=0;i<=numNivelesNecesarios;i++) sizeTemp.push_back(cantElementos);
    

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

    H = numNivelesNecesarios; 
    H_p = 0;
    H_pp = 0;
    numArreglos = H-H_pp+1;
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
    if(nivel+1==numArreglos && posAInsertar == sketch.at(nivel).first.size()) cerr << "error insertCompactionElement" << endl;
    
    //if(updating) cerr << "nivel " << nivel<<"/"<<numArreglos << ", posAInsertar: " << posAInsertar<<"/"<<sketch.at(nivel).first.size() << endl;

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
        //compaction(nivel,updating);
        iterativeCompaction(nivel,updating);
        posAInsertar = sketch.at(nivel).second;
    }
}

void KLL::constantSpaceCompaction(){
    // crear vector< pair<vector<double>,long > >
    vector<pair<vector<double>, long> > auxSketch = sketch;

    // vaciar/limpiar sketch
    for(int nivel=0;nivel<numArreglos;nivel++){
        sketch.at(nivel).second = 0;
        for(int j=0;j<sketch.at(nivel).first.size();j++){
            sketch.at(nivel).first.at(j)=-2.0;
        }
    }

    H++;
    H_pp++;
    wH_pp *= 2;
    mascara *= 2;

    // agregar los elementos de menor nivel al sketch mediante reservoirSampling
    for(int i=0;i<sketch.at(0).first.size();i++){
        if(sketch.at(0).first.at(i)>0){
            if(reservoirKLLSample(sketch.at(0).first.at(i),pow(2,H_pp-1)))
                insertElement(0,sampleElement);
        } 
    }

    // agregar el resto de los elementos desde auxSketch a sketch 
    for(int nivel=1;nivel<numArreglos;nivel++){
        for(int i=0;i<auxSketch.at(nivel).second;i++){
            insertCompactionElement(nivel-1,auxSketch.at(nivel).first.at(i),false);
        }
    }

    return;
}

vector<double> seleccionElementosACompactar(vector<double> &elements){
    vector<double> toReturn;
    unsigned char elementosPares = 0;
        if(rand()%2==0) elementosPares = 0; // se mantienen los elementos pares
        else elementosPares = 1; // se mantienen los elementos impares
    
    // sort de los elementos
    sort(elements.begin(), elements.end());

    //! Agregar los elementos de mayor a menor (en iterativeCompaction)
    for(int i=elementosPares;i<elements.size();i+=2){
        toReturn.push_back(elements.at(i));
    }

    //! Agregar los elementos de menor a mayor (en iterativeCompaction), PROBLEMA (TEORICO NO CONFIRMADO) CUANDO QUEDA 1 ELEMENTO EN EL COMPACTOR
    // for(int i=(elements.size()-1)-elementosPares;i>=0;i-=2){
    //     toReturn.push_back(elements.at(i));
    // }

    /*
    // indicar que los elementos fueron vaciados
    for(int i=0;i<elements.size();i++){
        elements.at(i)=-1.0;
    }
    */

    return toReturn;
}

bool KLL::iterativeCompaction(long nivelInicial, bool updating){
    long numElementosOcupados = sketch.at(nivelInicial).second;
    long numElementosTotales = sketch.at(nivelInicial).first.size();
    unsigned char elementosPares = 0;

    vector<vector<double>> elementosACompactar;
    bool seDebeCompactar = false;
    
    //print();

    if(numElementosOcupados>=numElementosTotales){
        if(updating) print();
        // Veo si se lleno y se tiene espacio limitado
        if(nivelInicial==H && espacioLimitado && espacioCte){
            constantSpaceCompaction(); // dejar trabajo de compactacion a constantSpaceCompaction
            return false;
        } 
        else if(nivelInicial==H-H_pp && espacioLimitado && !updating) return true; // indicar que se lleno sketch
        else if(nivelInicial==H-H_pp){ // se puede agregar un nivel superior
            vector<double> vectorAtLvlI(sketch.at(nivelInicial).first.size(),-2); 
            pair<vector<double>,long> toInsert;
            toInsert.first=vectorAtLvlI;
            toInsert.second=0; // representa el num de elementos ocupados en el arreglo
            sketch.push_back(toInsert);
            cerr << "!!! Se agrego compactor de nivel superior: " << sketch.size()-1 << " ... numElementos:" << numTotalElementos << endl;
            numArreglos++;
            H++;
        }
        
        if(updating) cerr << "nivelAgregado:" << nivelInicial << endl;
        elementosACompactar.push_back(seleccionElementosACompactar(sketch.at(nivelInicial).first));
        //cerr << "se compactara nivelInicial: " << nivelInicial << endl;
    } 

    while(!elementosACompactar.empty()){
        long indiceVector = elementosACompactar.size()-1;
        //long indiceVectorSize = elementosACompactar.at(indiceVector).size()-1;
        //cerr << "indiceVector" << indiceVector << endl;

        // 1 saco elemento a insertar en nivel superior
        if(elementosACompactar.at(indiceVector).empty()){
            if(updating) cerr << "se vacio nivel " << nivelInicial+indiceVector << endl;
            if(sketch.at(nivelInicial+indiceVector).first.at(sketch.at(nivelInicial+indiceVector).first.size()-1)!=-1.0) sketch.at(nivelInicial+indiceVector).first.at(sketch.at(nivelInicial+indiceVector).first.size()-1)=-1.0;
            //for(int i=0;i<sketch.at(nivelInicial+indiceVector).first.size();i++) cerr << sketch.at(nivelInicial+indiceVector).first.at(i) << " ";
            //cerr << endl;
            elementosACompactar.pop_back();
            continue;
        }
        double elementoAIngresar = elementosACompactar.at(indiceVector).at( elementosACompactar.at(indiceVector).size()-1 );
        elementosACompactar.at(indiceVector).pop_back();

        // Ingreso en compactor de nivel superior el elemento 
        long nivelAIngresar = nivelInicial + indiceVector + 1;
        long posAInsertar = sketch.at(nivelAIngresar).second;
        //cerr << "indice a ingresar: " << posAInsertar << "/" << sketch.at(nivelAIngresar).first.size() << endl;
        sketch.at(nivelAIngresar).first.at(posAInsertar) = elementoAIngresar;
        sketch.at(nivelAIngresar).second++;

        // elimino elementos asociados del elemento seleccionado
        long posAEliminar = sketch.at(nivelAIngresar-1).second-1;
        sketch.at(nivelAIngresar-1).first.at(posAEliminar) = -1.0;
        sketch.at(nivelAIngresar-1).second--;
        posAEliminar--;
        if(posAEliminar>=0){
            sketch.at(nivelAIngresar-1).first.at(posAEliminar) = -1.0;
            sketch.at(nivelAIngresar-1).second--;
        }
        //cerr << "nivel inferior " << nivelAIngresar-1 << " quedo con pos " << sketch.at(nivelAIngresar-1).second << endl;

        //print();

        // veo si es necesario compactar el nivel superior
        numElementosOcupados = sketch.at(nivelAIngresar).second;
        numElementosTotales = sketch.at(nivelAIngresar).first.size();
        if(numElementosOcupados>=numElementosTotales){
            
            // Veo si se lleno y se tiene espacio limitado
            if(nivelAIngresar==H-H_pp && espacioLimitado && espacioCte){
                    constantSpaceCompaction(); // dejar trabajo de compactacion a constantSpaceCompaction
                    return false;
            } 
            else if(nivelAIngresar==H-H_pp && espacioLimitado & !updating) return true; // indicar que se lleno sketch
            else if(nivelAIngresar==H-H_pp){ // se puede agregar un nivel superior
                vector<double> vectorAtLvlI(sketch.at(nivelAIngresar).first.size(),-2); 
                pair<vector<double>,long> toInsert;
                toInsert.first=vectorAtLvlI;
                toInsert.second=0; // representa el num de elementos ocupados en el arreglo
                sketch.push_back(toInsert);
                cerr << "!!! Se agrego compactor de nivel superior: " << sketch.size()-1 << " ... numElementos:" << numTotalElementos << endl;
                numArreglos++;
                H++;
            }
            
            elementosACompactar.push_back(seleccionElementosACompactar(sketch.at(nivelAIngresar).first));
        } 
    }
    
    
    //!
    // cerr << "FIN COMPACTACION" << endl;
    // print();
    
    
    return false;
}

bool KLL::compaction(long nivel, bool updating){
    //cout << "Compaction: " << nivel <<  endl;
    //print();

    long numElementosOcupados = sketch.at(nivel).second;
    long numElementosTotales = sketch.at(nivel).first.size();
    unsigned char elementosPares = 0;
    if(numElementosOcupados>=numElementosTotales){
        //cerr << "NIVEL: " << nivel << "/" << numArreglos-1 << endl;
        if(espacioLimitado && nivel+1==numArreglos) {
            if(espacioCte){
                cerr << "CONSTANT SPACE COMPACTION" << endl;
                constantSpaceCompaction();
                return true;
            }
            else if(!updating){
                cout << "Se lleno sketch" << endl;
                return true;
            }
        }
        else if(nivel+1==numArreglos) {
            vector<double> vectorAtLvlI(sketch.at(nivel).first.size(),-2); 
            pair<vector<double>,long> toInsert;
            toInsert.first=vectorAtLvlI;
            toInsert.second=0; // representa el num de elementos ocupados en el arreglo
            sketch.push_back(toInsert);
            cerr << "!!! " << sketch.size()-1 << " ... numElementos:" << numTotalElementos << endl;
            numArreglos++;
            H++;
        };
        if(debug) cerr << endl << "compaction " << nivel+1 << endl;
        if(rand()%2==0) elementosPares = 0; // se mantienen los elementos pares
        else elementosPares = 1; // se mantienen los elementos impares

        // sort del arreglo
        sort(sketch.at(nivel).first.begin(), sketch.at(nivel).first.end());
        
        if(debug) print();
        

        bool siguienteCompactorLleno=false;

        // insertar la mitad de elementos (los pares o impares) en el sgte. nivel
        for(int i=elementosPares;i<numElementosTotales;i+=2){
            if(sketch.at(nivel+1).second==sketch.at(nivel+1).first.size()) siguienteCompactorLleno=true;
            if(!siguienteCompactorLleno)insertCompactionElement(nivel+1,sketch.at(nivel).first.at(i),updating);
        }

        long nullElement = -1;
        for(int i=0;i<numElementosTotales;i++){
            if(!siguienteCompactorLleno) sketch.at(nivel).first.at(i) = nullElement;
        }
        
        if(!siguienteCompactorLleno) sketch.at(nivel).second = 0;

        if(siguienteCompactorLleno) return true;

        if(debug) cout << "Fin de compaction " << nivel+1 << endl;
        if(debug) print();

        return false;
        //nivel++;
        //numElementosOcupados = sketch.at(nivel).second;
        //numElementosTotales = sketch.at(nivel).first.size();
    }
    return false;
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

    if(!reservoirKLLSample(element,1)) return false; // como no se agrega un elemento al sketch, este no se llena
    insertElement(0,sampleElement);
    //cout << element << endl;
    numElementosRevisados++; // para metodo quantile
    //return compaction((long) 0, false);
    return iterativeCompaction(0, false);

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
    iterativeCompaction((long) 0, false);

    return;
}

// add utilizado cuando se hacer merge de KLL
void KLL::addv(double element){
    insertElement(0,element);
    iterativeCompaction((long) 0, true);

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

    // for(int i=0;i<elements.size();i++){
    //     cerr << "Element " << i << ": " << elements[i] << endl;
    // }

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
    // en caso de que algun kll tenga espacioLimitado se realizara la compactación necesaria,
    // esto se hara de arriba hacia abajo con el fin de evitar choques
    if(hasLimitedSpace()){
        for(int i=sketch.size()-1;i>=0;i--){
            if(sketch.at(i).first.size()==sketch.at(i).second) iterativeCompaction(i,true);
        }
    }

    if(kll2.hasLimitedSpace()){
        for(int i=kll2.getH()-kll2.getH_pp();i>=0;i--){
            if(kll2.sketchAtLevel(i).first.size()==kll2.sketchAtLevel(i).second){
                kll2.iterativeCompaction(i,true);
            }
        }
    }


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
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel,kll2pair.first.at(i),true);
                }
            }
        }
        else{ // KLL 2 es KLL Tradicional
            pair<unsigned long,double> kll2sample = kll2.getCurrentSample();
            // bool isPowerOf2 = false;
            // unsigned long baseOf2=0;
            unsigned long kll2SampleWeight = kll2sample.first;

            //insert elementos del sampler a mrl
            if (kll2SampleWeight != 0) { // si es 0 no hay que ingresar nada
                // se calculara el nivel correspondiente en el que insertar el elemento
                // para ello, se calculara el nivel al que deberia ingresarse segun el peso del sampler
                // en el que se tomara como relevancia el peso del sampler y de los niveles asociados

                kll2SampleWeight = kll2sample.first;
                uint64_t nivelSuperior=0; // nivel inferior siempre es (nivelSuperior-1)
                while (kll2SampleWeight /2 != 0) {
                    kll2SampleWeight /= 2;
                    nivelSuperior++;
                }
                uint64_t pesoNivelSuperior = 1<<nivelSuperior;
                uint64_t pesoNivelInferior = 1<<(nivelSuperior-1);
                
                double probNivelSuperior = (double) (kll2sample.first-pesoNivelInferior)/(double) (pesoNivelSuperior-pesoNivelInferior);
                if((double) (rand()/RAND_MAX) <= probNivelSuperior) insertCompactionElement(nivelSuperior, kll2sample.second, true);
                else insertCompactionElement(nivelSuperior-1, kll2sample.second, true);

                // while (kll2SampleWeight %2 == 0) {
                //     kll2SampleWeight /= 2;
                //     baseOf2++;
                // }
                // if (kll2SampleWeight == 1) isPowerOf2=true;
                // if(isPowerOf2) insertCompactionElement(baseOf2, kll2sample.second, true);
                // else {

                // }

            }
            
            //insert elementos de kll a mrl en el nivel respectivo
            for(int nivel=kll2.getH_pp();nivel<kll2.height(); nivel++){
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel,kll2pair.first.at(i),true);
                }
            }
        }
    } else { // KLL 1 es KLL Tradicional
        if(kll2.isAnMrl()){ // KLL 2 es un MRL
            // ingresamos elementos asociados al sampler
            int nivel = 0;
            for(nivel; nivel<H_pp;nivel++){
                if(nivel>=kll2.height()) break; // ya se ingresaron todos los niveles
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                uint64_t pesoNivel = pow(2,nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    addToSampler(kll2pair.first.at(i),pesoNivel);
                }
            }

            // ingresamos el resto de los elementos al compactor asociado
            for(nivel;nivel<kll2.height(); nivel++){
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel-H_pp,kll2pair.first.at(i),true);
                }
            }
        }
        else{ // KLL 2 es KLL tradicional
            int nivel = 0;
            pair<unsigned long,double> kll2sample = kll2.getCurrentSample();
            unsigned long kll2SampleWeight = kll2sample.first;

            //cerr << "INSERT ELEMENTO DE SAMPLER A SAMPLER" << endl;

            //insert elementos del sampler a kll
            if(kll2SampleWeight == 0){} // si es 0 no hay que ingresar nada
            else if(wH_pp < kll2SampleWeight) {  // caso en que el peso es superior a H_pp (se agrega el elemento a un compactor)
                // se calculara el nivel correspondiente en el que insertar el elemento
                // para ello, se calculara el nivel al que deberia ingresarse segun el peso del sampler
                // en el que se tomara como relevancia el peso del sampler y de los niveles asociados

                kll2SampleWeight = kll2sample.first;
                uint64_t nivelSuperior=0; // nivel inferior siempre es (nivelSuperior-1)
                while (kll2SampleWeight /2 != 0) {
                    kll2SampleWeight /= 2;
                    nivelSuperior++;
                }
                uint64_t pesoNivelSuperior = 1<<nivelSuperior;
                uint64_t pesoNivelInferior = 1<<(nivelSuperior-1);
                
                double probNivelSuperior = (double) (kll2sample.first-pesoNivelInferior)/(double) (pesoNivelSuperior-pesoNivelInferior);
                if((double) (rand()/RAND_MAX) <= probNivelSuperior) insertCompactionElement(nivelSuperior-H_pp, kll2sample.second, true);
                else insertCompactionElement(nivelSuperior-1-H_pp, kll2sample.second, true);
            } else { // aqui se agrega el elemento desde kll2_sampler a kll1_sampler
                addToSampler(kll2sample.second,kll2sample.first);
            }
            
            int nivelesEnSampler=0;
            int diffH_pp = H_pp - kll2.getH_pp();
            //cerr << "nivelInsertASampler" << nivel << endl;
            // ingresar elementos de KLL 2 a Sampler de KLL 1 en caso de ser necesario
            if(H_pp>kll2.getH_pp()){
                //cerr << "INSERT ELEMENTO DE KLL2 A SAMPLER" << endl;
                for(nivel=0; nivel<diffH_pp;nivel++){
                    if(nivel>=kll2.height()) break; //
                    pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                    uint64_t pesoNivel = pow(2,nivel+kll2.getH_pp());
                    for(int i=0; i < kll2pair.second;i++){
                        if(kll2pair.first.at(i)<0) continue;
                        addToSampler(kll2pair.first.at(i),pesoNivel);
                    }
                    nivelesEnSampler++;
                }
            }

            // cerr << "INSERT ELEMENTO DE COMPACTOR A COMPACTOR" << endl;
            // cerr << "difH_pp: " << diffH_pp << ", H_pp: " << H_pp << ", kll2H_pp: " << kll2.getH_pp() << endl;
            // cerr << "H: " << H << ", kll2H: " << kll2.getH() << endl;
            // cerr << "niveles en sampler: " << nivelesEnSampler << endl;
            // ingresar los elementos de los compactor de KLL 2 a los compactor de KLL1
            for(nivel; nivel<kll2.height();nivel++){
                //cerr <<  "nivel en kll2: " << nivel<< endl;
                pair<vector<double>,long> kll2pair = kll2.sketchAtLevel(nivel);
                //cerr <<  "nivel a ingresar en kll1: " << nivel-nivelesEnSampler << endl;
                for(int i=0; i < kll2pair.second;i++){
                    if(kll2pair.first.at(i)<0) continue;
                    insertCompactionElement(nivel-nivelesEnSampler,kll2pair.first.at(i),true);
                }
                cerr << "nivel:" << nivel-nivelesEnSampler+H_pp << endl;
                print(); //!
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
    heightKLL1 = H;
    heightKLL2 = kll2.getH();

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
    espacioLimitado = toCopy->hasLimitedSpace();
    espacioCte = toCopy->hasConstantSpace();

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
    espacioLimitado = toCopy->hasLimitedSpace();
    espacioCte = false;

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
    totalSize+=sizeof(espacioLimitado);
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
    std::ofstream archivo(outputFileName+"bin", std::ios::binary);
    uint64_t numBytes = 0;

    if (archivo) {
        // Almacenamos el tipo de estructura asociado
        archivo.write(reinterpret_cast<const char *>(&isMrl), sizeof(isMrl));
        numBytes += sizeof(isMrl);
        archivo.write(reinterpret_cast<const char *>(&espacioLimitado), sizeof(espacioLimitado));
        numBytes += sizeof(espacioLimitado);
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
            archivo.write(reinterpret_cast<const char *>(&espacioCte), sizeof(espacioCte));
            
            numBytes += sizeof(n);
            numBytes += sizeof(epsilon);
            numBytes += sizeof(delta);
            numBytes += sizeof(c);
            numBytes += sizeof(minK);
            numBytes += sizeof(numTotalElementos);
            numBytes += sizeof(sampleWeight);
            numBytes += sizeof(sampleElement);
            numBytes += sizeof(espacioCte);
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
KLL::KLL(uint32_t minKRead, uint32_t numElementosRevisadosRead, vector<vector<double>> niveles, double minElementRead, double maxElementRead, bool espacioLimitadoRead){
    uint32_t numNiveles = niveles.size();
    numArreglos = numNiveles;
    espacioLimitado = espacioLimitadoRead;
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

    espacioCte = false;

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
KLL::KLL(uint64_t nRead, double epsilonRead,double deltaRead,double cRead, uint32_t minKRead,uint64_t numTotalElementosRead,unsigned long sampleWeightRead,double sampleElementRead, uint64_t numElementosRevisadosRead,vector<vector<double>> niveles, double minElementRead, double maxElementRead, bool espacioLimitadoRead,bool espacioCteRead){
    isMrl = false;
    espacioCte = espacioCteRead;
    espacioLimitado = espacioLimitadoRead;
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
    bool espacioCteRead;
    bool espacioLimitadoRead;
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
    cerr << "isMrl " << isMrlRead << endl;
    archivo.read(reinterpret_cast<char*>(&espacioLimitadoRead), sizeof(espacioLimitadoRead));
    cerr << "espacioLimitado " << espacioLimitadoRead << endl;
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
        archivo.read(reinterpret_cast<char*>(&espacioCte), sizeof(espacioCte));
        cerr << "espacioCte " << espacioCte << endl;
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
        return KLL(minKRead, numElementosRevisadosRead, niveles, minElementRead, maxElementRead, espacioLimitadoRead);
    else 
        return KLL(nRead,epsilonRead,deltaRead,cRead,minKRead,numTotalElementosRead, sampleWeightRead, sampleElementRead, numElementosRevisadosRead, niveles, minElementRead, maxElementRead, espacioLimitadoRead, espacioCteRead);
}

bool KLL::hasLimitedSpace(){
    return espacioLimitado;
}

bool KLL::hasConstantSpace(){
    return espacioCte;
}



// CREACION KLL CON PARAMETROS INDICADOS
KLL::KLL(uint64_t HParam,uint64_t sParam,uint64_t H_ppParam,vector<int> sizeNivelesParam, bool isAnMrlParam, bool hasLimitedSpaceParam){
    isMrl = isAnMrlParam;
    espacioLimitado = hasLimitedSpaceParam;
    espacioCte = false;
    H = HParam;
    s = sParam;
    H_p = H-sParam;
    H_pp = H_ppParam;
    wH_pp = pow(2,H_pp);

    mascara = pow(2,H_pp);
    sampleElement=0;
    sampleWeight=0;
    minElement = std::numeric_limits<double>::max();
    maxElement = -1*minElement;

    numArreglos = (H - H_pp+1);
    numElementosRevisados = 0;
    numTotalElementos = 0;

    uint64_t espacioOcupado = 0;

    minK = sizeNivelesParam.at(0);
    k = sizeNivelesParam.at(sizeNivelesParam.size()-1);

    // inicializar los vectores de tam k*c^lvl
    for(int i=0;i<sizeNivelesParam.size();i++){
        // el valor por defecto es -1, que indica "vacio"
        unsigned long long cantElementos = sizeNivelesParam.at(i);
        espacioOcupado += cantElementos;
        //cerr << "Cantidad Elementos arreglo " << i << " (nivel " << i+H_pp+1 <<") :" << cantElementos<< endl;
        double valorElemento = -2;
        vector<double> vectorAtLvlI(cantElementos,valorElemento); 
        pair<vector<double>,long> toInsert;
        toInsert.first=vectorAtLvlI;
        toInsert.second=0; // representa el num de elementos ocupados en el arreglo
        sketch.push_back(toInsert);
    }

    string nombreKLL;
    if(isMrl) nombreKLL = " MRL ";
    else nombreKLL = " KLL ";
    cerr << "Se creo" << nombreKLL << "con parametros:" << endl;
    print();

} // MRL/KLL que se le indican los parametros que va a ocupar
