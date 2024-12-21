#include <iostream>
#include <omp.h>
#include <vector>
#include <random>
#include <chrono>

// Функция для имитации неравномерной нагрузки
void heavy_computation(int iter) {
    volatile double result = 0;
    for (int i = 0; i < iter * 1000; ++i) {
        result += i * 0.001;
    }
}

int main() {
    const int num_iterations = 100000;
    std::vector<int> workload(num_iterations);

    std::cout << "Num_iteration: " << num_iterations << "\n";

    // Генерация случайной нагрузки для каждой итерации
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    for (int& w : workload) {
        w = dis(gen);
    }

    for (int num_threads : {4, 8, 12, 16, 20}) {

        std::cout << "Number of threads: " << num_threads << "\n";
        omp_set_num_threads(num_threads);
        // Исследование режимов распределения
        std::vector<std::string> schedules = { "static", "dynamic", "guided" };

        for (const auto& schedule : schedules) {
            double start_time = omp_get_wtime();

            if (schedule == "static") {
#pragma omp parallel for schedule(static)
                for (int i = 0; i < num_iterations; ++i) {
                    heavy_computation(workload[i]);
                }
            }
            else if (schedule == "dynamic") {
#pragma omp parallel for schedule(dynamic)
                for (int i = 0; i < num_iterations; ++i) {
                    heavy_computation(workload[i]);
                }
            }
            else if (schedule == "guided") {
#pragma omp parallel for schedule(guided)
                for (int i = 0; i < num_iterations; ++i) {
                    heavy_computation(workload[i]);
                }
            }

            double end_time = omp_get_wtime();
            std::cout << "Schedule: " << schedule
                << ", Execution Time: " << (end_time - start_time) << " seconds\n";
        }

    }
    return 0;
}
