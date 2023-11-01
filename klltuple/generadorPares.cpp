// g++ generadorPares.cpp -o generadorPares
// ./generadorPares numParesAGenerar minElemento maxElemento minFlujo maxFlujo
// ./generadorPares 1000
// ./generadorPares 3000000 50000 900000000 1000 10000

#include <iostream>
#include <fstream>
#include <random>
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    int n = stoi(argv[1]);
    uint64_t minElement = stoull(argv[2]);
    uint64_t maxElement = stoull(argv[3]);
    uint64_t minFlujo = stoull(argv[4]);
    uint64_t maxFlujo = stoull(argv[5]);

    string nombreFichero = "pares"+to_string(n)+".txt";
    std::ofstream outFile(nombreFichero);

    if (!outFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return 1;
    }

    // Configurar un generador de números aleatorios
    std::default_random_engine generator(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<int> firstDistribution(minElement, maxElement);
    std::normal_distribution<> secondDistribution(maxFlujo+(minFlujo+maxFlujo)/2, (minFlujo+maxFlujo)/4);

    // Generar n pares aleatorios y escribirlos en el archivo
    for (int i = 0; i < n; ++i) {
        int64_t first = firstDistribution(generator);
        int64_t second = secondDistribution(generator);
        outFile << first << " " << second << std::endl;
    }

    outFile.close();

    std::cout << "Se generaron "<<n<<" pares aleatorios y se guardaron en " << nombreFichero << std::endl;

    return 0;
}
