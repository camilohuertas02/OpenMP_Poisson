#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono> // For timing
#include <omp.h>  // OpenMP library
#include "utils.h" // Common utilities, Grid type, constants

int main() {
    Grid T, T_new, f;

    // Initialize grids
    initialize_grid(T, f);
    T_new = T;

    // Jacobi iteration parameters
    int iterations = 0;
    double max_delta_iteration;

    // For calculating step sizes
    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    double global_max_delta = TOL + 1.0;

    // Main Jacobi iteration loop
    while (global_max_delta > TOL && iterations < MAX_ITER) {
        max_delta_iteration = 0.0;

        // Parallel update of interior points using OpenMP parallel for with collapse
        // The loops over 'i' (rows) and 'j' (columns) are collapsed into a single parallel loop.
        // Variables:
        // T, f, T_new: shared by default.
        // M, N, dx2, dy2, factor: shared, read-only.
        // i, j: private by default in omp for with collapse.
        // reduction(max:max_delta_iteration) for delta calculation.
        #pragma omp parallel for collapse(2) reduction(max:max_delta_iteration) shared(T, T_new, f, M, N, dx2, dy2, factor)
        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                // Store old value before update for delta calculation for this point
                double T_old_ij = T[i][j];

                T_new[i][j] = factor * (
                    (T[i-1][j] + T[i+1][j]) / dx2 +
                    (T[i][j-1] + T[i][j+1]) / dy2 -
                    f[i][j]
                );
                double current_diff = std::abs(T_new[i][j] - T_old_ij);
                if (current_diff > max_delta_iteration) { // This updates the reduction variable
                    max_delta_iteration = current_diff;
                }
            }
        }
        global_max_delta = max_delta_iteration;

        // Update T with T_new for the next iteration
        // This copy can also be parallelized, possibly with collapse as well.
        #pragma omp parallel for collapse(2) shared(T, T_new, M, N)
        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                T[i][j] = T_new[i][j];
            }
        }
        iterations++;
    }

    // Stop timer
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Output results
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << global_max_delta << std::endl;

    // Save the final grid
    save_grid(T, "data/solucion_collapse.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
