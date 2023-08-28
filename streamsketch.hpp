#include <iostream>
#include <vector>
#include "kll.hpp"

using namespace std;

class StreamSketch{
    public:
        // Constructores por defecto
        StreamSketch(uint64_t numElementsParam, double epsilonParam, double deltaParam, double cParam, int minKp); // KLL Tradicional
        //StreamSketch(uint64_t espacioMaximo, uint64_t numElementosParam, double cParam, int minKp, uint32_t maxKp); // KLL Tradicional con Espacio Maximo dado para abarcar un numElementosParam determinado
        StreamSketch(uint64_t numElementsParam, double epsilonParam, double cParam, int minKp); // KLL Tradicional que determina espacio a ocupar según epsilon entregado
        StreamSketch(uint64_t minKP); // MRL
        StreamSketch(uint64_t espacioMaximo, uint64_t numElementosParam); // MRL con Espacio Maximo dado para abarcar un numElementosParam determinado
        StreamSketch(double epsilonParam, uint64_t numElementosParam); // MRL que determina espacio a ocupar según epsilon entregado

        ~StreamSketch();

        void add(double element); // agregar element al sketch, retorna true cuando sketch se llenó tras la inserción
        uint64_t rank(double element); // indica el rank del elemento proporcionado
        vector<uint64_t> rank(vector<double> elements); 
        double select(uint64_t rank); // retorna el elemento cuyo rank es el indicado
        pair<double, bool> selectMinMax(uint64_t rank);
        vector<double> select(vector<uint64_t> rank); 
        vector<pair<double,bool>> selectMinMax(vector<uint64_t> rank); 
        double quantile(double q); // retorna elemento encontrado en el quantil q
        vector<double> quantile(vector<double> q); 

        void merge();
        uint64_t numElementosRango(double a, double b);

        // Operaciones auxiliares
        bool isAnMrl();
        pair<uint64_t,uint64_t> getNumElementsSeen(); 
        pair<double, double> getMinMaxElement();
        uint64_t sizeInBytes();
        void print(); // imprime arreglos
        void printNumSketches();
        bool hasLimitedSpace();
        vector<double> parametros();

        uint64_t saveData(string outputFileName); // retorna el numero de bytes ocupados en el archivo binario
        void addKLL(KLL kllToInsert);
    private:
        void setupStreamSketch(short typeKLLParam);
        void addSketch();
        KLL mergeSketches();

        vector<KLL> sketches;
        short typeKLL; // indica el kll que se esta creando, usado para crear posteriores kll a medida que sea necesario
        uint64_t numElements;
        double epsilon,delta,c;
        int minK;
        uint64_t espacioMaximo;
        uint64_t sketchActual; // indica a que sketch hay que ingresar elementos
};
