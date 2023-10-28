// g++ heaptest.cpp minheap.cpp minheap.hpp -o heaptest
// ./heaptest nombreFichero numNivelesHeap
// ./heaptest pares1000.txt 4

#include "minheap.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>

// Función para almacenar un par (suma, segundo) en un vector
void processPair(std::vector<std::pair<int64_t, int64_t>>& results, std::pair<int64_t, int64_t> pair) {
    for (auto& result : results) {
        if (result.second == pair.second) {
            result.first += pair.first;
            return;
        }
    }
    results.push_back(pair);
}

// Función para leer un archivo de pares y calcular la suma de los primeros componentes
std::vector<std::pair<int64_t, int64_t>> calculateSum(const std::string& fileName) {
    std::ifstream file(fileName);
    std::vector<std::pair<int64_t, int64_t>> results;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::pair<int64_t, int64_t> pair;
            std::istringstream iss(line);
            if (iss >> pair.first >> pair.second) {
                processPair(results, pair);
            }
        }
        file.close();
    } else {
        std::cerr << "No se pudo abrir el archivo: " << fileName << std::endl;
    }

    // Ordenar los resultados por la suma de los primeros componentes
    std::sort(results.begin(), results.end());

    return results;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_de_datos> <niveles>" << std::endl;
        return 1;
    }

    // Obtener el nombre del archivo de datos y la cantidad de niveles desde los argumentos
    std::string archivoDatos = argv[1];
    int niveles = std::stoi(argv[2]);

    // Crear una instancia de MinHeap con la cantidad de niveles especificada
    MinHeap minHeap(0, niveles);

    // Abrir el archivo de datos
    std::ifstream archivo(archivoDatos);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo de datos." << std::endl;
        return 1;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::pair<int64_t, int64_t> elemento;
        std::istringstream iss(linea);
        if (iss >> elemento.first >> elemento.second) {
            // Insertar el elemento en el Min Heap
            minHeap.insert(elemento);
        } else {
            std::cerr << "Error al leer la línea: " << linea << std::endl;
        }
    }

    archivo.close();

    minHeap.printMinHeap();

    // Obtener el vector de elementos del Min Heap
    std::vector<std::pair<int64_t, int64_t>> heap = minHeap.getHeap();

    // Comprobar si los elementos tienen la suma de sus primeros componentes correcta
    bool sumaCorrecta = true;
    for (const auto& par : heap) {
        int64_t suma = 0;
        for (const auto& otroPar : heap) {
            if (par.second == otroPar.second) {
                suma += otroPar.first;
            }
        }
        if (par.first != suma) {
            sumaCorrecta = false;
            break;
        }
    }

    if (sumaCorrecta) {
        std::cout << "La suma de los primeros componentes es correcta para todos los elementos." << std::endl;
    } else {
        std::cout << "La suma de los primeros componentes no es correcta para al menos un elemento." << std::endl;
    }

    std::vector<std::pair<int64_t, int64_t>> results = calculateSum(archivoDatos);

    std::cout << "Resultado (suma, segundo):" << std::endl;
    for (const auto& result : results) {
        std::cout << "(" << result.first << ", " << result.second << ")" << std::endl;
    }

    return 0;
}
