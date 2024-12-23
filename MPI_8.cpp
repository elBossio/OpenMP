#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

// Функция для обмена сообщениями с использованием MPI_Send и MPI_Recv
double message_exchange_send_recv(int rank, int message_size, int iterations) {
    vector<char> message(message_size, 'A');
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

    double end_time = MPI_Wtime();
    return end_time - start_time;
}

// Функция для обмена сообщениями с использованием MPI_Sendrecv
double message_exchange_sendrecv(int rank, int message_size, int iterations) {
    vector<char> send_message(message_size, 'A');
    vector<char> recv_message(message_size, 'B');
    double start_time = MPI_Wtime();

    for (int i = 0; i < iterations; ++i) {
        if (rank == 0) {
            MPI_Sendrecv(send_message.data(), message_size, MPI_CHAR, 1, 0,
                recv_message.data(), message_size, MPI_CHAR, 1, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (rank == 1) {
            MPI_Sendrecv(send_message.data(), message_size, MPI_CHAR, 0, 0,
                recv_message.data(), message_size, MPI_CHAR, 0, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    double end_time = MPI_Wtime();
    return end_time - start_time;
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

    if (rank == 0) {
        cout << "Message size (bytes), Iterations, Time (Send/Recv), Time (Sendrecv)" << endl;
    }

    for (int iterations : {10, 25, 50, 100, 200}) {
        for (int message_size = 1024; message_size <= 268435456; message_size *= 2) { // Уменьшил до 1MB для наглядности
            double time_send_recv = message_exchange_send_recv(rank, message_size, iterations);
            double time_sendrecv = message_exchange_sendrecv(rank, message_size, iterations);

            if (rank == 0) {
                cout << message_size << ", " << iterations << ", "
                    << time_send_recv << ", " << time_sendrecv << endl;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
