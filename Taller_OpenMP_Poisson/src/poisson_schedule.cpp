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

        // Parallel update of interior points using OpenMP parallel for with a schedule clause
        // Using schedule(static) as an example. Other schedules like dynamic or guided could be used.
        // The chunk size for static or dynamic can also be specified, e.g., schedule(static, M / (4 * omp_get_max_threads()))
        // For this example, we use default static chunking.
        #pragma omp parallel for private(j) reduction(max:max_delta_iteration) shared(T, T_new, f, M, N, dx2, dy2, factor) schedule(static)
        for (int i = 1; i <= M; ++i) {
            double local_delta_thread = 0.0;
            for (int j = 1; j <= N; ++j) {
                T_new[i][j] = factor * (
                    (T[i-1][j] + T[i+1][j]) / dx2 +
                    (T[i][j-1] + T[i][j+1]) / dy2 -
                    f[i][j]
                );
                double current_diff = std::abs(T_new[i][j] - T[i][j]);
                if (current_diff > local_delta_thread) {
                    local_delta_thread = current_diff;
                }
            }
            if (local_delta_thread > max_delta_iteration) { // Reduction clause handles this logic
                 max_delta_iteration = local_delta_thread;
            }
        }
        global_max_delta = max_delta_iteration;

        // Update T with T_new for the next iteration
        // This copy can also be parallelized.
        #pragma omp parallel for private(j) shared(T, T_new, M, N) schedule(static)
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
    // The filename should reflect the specific schedule if multiple are tested.
    // For this example, we assume "schedule" implies the static schedule used.
    save_grid(T, "data/solucion_schedule_static.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
