#include <iostream>
#include <vector>
#include <omp.h>
#include <limits>
#include <chrono>

using namespace std;

void GenerateBandMatrix(vector<vector<int>>& matrix, int bandwidth) {
    int N = matrix.size();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (abs(i - j) <= bandwidth - 1) {
                matrix[i][j] = rand() % 10 + 1;
            }
            else {
                matrix[i][j] = 0;
            }
        }
    }
}

void GenerateLowerTriangularMatrix(vector<vector<int>>& matrix) {
    int N = matrix.size();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

int main() {
    srand(time(0));
    int N = 20000;
    vector<vector<int>> matrix(N, vector<int>(N));
    double start_time, end_time;

    //GenerateBandMatrix(matrix, 5); // Ленточная матрица с шириной полосы = 3
    GenerateLowerTriangularMatrix(matrix); // Нижнетреугольная матрица

    for (int num_threads : {1, 2, 4, 8, 12, 16, 32}) {
        int result = INT_MIN;
        omp_set_num_threads(num_threads);
        cout << "Threads: " << num_threads<<endl;

        start_time = omp_get_wtime();

        // Изменение правила распределения итераций
#pragma omp parallel for schedule(static) reduction(max:result)
        for (int i = 0; i < N; i++) {
            int min_in_row = INT_MAX;
            for (int j = 0; j < N; j++) {
                if (matrix[i][j] > 0 && matrix[i][j] < min_in_row) {
                    min_in_row = matrix[i][j];
                }
            }
            if (min_in_row != INT_MAX) {
                result = max(result, min_in_row);
            }
        }

        end_time = omp_get_wtime();

        cout << "static" << ", Time: " << end_time - start_time << " sec" << endl;

        start_time = omp_get_wtime();

#pragma omp parallel for schedule(dynamic) reduction(max:result)
        for (int i = 0; i < N; i++) {
            int min_in_row = INT_MAX;
            for (int j = 0; j < N; j++) {
                if (matrix[i][j] > 0 && matrix[i][j] < min_in_row) {
                    min_in_row = matrix[i][j];
                }
            }
            if (min_in_row != INT_MAX) {
                result = max(result, min_in_row);
            }
        }

        end_time = omp_get_wtime();

        cout << "dynamic" << ", Time: " << end_time - start_time << " sec" << endl;

        start_time = omp_get_wtime();

#pragma omp parallel for schedule(guided) reduction(max:result)
        for (int i = 0; i < N; i++) {
            int min_in_row = INT_MAX;
            for (int j = 0; j < N; j++) {
                if (matrix[i][j] > 0 && matrix[i][j] < min_in_row) {
                    min_in_row = matrix[i][j];
                }
            }
            if (min_in_row != INT_MAX) {
                result = max(result, min_in_row);
            }
        }

        end_time = omp_get_wtime();

        cout << "guided" << ", Time: " << end_time - start_time << " sec" << endl;
    }

    return 0;
}
