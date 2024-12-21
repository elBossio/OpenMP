#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;

int main() {

    const int N = 100000000;  
    // Инициализация генератора случайных чисел
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::vector<int> A(N);
    std::vector<int> B(N);

    for (int i = 0; i < N; i++) {
        A[i] = std::rand() % 10 + 1;
        B[i] = std::rand() % 10 + 1;
    }

    long long scalar_product;

    for (int num_threads : {1, 2, 4, 8, 12, 16, 20, 32}) {

        omp_set_num_threads(num_threads);

        auto start = chrono::high_resolution_clock::now();

        // Вычисление скалярного произведения
        scalar_product = 0;
#pragma omp parallel for reduction(+:scalar_product)
        for (int i = 0; i < N; ++i) {
            scalar_product += A[i] * B[i];
        }

        auto end = chrono::high_resolution_clock::now();
        // auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        std::chrono::duration<double> elapsed_time = end - start;

        cout << "Number of threads: " << num_threads
            << ", Time: " << elapsed_time.count() << "sec" << endl;
    }

    return 0;
}
