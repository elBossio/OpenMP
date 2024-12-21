#include <iostream>
#include <omp.h>
#include <cmath>
#include <chrono>

using namespace std;
double f(double x) {
	return cos(x);
	// return cos (0.5 * x) * exp (0.25 * x);
}

int main() {
	double a = 0;
	double b = 3;
	
	// количество разбиений
	int N = 100000000;

	double h = (b - a) / N;
	double integral = 0;
	
	for (int num_threads : {1, 2, 4, 8, 12, 16, 20, 32, 40, 48, 56, 64}) {
		omp_set_num_threads(num_threads);

		auto start = chrono::high_resolution_clock::now();
#pragma omp parallel for reduction(+:integral)
		for (int i = 0; i < N; i++) {
			double x = a + i * h;
			integral += f(x);
		}

		integral *= h;

		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed_time = end - start;
		cout << " Num of threads: " << num_threads
			<< ", Time: " << elapsed_time.count() << " sec" << endl;
	}

	return 0;

}