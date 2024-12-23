#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

std::vector<long long> generateRandomVector(long long size) {
    std::vector<long long> vec(size);

    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<long long> dis(0, 100);

    for (long long& val : vec) {
        val = dis(gen);
    }
    return vec;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    std::vector<long long> vectorSizes = { 1000, 10000, 100000, 1000000, 10000000, 100000000, 500000000, 1000000000 };

    for (long long VECTOR_SIZE : vectorSizes) {
        if (rank == 0) {
            std::cout << "\nVector size: " << VECTOR_SIZE << std::endl;
        }

        std::vector<long long> fullVector;
        std::vector<long long> localVector;
        int localSize, remainder;

        double startTime, endTime;

        if (rank == 0) {
            fullVector = generateRandomVector(VECTOR_SIZE);

            localSize = VECTOR_SIZE / size;
            remainder = VECTOR_SIZE % size;
        }

        MPI_Bcast(&localSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&remainder, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Вычисление числа элементов, которые должен получить текущий процесс
        int startIdx = rank * localSize + std::min(rank, remainder);
        int endIdx = startIdx + localSize + (rank < remainder ? 1 : 0);

        localVector.resize(endIdx - startIdx);

        if (rank == 0) {
            for (int i = 1; i < size; ++i) {
                int startIdxOther = i * localSize + std::min(i, remainder);
                int endIdxOther = startIdxOther + localSize + (i < remainder ? 1 : 0);
                MPI_Send(&fullVector[startIdxOther], endIdxOther - startIdxOther, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
            }

            localVector.assign(fullVector.begin(), fullVector.begin() + localVector.size());
        }
        else {
            MPI_Recv(localVector.data(), localVector.size(), MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Синхронизация процессов и запуск таймера
        MPI_Barrier(MPI_COMM_WORLD);
        startTime = MPI_Wtime();

        // int localMin = *std::min_element(localVector.begin(), localVector.end());
        int localMax = *std::max_element(localVector.begin(), localVector.end());

        // int globalMin;
        int globalMax;

        // MPI_Reduce(&localMin, &globalMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
        MPI_Reduce(&localMax, &globalMax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

        // Остановка таймера
        MPI_Barrier(MPI_COMM_WORLD);
        endTime = MPI_Wtime();

        if (rank == 0) {
            // std::cout << "Minimum value: " << globalMin << std::endl;
            // std::cout << "Maximum value: " << globalMax << std::endl;
            std::cout << "Number of processes: " << size << std::endl;
            std::cout << "Execution time: " << (endTime - startTime) << " seconds" << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
