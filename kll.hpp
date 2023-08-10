#include <iostream>
#include <vector>

using namespace std;

class KLL{
    public:
        // Constructores por defecto
        KLL(uint64_t numElements, double epsilonParam, double deltaParam, double cParam, int minKp); // KLL Tradicional
        KLL(uint64_t espacioMaximo, uint64_t numElementosParam, double cParam, int minKp, uint32_t maxKp); // KLL Tradicional con Espacio Maximo dado para abarcar un numElementosParam determinado
        KLL(uint64_t numElementosParam, double epsilon, double cParam, int minKp); // KLL Tradicional que determina espacio a ocupar según epsilon entregado
        KLL(uint64_t minKP); // MRL
        KLL(uint64_t espacioMaximo, uint64_t numElementosParam); // MRL con Espacio Maximo dado para abarcar un numElementosParam determinado
        KLL(double epsilonEntregado, uint64_t numElementosParam); // MRL que determina espacio a ocupar según epsilon entregado

        ~KLL();
        std::hash<long> hashLong;

        // Operaciones asociadas al problema y funcionalidad general

        bool sample(double element); // indica si el elemento es seleccionado al samplear
        bool murmurHashSample(double element); // indica si el elemento es seleccionado al samplear
        bool reservoirKLLSample(double element, uint64_t elementWeight);

        bool add(double element); // agregar element al sketch
        void add(double element, uint32_t elementWeight); // agregar element al sketch
        void addv(double element); // agregar element al sketch
        uint64_t rank(double element); // indica el rank del elemento proporcionado
        vector<uint64_t> rank(vector<double> elements); 
        double select(uint64_t rank); // retorna el elemento cuyo rank es el indicado
        pair<double, bool> selectMinMax(uint64_t rank);
        vector<double> select(vector<uint64_t> rank); 
        vector<pair<double,bool>> selectMinMax(vector<uint64_t> rank); 
        double quantile(double q); // retorna elemento encontrado en el quantil q
        vector<double> quantile(vector<double> q); 

        // Operaciones para realizar merge
        long height();
        pair<vector<double>, long> sketchAtLevel(long nivel);
        void update(KLL kll2);
        void setSeconds(vector<long> seconds);
        KLL kllMerge(KLL &kll2);
        uint64_t numElementosRango(double a, double b);

        // Operaciones auxiliares
        bool isAnMrl();
        uint32_t getH();
        uint32_t getH_pp();
        uint32_t getFirstLevelK();
        pair<uint64_t,uint64_t> getNumElementsSeen(); 
        pair<double, double> getMinMaxElement();
        pair<unsigned long, double> getCurrentSample();
        uint64_t sizeInBytes();
        vector<double> parametros();
        void print(); // imprime arreglos
        bool areEqual(KLL);

        uint64_t saveData(string outputFileName); // retorna el numero de bytes ocupados en el archivo binario
        KLL readData(string inputFileName); // retorna estructura asociado al archivo proporcionado

    private:
        bool isMrl;
        double minElement;
        double maxElement;

        // variables para reservoirKLLSample
        unsigned long sampleWeight;
        double sampleElement;
        uint32_t minK;

        unsigned int numArreglos;
        vector<pair<vector<double>, long> > sketch; // arreglo de arreglos con tamaño decreciente
            // sketch[i].first almacena los vectores donde se almacenan los elementos de nivel i
            // sketch[i].second mantiene el num de elementos ocupados en dicho nivel i
        
        // Operaciones
        void addToSampler(double element,uint64_t weight);
        void insertElement(long nivel,double &element);
        void insertCompactionElement(long nivel,double &element,bool updating);
        bool compaction(long nivel,bool updating);

        // variables k y c son ctes. entregadas por el usuario, c esta en rango ]0.5,1[
        uint64_t n; // si bien no pertenece a la cota espacial, es necesario para determinar H
        uint64_t numElementosRevisados, numTotalElementos; 
        double epsilon, delta, c;
        long H, H_p, H_pp;
        long k, s;
        uint64_t mascara;
        long wH_pp;

        bool debug = false;

        void compaction(int level);
        // operaciones para realizar merge de la estructura
        KLL copy();
        KLL(uint64_t nCopy, double epsilonCopy, double deltaCopy, double cCopy, uint32_t minKCopy, uint64_t numElementosRevisadosCopy, uint64_t numTotalElementosCopy, KLL*); // KLL Tradicional
        KLL(uint32_t mrlKminCopy, uint64_t numTotalElementosCopy, KLL*); // MRL
        

        // para readData
        KLL(uint32_t minKRead, uint32_t numElementosRevisadosRead, vector<vector<double>> niveles, double minElementRead, double maxElementRead);        
        KLL(uint64_t nRead, double epsilonRead,double deltaRead,double cRead, uint32_t minKRead,uint64_t numTotalElementosRead,unsigned long sampleWeightRead,double sampleElementRead, uint64_t numElementosRevisadosRead,vector<vector<double>> niveles, double minElementRead, double maxElementRead);

};
