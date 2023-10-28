// g++ topk.cpp streamsketch.hpp streamsketch.cpp kll.cpp kll.hpp -o topk
// ./topk nombreBinario k             
// ./topk Mendeley.klle0.005000d0.001000c0.666660mk20.bin 20
// ./topk Mendeley.mrlk200.bin 20
// ./topk Chicago-20080319.klle0.005000d0.001000c0.666660mk20.bin 100
// ./topk Chicago-20080319.mrlk200.bin 100
// gdb -ex=r --args topk Chicago-20080319.klle0.005000d0.001000c0.666660mk20.bin 100
// gdb -ex=r --args topk Chicago-20080319.mrlk200.bin 100

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

using namespace std;

int main(int argc, char*argv[]){
    if(argc!=3){
        cout << "error en la entrega de parametros" << endl;
        return 0;
    }

    string nombreCarpeta = "saveKll";
    string nombreBinario = "";
    nombreBinario.append(nombreCarpeta+"/");
    nombreBinario.append(argv[1]);
    uint32_t k = stoi(argv[2]);

    KLL kll(200);
    kll = kll.readData(nombreBinario);

    vector<double> topK = kll.topKElements(k);

    cout << "para k: " << k << " topK elements:" << endl;
    for(int i=0;i<topK.size();i++){
        printf("%d\t %lf\n",i,topK.at(i));
        //if(topK.at(i)==3232235891.000000) cout << endl << "SE ENCONTRO 3232235891.000000" << endl; // para mendeley
    }
    cout << endl;

    return 0;
}

/*

Para medir el desempeño de la estimación, se utilizara la metrica de error relativo (AVE), que toma en 
consideración los elementos estimados a ser top-k y entrega una medida de cual es el error obtenido
segun sus frecuencias. Matematicamente, se encuentra expresado como:

    sum_i(1,k)[(|f^_i-f_i|/f_i)/k]

en donde
f^_i corresponde a la frecuencia estimada del elemento iesimo.
f_i corresponde a la frecuencia real del elemento iesimo
k representa el número de elementos seleccionados


Las figuras a continuación representan algunos resultados obtenidos, los cuales se encuentran separadas según
el elemento estimado perteneciente al top k, su estimación de frecuencia y su frecuencia real. Además
se tiene el nombre del archivo de trazas, la estructura KLL almacenada en un binario y el número k empleado.

*/