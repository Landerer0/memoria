// g++ resumenEjecucion.cpp -o resumenEjecucion
// ./resumenEjecucion nombreArchivo
// ./resumenEjecucion Mendeley
// ./resumenEjecucion Chicago-20160121
// ./resumenEjecucion Sanjose-20081016
// ./resumenEjecucion Mawi-20181201

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>

using namespace std;

// Método para leer filas de un archivo y almacenar los datos en un vector<double>
std::vector<std::vector<double>> leerArchivoYAlmacenarDatos(string nombreArchivo) {
    std::vector<std::vector<double>> datos; // Vector de vectores para almacenar los datos

    std::ifstream archivo(nombreArchivo); // Abrir el archivo

    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return datos; // Devuelve un vector vacío si no se pudo abrir el archivo
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::vector<double> fila; // Vector para almacenar los datos de una fila

        std::istringstream ss(linea);
        double valor;
        while (ss >> valor) {
            fila.push_back(valor);
        }

        datos.push_back(fila); // Agregar la fila al vector de datos
    }

    archivo.close(); // Cerrar el archivo

    return datos;
}

std::vector<std::vector<double>> calcularTranspuesto(const std::vector<std::vector<double>>& matriz) {
    if (matriz.empty()) {
        return std::vector<std::vector<double>>(); // Devolver una matriz vacía si la matriz de entrada está vacía
    }

    size_t filas = matriz.size();
    size_t columnas = matriz[0].size();

    std::vector<std::vector<double>> transpuesta(columnas, std::vector<double>(filas, 0.0));

    for (size_t i = 0; i < filas; i++) {
        for (size_t j = 0; j < columnas; j++) {
            transpuesta[j][i] = matriz[i][j];
        }
    }

    return transpuesta;
}

double calcularMedia(const std::vector<double>& numeros) {
    double suma = 0.0;
    for (const double numero : numeros) {
        suma += numero;
    }
    return suma / static_cast<double>(numeros.size());
}

double calcularDesviacionEstandar(const std::vector<double>& numeros) {
    double media = calcularMedia(numeros);
    double sumaCuadradosDiferencia = 0.0;

    for (const double numero : numeros) {
        sumaCuadradosDiferencia += (numero - media) * (numero - media);
    }

    double varianza = sumaCuadradosDiferencia / static_cast<double>(numeros.size());
    return std::sqrt(varianza);
}

void almacenarDatos(string filenameEspecifications, vector<double> &parametrosKLL, int numRepeticiones, vector<double> &consultaCuantiles
                    , vector<double> rankMean, vector<double> rankStd, vector<double> quantileMean, vector<double> quantileStd
                    , vector<double> tiempoMean, vector<double> tiempoStd,vector<double> tiemposExtraMean, vector<double> tiemposExtraStd){
    // se define ubicacion resultante de los archivos generados por las pruebas
    std::string nombreCarpeta = "resultados/";
    string nombreIteracion = "";
    // Nombre del archivo de salida
    std::string filename = nombreCarpeta+filenameEspecifications+"Generales"+to_string(numRepeticiones)+".txt";

    // Abrir el archivo en modo de escritura y almacenar los resultados en distintos archivos
    std::ofstream outfile(filename);
    if (outfile.is_open()) { // parametros (N, epsilon, rango, entre otros)
        for (const auto& element : parametrosKLL) {
            outfile << element << " ";
        }
        for (const auto& element : tiemposExtraMean) {
            outfile << element << " ";
        }
        for (const auto& element : tiemposExtraStd) {
            outfile << element << " ";
        }
        outfile << "\n";
        std::cout << "Los datos Parametros se han almacenado correctamente en el archivo " << filename << std::endl;
        
        for (const auto& element : consultaCuantiles) {
            outfile << element << " ";
        }
        outfile << "\n";

        std::cout << "Los datos Consulta se han almacenado correctamente en el archivo " << filename << std::endl;
    
        
        for (const auto& element : rankMean) {
            outfile << element << " ";
        }
        outfile << "\n";
        for (const auto& element : rankStd) {
            outfile << element << " ";
        }
        outfile << "\n";
        for (const auto& element : quantileMean) {
            outfile << element << " ";
        }
        outfile << "\n";
        for (const auto& element : quantileStd) {
            outfile << element << " ";
        }
        outfile << "\n";

        for (const auto& element : tiempoMean) {
            outfile << element << " ";
        }
        for (const auto& element : tiempoStd) {
            outfile << element << " ";
        }
        outfile << "\n";
        
        outfile.close();
    }
}

void crearResumen(string archivo, string nombreArchivo, int numRepeticiones){
    string nombreCarpeta = "resultados/";
    string nombreFichero = nombreCarpeta+nombreArchivo;

    vector<double> parametrosFinales;
    vector<vector<double>> tiemposExtras;
    vector<double> consultaCuantil;
    vector<vector<double>> ranks;
    vector<vector<double>> quantiles;
    vector<vector<double>> tiempo;

    for(int iteracion = 1;iteracion<=numRepeticiones;iteracion++){
        string fichero = nombreFichero;
        fichero.append(to_string(iteracion));
        fichero.append(".txt");

        std::vector<std::vector<double>> datos = leerArchivoYAlmacenarDatos(fichero);
        if(iteracion==1){
            parametrosFinales = datos.at(0);
            parametrosFinales.pop_back();
            parametrosFinales.pop_back();
            parametrosFinales.pop_back();
            consultaCuantil = datos.at(1);
        }
        vector<double> temp;
        for(int i=datos.at(0).size()-3;i<datos.at(0).size();i++){
            temp.push_back(datos.at(0).at(i));
        }
        tiemposExtras.push_back(temp);
        ranks.push_back(datos.at(2));
        quantiles.push_back(datos.at(3));
        tiempo.push_back(datos.at(4));
    }


    // procesar los datos
    vector<double> rankMean;
    vector<double> rankStd;
    vector<double> quantileMean;
    vector<double> quantileStd;
    vector<double> timeMean;
    vector<double> timeStd;
    vector<double> tiemposExtrasMean;
    vector<double> tiemposExtrasStd;

    tiemposExtras = calcularTranspuesto(tiemposExtras);
    ranks = calcularTranspuesto(ranks);
    quantiles = calcularTranspuesto(quantiles);
    tiempo = calcularTranspuesto(tiempo);


    // Calcular medias y desviaciones estándar para cada vector
    for (const std::vector<double>& fila : ranks) {
        rankMean.push_back(calcularMedia(fila));
        rankStd.push_back(calcularDesviacionEstandar(fila));
    }
    for (const std::vector<double>& fila : quantiles) {
        quantileMean.push_back(calcularMedia(fila));
        quantileStd.push_back(calcularDesviacionEstandar(fila));
    }
    for (const std::vector<double>& fila : tiempo) {
        timeMean.push_back(calcularMedia(fila));
        timeStd.push_back(calcularDesviacionEstandar(fila));
    }
    for (const std::vector<double>& fila : tiemposExtras) {
        tiemposExtrasMean.push_back(calcularMedia(fila));
        tiemposExtrasStd.push_back(calcularDesviacionEstandar(fila));
    }

    almacenarDatos( nombreArchivo, parametrosFinales, numRepeticiones, consultaCuantil
                    , rankMean, rankStd, quantileMean, quantileStd
                    , timeMean, timeStd, tiemposExtrasMean, tiemposExtrasStd);

}

int main(int argc, char* argv[]) {
    int numRepeticiones = 100;
    string archivo = argv[1];

    vector<string> eps = {}; // eps short
    vector<string> epsilon = {};
    vector<string> delta = {"0.000000"};

    eps.push_back("0.01");
    epsilon.push_back("0.010000");

    eps.push_back("0.0075");
    epsilon.push_back("0.007500");

    eps.push_back("0.006");
    epsilon.push_back("0.006000");

    eps.push_back("0.005");
    epsilon.push_back("0.005000");

    eps.push_back("0.004");
    epsilon.push_back("0.004000");

    eps.push_back("0.003");
    epsilon.push_back("0.003000");

    eps.push_back("0.0025");
    epsilon.push_back("0.002500");

    eps.push_back("0.002");
    epsilon.push_back("0.002000");
        
    for(int i=0;i<epsilon.size();i++){
        for(int j=0;j<delta.size();j++){
            std::string nombreArchivo;
            nombreArchivo = nombreArchivo+"epsilonDetermined";
            nombreArchivo = nombreArchivo+eps.at(i);
            nombreArchivo = nombreArchivo+"datospreprocesado";
            nombreArchivo = nombreArchivo+archivo;
            nombreArchivo = nombreArchivo+"e";
            nombreArchivo = nombreArchivo+epsilon.at(i);
            nombreArchivo = nombreArchivo+"d";
            nombreArchivo = nombreArchivo+delta.at(j);
            nombreArchivo = nombreArchivo+"Resultados";

            crearResumen(archivo, nombreArchivo, 100);

        }

        std::string nombreArchivomrl;
        nombreArchivomrl = nombreArchivomrl + "epsilonDetermined";
        nombreArchivomrl = nombreArchivomrl + eps.at(i);
        nombreArchivomrl = nombreArchivomrl + "datospreprocesado";
        nombreArchivomrl = nombreArchivomrl + archivo;
        nombreArchivomrl = nombreArchivomrl + "kminResultados";
        crearResumen(archivo, nombreArchivomrl, 100);

    }
    

    

    return 0;
}
