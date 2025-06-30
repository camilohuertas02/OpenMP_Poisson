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

    // Initialize grids
    for(int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            T[i][j] = 0.0;
            T_new[i][j] = 0.0;
            f[i][j] = sin(M_PI * i / (M-1)) * sin(M_PI * j / (N-1));
        }
    }

    // Shared iteration counter, as per Activity 6
    int iterations = 0;
    double max_delta;

    omp_set_num_threads(8);

    auto start_time = std::chrono::high_resolution_clock::now();

    do {
        max_delta = 0.0;

        // --- CALCULATION LOOP ---
        // Parallelized calculation using reduction for max_delta.
        // This is the most efficient way to handle the calculation itself.
        #pragma omp parallel for reduction(max:max_delta)
        for (int i = 1; i < M - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);
                max_delta = std::max(max_delta, std::abs(T_new[i][j] - T[i][j]));
            }
        }

        // --- COPY LOOP & ATOMIC INCREMENT ---
        // A combined parallel region for the copy and the iteration increment.
        #pragma omp parallel
        {
            // Parallelize the copy operation using an omp for loop
            #pragma omp for
            for (int i = 1; i < M - 1; ++i) {
                for (int j = 1; j < N - 1; ++j) {
                    T[i][j] = T_new[i][j];
                }
            }

            // --- ATOMIC DEMONSTRATION as per Activity 6 ---
            // We use 'single' to ensure only one thread executes this block.
            // We use 'atomic' to protect the increment of the shared 'iterations' variable.
            // This correctly demonstrates the use case requested in the assignment.
            #pragma omp single
            {
                #pragma omp atomic
                iterations++;
            }
        } // End of the second parallel region

    } while (max_delta > TOL && iterations < MAX_ITER);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Output results
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << max_delta << std::endl;

    save_grid(T, "data/solucion_atomic.dat");

    return 0;
}
