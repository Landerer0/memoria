#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "streamsketch.hpp"
#include "murmurhash.hpp"

//? Serie de constructores que crean un KLL

StreamSketch::StreamSketch(uint64_t numElementsParam, double epsilonParam, double deltaParam, double cParam, int minKp){
    setupStreamSketch(10);
    numElements = numElementsParam;
    epsilon = epsilonParam;
    delta = deltaParam;
    c = cParam;
    minK = minKp;
    addSketch();
} // KLL Tradicional

StreamSketch::StreamSketch(uint64_t numElementsParam, double epsilonParam, double cParam, int minKp){
    setupStreamSketch(11);
    numElements = numElementsParam;
    epsilon = epsilonParam;
    c = cParam;
    minK = minKp;
    addSketch();
} // KLL Tradicional que determina espacio a ocupar según epsilon entregado

StreamSketch::StreamSketch(uint64_t minKP){
    setupStreamSketch(20);
    minK = minKP;
    addSketch();
} // MRL

StreamSketch::StreamSketch(uint64_t espacioMaximoParam, uint64_t numElementsParam){
    setupStreamSketch(21);
    espacioMaximo = espacioMaximoParam;
    numElements = numElementsParam;
    addSketch();
} // MRL con Espacio Maximo dado para abarcar un numElementosParam determinado

StreamSketch::StreamSketch(double epsilonParam, uint64_t numElementsParam){
    setupStreamSketch(22);
    epsilon = epsilonParam;
    numElements = numElementsParam;
    addSketch();
} // MRL que determina espacio a ocupar según epsilon entregado

StreamSketch::~StreamSketch(){
    
}

void StreamSketch::setupStreamSketch(short typeKLLParam){
    typeKLL = typeKLLParam;
    numElements = 0;
    epsilon=0;
    delta=0;
    c=0;
    minK=0;
    espacioMaximo=0;
    sketchActual=0;
}

void StreamSketch::addSketch(){
    switch(typeKLL){
            case 10: // KLL Tradicional
                 sketches.push_back(KLL(numElements, epsilon, delta, c, minK));
                break;
            case 11: // KLL Tradicional que determina espacio a ocupar según epsilon entregado
                sketches.push_back(KLL(numElements, epsilon, c, minK));
                break;
            case 20: // MRL
                sketches.push_back(KLL(minK));
                break;
            case 21: // MRL con Espacio Maximo dado para abarcar un numElementosParam determinado
                sketches.push_back(KLL(espacioMaximo, numElements));
                break;
            case 22: // MRL que determina espacio a ocupar según epsilon entregado
                sketches.push_back(KLL(epsilon, numElements));
                break;
            default: 
                cerr << "error" << endl;
                exit(1);
        }
}


KLL StreamSketch::mergeSketches(){
    if(sketchActual==0) return sketches.at(0);
    KLL mergedSketch = sketches.at(0).kllMerge(sketches.at(1));

    // sketchActual va desde 0 hasta n-1
    for(int i=1;i<sketchActual;i++){
        mergedSketch.kllMerge(sketches.at(i+1));
    }

    return mergedSketch;
}


uint64_t StreamSketch::sizeInBytes(){
    uint64_t totalSize = 0;
    for(int i=0;i<=sketchActual;i++) totalSize+=sketches.at(i).sizeInBytes();
    return totalSize;
}

uint64_t StreamSketch::saveData(string outputFileName){
    uint64_t totalBytes=0;
    for(int i=0;i<=sketchActual;i++) totalBytes+=sketches.at(i).saveData(outputFileName+to_string(i));
    return totalBytes;
}

vector<double> StreamSketch::parametros(){
    return sketches.at(0).parametros();
}

void StreamSketch::printNumSketches(){
    cout << "|||   Stream Sketch Print   |||" << endl;
    cout << "numSketches: " << sketchActual+1 << ", typeKLL: " << typeKLL << endl << endl;
}

void StreamSketch::print(){
    cout << "|||   Stream Sketch Print   |||" << endl;
    cout << "numSketches: " << sketchActual+1 << ", typeKLL: " << typeKLL << endl << endl;
    for(int i=0;i<=sketchActual;i++) sketches.at(i).print();
 }

void StreamSketch::add(double element){
    if(sketches.at(sketchActual).add(element)){
        cerr << endl << endl << "!! se agrego nuevo sketch en Stream Sketch!!" << endl << endl;
        sketchActual++;
        addSketch();
    }
} // agregar element al sketch, retorna true cuando sketch se llenó tras la inserción

uint64_t StreamSketch::rank(double element){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.rank(element);
} // indica el rank del elemento proporcionado

vector<uint64_t> StreamSketch::rank(vector<double> elements){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.rank(elements);
} 

double StreamSketch::select(uint64_t rank){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.select(rank);
} // retorna el elemento cuyo rank es el indicado

pair<double, bool> StreamSketch::selectMinMax(uint64_t rank){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.selectMinMax(rank);
}

vector<double> StreamSketch::select(vector<uint64_t> rank){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.select(rank);
} 

vector<pair<double,bool>> StreamSketch::selectMinMax(vector<uint64_t> rank){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.selectMinMax(rank);
} 

double StreamSketch::quantile(double q){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.quantile(q);
} // retorna elemento encontrado en el quantil q

vector<double> StreamSketch::quantile(vector<double> q){
    KLL mergedSketch = mergeSketches();
    return mergedSketch.quantile(q);
} 