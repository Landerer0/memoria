#include <iostream>
#include <vector>

using namespace std;

class KLL{
    public:
        KLL(unsigned long,double,double, double);
        ~KLL();
        std::hash<long> hashLong;

        // Operaciones asociadas al problema y funcionalidad general

        bool sample(long element); // indica si el elemento es seleccionado al samplear
        bool murmurHashSample(long element); // indica si el elemento es seleccionado al samplear
        bool reservoirKLLSample(long element, long elementWeight);

        void add(double element); // agregar element al sketch
        void addv(double element); // agregar element al sketch
        unsigned long rank(double element); // indica el rank del elemento proporcionado
        long select(long rank); // retorna el elemento cuyo rank es el indicado
        long quantile(double q); // retorna elemento encontrado en el quantil q

        // Operaciones para realizar merge
        long height();
        pair<vector<double>, long> sketchAtLevel(long nivel);
        bool sortedAtLevel(long nivel);
        void update(KLL kll2);
        void setSeconds(vector<long> seconds);

        // Operaciones auxiliares
        long sizeInBytes();
        vector<double> parametros();
        void print(); // imprime arreglos
        KLL kllMerge(KLL &kll2);

    private:
        // variables para reservoirKLLSample
        unsigned long sampleWeight;
        double sampleElement;

        unsigned int numArreglos;
        unsigned int numArreglosOcupados;
        vector<pair<vector<double>, long> > sketch; // arreglo de arreglos con tamaño decreciente
            // sketch[i].first almacena los vectores donde se almacenan los elementos de nivel i
            // sketch[i].second mantiene el num de elementos ocupados en dicho nivel i
        vector<bool> sorted; // indica si el sketch[i] se encuentra ordenado
        
        // Operaciones
        void insertElement(long nivel,double &element);
        void insertCompactionElement(long nivel,double &element,bool updating);
        void compaction(long nivel,bool updating);

        // variables k y c son ctes. entregadas por el usuario, c esta en rango ]0.5,1[
        unsigned long long n; // si bien no pertenece a la cota espacial, es necesario para determinar H
        unsigned long long numElementosRevisados, numTotalElementos; 
        double epsilon, delta, c;
        long H, H_p, H_pp;
        long k, s;
        unsigned long mascara;
        long wH_pp;

        bool debug = false;

        void compaction(int level);
        KLL copy();
        KLL(unsigned long,double,double, unsigned long long, KLL*);
};
