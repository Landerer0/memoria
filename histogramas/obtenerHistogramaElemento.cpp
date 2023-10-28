// g++ obtenerHistogramaElemento.cpp -o obtenerHistogramaElemento
// ./obtenerHistogramaElemento archivoTraza
// ./obtenerHistogramaElemento Mendeley.txt
// ./obtenerHistogramaElemento Mawi-20181201.txt
// ./obtenerHistogramaElemento Chicago-20160121.txt
// ./obtenerHistogramaElemento Sanjose-20081016.txt

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

int main(int argc, char*argv[]){
    ifstream inputFile(argv[1]);
    if (!inputFile) {
        cerr << "No se pudo abrir el archivo de entrada." << endl;
        return 1;
    }

    unordered_map<uint64_t, uint64_t> frequencyMap;
    uint64_t number;

    // Leer el archivo y contar la frecuencia de cada elemento
    while (inputFile >> number) {
        frequencyMap[number]++;
    }

    // Crear un vector de pares (frecuencia, elemento) para ordenar
    vector<pair<uint64_t, uint64_t>> sortedElements;

    for (const auto& entry : frequencyMap) {
        sortedElements.push_back(make_pair(entry.second, entry.first));
    }

    // Ordenar el vector en orden descendente de frecuencia
    sort(sortedElements.rbegin(), sortedElements.rend());

    // Abrir el archivo de salida
    string nombreSalida = "datosHistogramaElemento";
    nombreSalida.append(argv[1]);
    ofstream outputFile(nombreSalida);
    if (!outputFile) {
        cerr << "No se pudo abrir el archivo de salida." << endl;
        return 1;
    }

    // Escribir los resultados en el archivo de salida
    for (const auto& entry : sortedElements) {
        outputFile << entry.first << " " << entry.second << endl;
    }

    cout << "Los resultados se han guardado en "<< nombreSalida<<" ." << endl;

    return 0;
}
