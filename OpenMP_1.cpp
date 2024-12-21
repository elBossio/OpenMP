#include <iostream>
#include <vector>
#include <cstdlib>
#include <omp.h>
#include <limits>

void find_min_with_reduction(const std::vector<int>& arr, int num_threads) {
    omp_set_num_threads(num_threads);
    int min_val = std::numeric_limits<int>::max();
    double start_time = omp_get_wtime();

#pragma omp parallel for reduction(min:min_val)
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Threads: " << num_threads
        << ", Time (with reduction): " << end_time - start_time << " seconds" << std::endl;
}

void find_min_without_reduction(const std::vector<int>& arr, int num_threads) {
    omp_set_num_threads(num_threads);
    int min_val = std::numeric_limits<int>::max();
    double start_time = omp_get_wtime();

#pragma omp parallel
    {
        int local_min = std::numeric_limits<int>::max();

#pragma omp for
        for (size_t i = 0; i < arr.size(); i++) {
            if (arr[i] < local_min) {
                local_min = arr[i];
            }
        }

#pragma omp critical
        {
            if (local_min < min_val) {
                min_val = local_min;
            }
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Threads: " << num_threads
        << ", Time (without reduction): " << end_time - start_time << " seconds" << std::endl;
}

int main() {
    size_t n = 10000000; // Размер массива
    std::vector<int> arr(n);

    // Инициализация массива случайными значениями
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (size_t i = 0; i < n; i++) {
        arr[i] = std::rand() % 10000;
    }

    for (int threads = 1; threads <= 32; threads *= 2) {
        find_min_with_reduction(arr, threads);
        find_min_without_reduction(arr, threads);
    }

    return 0;
}
