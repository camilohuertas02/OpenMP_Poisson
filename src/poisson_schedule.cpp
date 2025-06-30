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

    // Initialize grids (example initialization)
    for(int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            T[i][j] = 0.0;
            T_new[i][j] = 0.0;
            f[i][j] = sin(M_PI * i / (M-1)) * sin(M_PI * j / (N-1));
        }
    }

    int iterations = 0;
    double max_delta;

    // Set number of threads
    omp_set_num_threads(8);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    do {
        max_delta = 0.0;

        // --- CALCULATION LOOP with schedule(static) ---
        // The schedule(static) clause divides the iterations among threads in fixed-size chunks.
        // It's efficient when all iterations take roughly the same amount of time.
        // 'private(j)' is removed to fix the compilation error.
        #pragma omp parallel for reduction(max:max_delta) schedule(static)
        for (int i = 1; i < M - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                // Apply the finite difference formula
                T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);

                // Update max_delta directly. The reduction clause handles everything.
                max_delta = std::max(max_delta, std::abs(T_new[i][j] - T[i][j]));
            }
        }

        // --- COPY LOOP with schedule(static) ---
        // This copy operation is also parallelized for consistency.
        #pragma omp parallel for schedule(static)
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

    // Output results
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << max_delta << std::endl;

    // Save the final grid. The filename reflects the schedule tested.
    // To test schedule(dynamic), you would change it here and in the pragma.
    save_grid(T, "data/solucion_schedule_static.dat");

    return 0;
}
