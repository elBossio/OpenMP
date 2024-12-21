#include <iostream>
#include <omp.h>
#include <vector>
#include <iomanip>

const int n = 100000000;

// функция для выполнения редукции с атомарными операциями
double reduction_atomic(const std::vector<double>& data, int num_threads) {
    double result = 0.0;
    omp_set_num_threads(num_threads);

#pragma omp parallel
    {
        double local_sum = 0.0;

#pragma omp for
        for (int i = 0; i < data.size(); ++i) {
            local_sum += data[i];
        }

#pragma omp atomic
        result += local_sum;
    }

    return result;
}

// функция для выполнения редукции с критической секцией
double reduction_critical(const std::vector<double>& data, int num_threads) {
    double result = 0.0;
    omp_set_num_threads(num_threads);
#pragma omp parallel
    {
        double local_sum = 0.0;
#pragma omp for
        for (int i = 0; i < n; ++i) {
            local_sum += data[i];
        }
#pragma omp critical
        {
            result += local_sum;
        }
    }
    return result;
}


// функция для выполнения редукции с замками
double reduction_lock(const std::vector<double>& data, int num_threads) {
    double result = 0.0;
    omp_set_num_threads(num_threads);
    omp_lock_t lock;
    omp_init_lock(&lock);

    #pragma omp parallel
    {
        double local_sum = 0.0;
        #pragma omp for
        for (int i = 0; i < data.size(); ++i) {
            local_sum += data[i];
        }
        omp_set_lock(&lock);
        result += local_sum;
        omp_unset_lock(&lock);
    }
    omp_destroy_lock(&lock);
    return result;
}


// функция для выполнения редукции с использованием директивы reduction
double reduction_openmp(const std::vector<double>& data, int num_threads) {
    double result = 0.0;
    omp_set_num_threads(num_threads);
#pragma omp parallel for reduction(+:result)
    for (int i = 0; i < n; ++i) {
        result += data[i];
    }
    return result;
}

int main() {
    std::vector<double> data(n, 1.0);

    double start_time, end_time;

    for (int num_threads : {1, 2, 4, 8, 12, 16, 32, 64, 128})
    {
        std::cout << "num threads: " << num_threads << "\n";

        start_time = omp_get_wtime();
        double result_atomic = reduction_atomic(data, num_threads);
        end_time = omp_get_wtime();
        std::cout << "atomic reduction time: " << end_time - start_time << " seconds\n";
        //std::cout << "atomuc result = " << std::fixed << std::setprecision(2) << result_atomic << "\n";

        start_time = omp_get_wtime();
        double result_critical = reduction_critical(data, num_threads);
        end_time = omp_get_wtime();
        std::cout << "critical section reduction time: " << end_time - start_time << " seconds\n";
        //std::cout << "critical result = " << std::fixed << std::setprecision(2) << result_critical << "\n";

        start_time = omp_get_wtime();
        double result_lock = reduction_lock(data, num_threads);
        end_time = omp_get_wtime();
        std::cout << "lock reduction time: " << end_time - start_time << " seconds\n";
        //std::cout << "lock result = " << std::fixed << std::setprecision(2) << result_lock << "\n";

        start_time = omp_get_wtime();
        double result_openmp = reduction_openmp(data, num_threads);
        end_time = omp_get_wtime();
        std::cout << "openmp reduction time: " << end_time - start_time << " seconds\n";
        //std::cout << "reduction result = " <<std::fixed << std::setprecision(2) << result_openmp << "\n" << "\n";
    }

    return 0;
}
