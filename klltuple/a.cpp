#include <iostream>
#include <vector>

// Función para mover elementos (-1, -1) al final del vector
void moveNegativeOnesToEnd(std::vector<std::pair<int64_t, int64_t>>& pairs) {
    int n = pairs.size();
    int i = 0;  // Índice para recorrer el vector

    // Recorre el vector y mueve los elementos (-1, -1) al final
    for (int j = 0; j < n; j++) {
        if (pairs[j].first != -1 || pairs[j].second != -1) {
            pairs[i] = pairs[j];
            i++;
        }
    }

    // Llena el final del vector con elementos (-1, -1)
    while (i < n) {
        pairs[i] = { -1, -1 };
        i++;
    }
}

int main() {
    std::vector<std::pair<int64_t, int64_t>> pairs = {{-1, -1}, {1, 5}, {-1,-1}, {2, 6}, {1, 3}, {3, 8}, {-1, -1}, {-1, -1}};

    moveNegativeOnesToEnd(pairs);

    std::cout << "Vector con elementos (-1, -1) al final: ";
    for (const auto& p : pairs) {
        std::cout << "(" << p.first << ", " << p.second << ") ";
    }

    std::cout << std::endl;

    return 0;
}
