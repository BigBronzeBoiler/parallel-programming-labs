#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

typedef vector<vector<double>> Matrix;

vector<vector<double>> readMatrix(const string& filename, int n) {
    vector<vector<double>> mat(n, vector<double>(n));
    ifstream file(filename);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            file >> mat[i][j];
    return mat;
}

void save_matrix(const string& filename, const  vector<vector<double>>& mat, int n) {
    ofstream file(filename);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            file << mat[i][j] << " ";
        file << '\n';
    }
}

int main(int argc, char* argv[]) {

    int n = atoi(argv[1]);

    vector<vector<double>> A = readMatrix("A.txt", n);
    vector<vector<double>> B = readMatrix("B.txt", n);
    vector<vector<double>> C(n, vector<double>(n, 0.0));

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            double temp = A[i][k];
            for (int j = 0; j < n; ++j) {
                C[i][j] += temp * B[k][j];
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    save_matrix("result.txt", C, n);

    cout << "Size: " << n << "x" << n << '\n';
    cout << "Time: " << duration.count() << " mcs" << '\n';
    cout << "Complexity: " << (long long)n * n * n << " operations" << '\n';

    return 0;
}