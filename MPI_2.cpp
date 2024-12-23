#include <mpi.h>
#include <iostream>
#include <vector>

// Функция для генерации случайного вектора
//std::vector<int> generate_vector(int size) {
//    std::vector<int> vec(size);
//    for (int i = 0; i < size; ++i) {
//        vec[i] = rand() % 100; // Случайные числа от 0 до 99
//    }
//    return vec;
//}

// Функция для генерации вектора с фиксированным значением
std::vector<int> generate_vector(int size, int value) {
    return std::vector<int>(size, value);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double start_time;

    std::vector<long long> vectorSizes = { 1000, 10000, 100000, 1000000, 10000000, 100000000, 500000000 };
    for (long long VECTOR_SIZE : vectorSizes) {
        if (rank == 0) {
            std::cout << "\nVector size: " << VECTOR_SIZE << std::endl;
        }

        const int vector_size = VECTOR_SIZE;
        int base_size = vector_size / size;
        int remainder = vector_size % size;

        std::vector<int> vec1, vec2;
        int local_dot_product = 0, global_dot_product = 0;

        if (rank == 0) {
            // srand(time(0));
            vec1 = generate_vector(vector_size, 1); // Первый вектор заполнен 1
            vec2 = generate_vector(vector_size, 2); // Второй вектор заполнен 2

            start_time = MPI_Wtime();
            for (int i = 1; i < size; ++i) {
                int start_index = i * base_size + std::min(i, remainder);
                int count = base_size + (i < remainder ? 1 : 0);
                MPI_Send(vec1.data() + start_index, count, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(vec2.data() + start_index, count, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }

        int local_size = base_size + (rank < remainder ? 1 : 0);
        std::vector<int> local_vec1(local_size), local_vec2(local_size);

        if (rank == 0) {
            int start_index = 0;
            std::copy(vec1.begin() + start_index, vec1.begin() + start_index + local_size, local_vec1.begin());
            std::copy(vec2.begin() + start_index, vec2.begin() + start_index + local_size, local_vec2.begin());
        }
        else {
            MPI_Recv(local_vec1.data(), local_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(local_vec2.data(), local_size, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0; i < local_size; ++i) {
            local_dot_product += local_vec1[i] * local_vec2[i];
        }

        MPI_Reduce(&local_dot_product, &global_dot_product, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        double end_time = MPI_Wtime();

        if (rank == 0) {
            // std::cout << "Scalar product: " << global_dot_product << std::endl;
            std::cout << "Execution time: " << end_time - start_time << " seconds" << std::endl;
            // std::cout << "Number of processes: " << size << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
