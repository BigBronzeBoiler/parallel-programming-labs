#include <iostream>
#include <vector>
#include <fstream>
#include <mpi.h>

using namespace std;

using ll = long long;

vector<ll> readMatrix(const string& filename, int n) {
    vector<ll> mat(n * n);
    ifstream file(filename);
    for (int i = 0; i < n * n; ++i) {
        file >> mat[i];
    }
    return mat;
}

void save_matrix(const string& filename, const vector<ll>& mat, int n) {
    ofstream file(filename);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            file << mat[i * n + j] << " ";
        }
        file << '\n';
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0) cerr << "Usage: " << argv[0] << " <matrix_size>\n";
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);

    vector<ll> A, B(n * n), C;

    if (rank == 0) {
        A = readMatrix("A.txt", n);
        B = readMatrix("B.txt", n);
        C.assign(n * n, 0LL);
    }

    // Рассылаем B всем процессам
    MPI_Bcast(B.data(), n*n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Распределение строк
    vector<int> sendcounts(size);
    vector<int> displs(size);
    int offset = 0;
    
    for (int i = 0; i < size; ++i) {
        int rows = n / size + (i < (n % size) ? 1 : 0);
        sendcounts[i] = rows * n;
        displs[i] = offset;
        offset += sendcounts[i];
    }

    int local_rows = sendcounts[rank] / n;
    vector<ll> local_A(sendcounts[rank]);
    vector<ll> local_C(sendcounts[rank], 0LL);

    // Раздаём части A
    MPI_Scatterv(rank == 0 ? A.data() : nullptr, 
                 sendcounts.data(), displs.data(), MPI_LONG_LONG,
                 local_A.data(), sendcounts[rank], MPI_LONG_LONG, 
                 0, MPI_COMM_WORLD);

    double start_time = 0.0;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // Умножение
    for (int i = 0; i < local_rows; ++i) {
        for (int k = 0; k < n; ++k) {
            ll temp = local_A[i * n + k];
            for (int j = 0; j < n; ++j) {
                local_C[i * n + j] += temp * B[k * n + j];
            }
        }
    }

    // Собираем результат
    MPI_Gatherv(local_C.data(), sendcounts[rank], MPI_LONG_LONG,
                rank == 0 ? C.data() : nullptr,
                sendcounts.data(), displs.data(), MPI_LONG_LONG,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        double end_time = MPI_Wtime();
        double duration = (end_time - start_time) * 1000000.0;

        save_matrix("result.txt", C, n);

        cout << "Size: " << n << "x" << n << '\n';
        cout << "Time: " << duration << " mcs" << '\n';
        cout << "Complexity: " << (long long)n * n * n << " operations" << '\n';
    }

    MPI_Finalize();
    return 0;
}