#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

// Функция для многократного обмена сообщениями между двумя процессами
void message_exchange(int rank, int size, int message_size, int iterations) {
    // Буфер для сообщения
    vector<char> message(message_size, 'A');
    // auto start_time = high_resolution_clock::now();
    double start_time = MPI_Wtime();

    for (int i = 0; i < iterations; ++i) {
        if (rank == 0) {
            MPI_Send(message.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(message.data(), message_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (rank == 1) {
            MPI_Recv(message.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(message.data(), message_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }
    // auto end_time = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(end_time - start_time);  // duration.count()

    double end_time = MPI_Wtime();
    if (rank == 0) {
        cout << " - Message size: " << message_size << " bytes, Time taken: " << end_time - start_time << " seconds" << endl;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            cout << "This program requires exactly 2 processes." << endl;
        }
        MPI_Finalize();
        return 1;
    }

    for (int exchange_num : {10, 25, 50, 100, 200}) {
        // Перебор различных размеров сообщений
        int iterations = exchange_num;  // Количество итераций обмена сообщениями
        if (rank == 0) {
            cout << "Exchange num : " << iterations << endl;
        }
        for (int message_size = 1024; message_size <= 536870912; message_size *= 2) {
            message_exchange(rank, size, message_size, iterations);
        }
    }
    MPI_Finalize();

    return 0;
}
