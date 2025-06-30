#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include "utils.h" // Common utilities, Grid type, constants

int main() {
	Grid T(M, std::vector<double>(N));
	Grid T_new(M, std::vector<double>(N));
	Grid f(M, std::vector<double>(N));

	// For simplicity, let's assume utils.h has an initialize_grid function
	// that sets up T, T_new, and f with boundary conditions.
	// If not, we'd initialize them here. For now, let's assume they are zeroed.
	for(int i=0; i<M; ++i) {
		for (int j=0; j<N; ++j) {
			T[i][j] = 0.0;
			T_new[i][j] = 0.0;
			// Example source term, can be anything
			f[i][j] = sin(M_PI * i / (M-1)) * sin(M_PI * j / (N-1));
		}
	}


	int iterations = 0;
	double max_delta; // Used to check for convergence

	// Set the number of threads to use
	// En un proyecto real, es mejor usar la variable de entorno OMP_NUM_THREADS
	omp_set_num_threads(8);

	// Start timer
	auto start_time = std::chrono::high_resolution_clock::now();

	do {
		max_delta = 0.0; // Reset max delta for this iteration

		// --- CALCULATION LOOP ---
		// Parallelize the loop over 'i' (rows).
		// reduction(max:max_delta) finds the maximum difference across all threads safely.
		// 'j' does not need to be declared private, as its scope is inside the loop.
#pragma omp parallel for reduction(max:max_delta)
		for (int i = 1; i < M - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				// Apply the finite difference formula (Jacobi method)
				T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);

				// Update max_delta directly. The reduction clause handles the synchronization.
				max_delta = std::max(max_delta, std::abs(T_new[i][j] - T[i][j]));
			}
		}

		// --- COPY LOOP ---
		// Copy the new grid to the old one for the next iteration.
		// This loop can also be parallelized.
#pragma omp parallel for
		for (int i = 1; i < M - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				T[i][j] = T_new[i][j];
			}
		}

		iterations++;
	} while (max_delta > TOL && iterations < MAX_ITER);

	// Stop timer
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_time = end_time - start_time;

	// Output results in the specified format
	std::cout << std::fixed << std::setprecision(6);
	std::cout << "Time:" << elapsed_time.count() << std::endl;
	std::cout << "Iterations:" << iterations << std::endl;
	std::cout << "Final_Delta:" << max_delta << std::endl;

	// Save the final grid
	save_grid(T, "data/solucion_parallel_for.dat");

	return 0;
}
