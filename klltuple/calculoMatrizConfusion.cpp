// g++ calculoMatrizConfusion.cpp -o calculoMatrizConfusion
// ./calculoMatrizConfusion archivoReal archivoEstimado
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio10256flujosChicago2015.klltuplee0.050000d0.001000c0.666666mk20h7.txt
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio30096flujosChicago2015.klltuplee0.010000d0.001000c0.666666mk20h7.txt
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio45472flujosChicago2015.klltuplee0.007500d0.001000c0.666666mk20h7.txt
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio55736flujosChicago2015.klltuplee0.005000d0.001000c0.666666mk20h7.txt
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio264768flujosChicago2015.klltuplee0.001000d0.001000c0.666666mk20h7.txt

// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio27936flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk39317594h7.txt
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio106312flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk39317594h7.txt

// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio24720flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk39317594h7
// ./calculoMatrizConfusion sortflujosChicago2015.txt estimacionFrecuencias/Espacio90296flujosChicago2015.klltuplee0.000000d0.000000c0.000000mk39317594h7


#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

int main(int argc, char*argv[]){
    string archivo1 = argv[1];
    string archivo2 = argv[2];
    vector<pair<int64_t, int64_t>> reales;
    vector<int64_t> flujosReales;
    vector<pair<int64_t, int64_t>> estimados;
    vector<int64_t> flujosEstimados;

    ifstream archivoReal(archivo1);
    int64_t payload, flujo;
    while (archivoReal >> payload >> flujo) {
        reales.push_back(make_pair(payload, flujo));
        flujosReales.push_back(flujo);
    }
    archivoReal.close();

    ifstream archivoEstimado(archivo2);
    while (archivoEstimado >> payload >> flujo) {
        estimados.push_back(make_pair(payload, flujo));
        flujosEstimados.push_back(flujo);
    }
    archivoEstimado.close();

    // Contadores para TP, FP y FN
    vector<pair<int64_t, int64_t>> TP, FP, FN;

    // Comparar segundos componentes y contar TP, FP y FN
    for (int i=0;i<flujosEstimados.size();i++) {
        int flujoEstimado = flujosEstimados.at(i);
        auto it = find_if(flujosReales.begin(), flujosReales.end(),
            [&flujoEstimado](const auto& flujoReal) {
                return flujoEstimado == flujoReal;
            }
        );

        if (it != flujosReales.end()) {
            TP.push_back(estimados.at(i));
        } else {
            FP.push_back(estimados.at(i));
        }
    }
    for (int i=0;i<flujosEstimados.size();i++) {
        int flujoReal = flujosReales.at(i);
        auto it = find_if(flujosEstimados.begin(), flujosEstimados.end(),
            [&flujoReal](const auto& flujoEstimado) {
                return flujoReal == flujoEstimado;
            }
        );

        if (it == flujosEstimados.end()) {
            FN.push_back(reales.at(i));
        }
    }


    cout << "TP: " << TP.size() << endl;
    // for(int i=0;i<TP.size();i++){
    //     cout << TP.at(i).first << "\t" << TP.at(i).second << endl;
    // }
    cout << "FP: " << FP.size() << endl;
    // for(int i=0;i<FP.size();i++){
    //     cout << FP.at(i).first << "\t" << FP.at(i).second << endl;
    // }


    return 0;
}