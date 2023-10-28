// g++ reduction.cpp -o reduction
// ./reduction

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

bool comparar_pares(const pair<int64_t, int64_t>& a, const pair<int64_t, int64_t>& b) {
    return a.second < b.second;
}

void procesar_vector(vector<pair<int64_t, int64_t>>& v) {
    stable_sort(v.begin(), v.end(), comparar_pares);

    for (int i = 0; i < v.size() - 1; i++) {
        if (v[i].second == v[i + 1].second) {
            v[i].first += v[i + 1].first;
            v[i + 1] = make_pair(-1, -1);
        }
    }
}

int main() {
    vector<pair<int64_t, int64_t>> v = {{1, 2}, {3, 4}, {5, 4}, {7, 8}, {9, 8}};
    procesar_vector(v);

    for (int i = 0; i < v.size(); i++) {
        cout << "(" << v[i].first << ", " << v[i].second << ") ";
    }

    return 0;
}
