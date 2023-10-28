// g++ obtenerHistograma.cpp -o obtenerHistograma
// ./obtenerHistograma archivoTraza
// ./obtenerHistograma Mendeley.txt
// ./obtenerHistograma Mawi-20181201.txt
// ./obtenerHistograma Chicago-20160121.txt
// ./obtenerHistograma Sanjose-20081016.txt

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

int main(int argc, char*argv[]){
    string ficheroEntrada = "datosHistogramaElementos";
    ficheroEntrada.append(argv[1]);
    std::ifstream inputFile(ficheroEntrada);
    if (!inputFile) {
        std::cerr << "No se pudo abrir el archivo " << ficheroEntrada << std::endl;
        return 1;
    }


    string ficheroSalida= "datosHistograma";
    ficheroSalida.append(argv[1]);

    std::ofstream outputFile(ficheroSalida);
    if (!outputFile) {
        std::cerr << "No se pudo abrir el archivo " << ficheroSalida << std::endl;
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    uint64_t newNumber = -1;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        uint64_t frecuencia, numero;
        
        if (!(iss >> frecuencia >> numero)) {
            std::cerr << "Error de formato en la línea: " << line << std::endl;
            continue;
        }

        newNumber++;

        lines.push_back(std::to_string(newNumber) + " " + std::to_string(frecuencia));
    }

    inputFile.close();

    string nombreTraza = argv[1];
    nombreTraza = nombreTraza.substr(0, nombreTraza.length() - 4); // eliminar "".txt"
    outputFile << "Distance " << nombreTraza << endl;
    for (const std::string& updatedLine : lines) {
        outputFile << updatedLine << std::endl;
    }

    outputFile.close();

    std::cout << "La conversión se ha completado con éxito." << std::endl;
    return 0;
}
