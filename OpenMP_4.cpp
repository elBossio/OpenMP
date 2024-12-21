#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>
#include <chrono>

using namespace std;

int Generator(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main() {
    srand(time(0));
    int N = 10000;
    vector<vector<int>>matrix(N, vector<int>(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = Generator(1, 10);
        }
    }
   
    for (int num_threads : {1, 2, 4, 8, 12, 16, 20, 32, 40, 52, 64}) {
        int result = INT_MIN;
        omp_set_num_threads(num_threads);

        //double start_time = omp_get_wtime();
        auto start_time = chrono::high_resolution_clock::now();

#pragma omp parallel for reduction(max:result)
        for (int i = 0; i < N; i++) {
            int min_in_row = matrix[i][0];
            for (int j = 1; j < N; j++) {
                if (matrix[i][j] < min_in_row) {
                    min_in_row = matrix[i][j];
                }
            }
            result = max(result, min_in_row);
        }

        //double end_time = omp_get_wtime();
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end_time - start_time;

        cout << "Num of threads: " << num_threads 
            << ", Time: " << elapsed_time.count() << " sec" << endl;
    }
    
    return 0;
}
