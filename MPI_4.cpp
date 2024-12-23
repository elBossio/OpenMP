#include <stdio.h>
#include <iostream>
#include <mpi.h>

void fill_matrix(int** matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}


void multiply_matrices(int** matrix1, int** matrix2, int** result, int width, int size, int iter) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            int sum = 0;
            for (int k = 0; k < size; k++) {
                sum += matrix1[i][k] * matrix2[k][j];
            }
            result[i][(j + iter * width) % size] = sum;
        }
    }
}



int main(int argc, char* argv[]) {
    int procNum, procRank;
    int size = atoi(argv[1]);
    int** matrix_A = new int* [size];
    int** matrix_B = new int* [size];
    int** matrix_C = new int* [size];

    for (int i = 0; i < size; i++) {
        matrix_A[i] = new int[size];
        matrix_B[i] = new int[size];
        matrix_C[i] = new int[size];
    }

    fill_matrix(matrix_A, size);
    fill_matrix(matrix_B, size);


    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix_C[i][j] = 0;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    srand(time(NULL) + procRank);
    MPI_Status Status;

    MPI_Comm SquareComm;

    int width = size / procNum;

    int ndims = 1;
    int dims[1] = { procNum };
    int periods[1] = { 0 };
    int reorder = 0;

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &SquareComm);

    int** curLine = new int* [width];
    int** curColumn = new int* [size];
    int** resLine = new int* [width];

    for (int i = 0; i < size; i++) {
        curColumn[i] = new int[width];
    }

    for (int i = 0; i < width; i++) {
        curLine[i] = new int[size];
        resLine[i] = new int[size];
    }



    for (int i = 0; i < width; i++) {
        for (int j = 0; j < size; j++) {
            curLine[i][j] = matrix_A[i + procRank * width][j];
            curColumn[j][i] = matrix_B[j][i + procRank * width];
            resLine[i][j] = 0;
        }
    }

    int curNum = procRank;
    double t1;
    if (procRank == 0)
        t1 = MPI_Wtime();

    for (int iter = 0; iter < procNum; iter++) {
        multiply_matrices(curLine, curColumn, resLine, width, size, curNum);
        if (procNum > 1 && iter != procNum - 1) {
            for (int i = 0; i < size; i++) {
                MPI_Sendrecv_replace(curColumn[i], width, MPI_INT, (procRank + 1) % procNum /* куда посылаем*/, 0, procRank == 0 ? procNum - 1 : procRank - 1 /*откуда ждем */, MPI_ANY_TAG, SquareComm, &Status);
            }
        }
        curNum = curNum == 0 ? procNum - 1 : curNum - 1;
    }

    if (procRank != 0) {
        int* tmp = new int[size * width];
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < size; j++) {
                tmp[i * size + j] = resLine[i][j];
            }
        }
        MPI_Send(tmp, size * width, MPI_INT, 0, 0, SquareComm);
        delete[] tmp;
    }
    else {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < size; j++) {
                matrix_C[i][j] = resLine[i][j];
            }
        }
        int* tmp = new int[size * width];
        for (int rank = 1; rank < procNum; rank++) {
            MPI_Recv(tmp, size * width, MPI_INT, rank, MPI_ANY_TAG, SquareComm, &Status);
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < size; j++) {
                    matrix_C[i + rank * width][j] = tmp[i * size + j];
                }
            }
        }
    }

    if (procRank == 0) {
        double t2 = MPI_Wtime();
        double res = t2 - t1;
        std::cout << "\ntime: " << res << std::endl;
        std::cout << "size : " << size << std::endl;
    }


    for (int i = 0; i < size; i++) {

        if (i < width) {
            delete[] matrix_A[i];
            delete[] matrix_B[i];
            delete[] matrix_C[i];
            delete[] resLine[i];
            delete[] curLine[i];
        }
        delete[] curColumn[i];
    }

    delete[] matrix_A;
    delete[] matrix_B;
    delete[] matrix_C;
    delete[] resLine;
    delete[] curLine;
    delete[] curColumn;
    MPI_Finalize();

    return 0;
}
