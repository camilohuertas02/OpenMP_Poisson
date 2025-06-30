#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include "utils.h"

int main() {
    Grid T(M, std::vector<double>(N));
    Grid T_new(M, std::vector<double>(N));
    Grid f(M, std::vector<double>(N));

    // Initialize grids
    initialize_grid(T, f);
    T_new = T;

    int iterations = 0;
    double max_delta;

    omp_set_num_threads(8);
    
    auto start_time = std::chrono::high_resolution_clock::now();

    do {
        max_delta = 0.0;

        // --- CALCULATION LOOP with collapse(2) ---
        // The collapse(2) clause merges the two nested loops into a single, larger
        // iteration space, giving OpenMP more flexibility to distribute work.
        #pragma omp parallel for collapse(2) reduction(max:max_delta)
        for (int i = 1; i < M - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                // Apply the finite difference formula
                T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);

                // CORRECCIÓN: Update max_delta directly. The reduction clause handles the rest.
                max_delta = std::max(max_delta, std::abs(T_new[i][j] - T[i][j]));
            }
        }

        // --- COPY LOOP with collapse(2) ---
        // This copy operation is also parallelized.
        #pragma omp parallel for collapse(2)
        for (int i = 1; i < M - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                T[i][j] = T_new[i][j];
            }
        }

        iterations++;
    } while (max_delta > TOL && iterations < MAX_ITER);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Output results
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << max_delta << std::endl;

    save_grid(T, "data/solucion_collapse.dat");

    return 0;
}
