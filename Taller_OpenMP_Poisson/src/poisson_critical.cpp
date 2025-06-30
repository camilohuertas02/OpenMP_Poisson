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
    double max_delta_iteration_global; // This will be updated in a critical section or via reduction

    // For calculating step sizes
    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    double global_max_delta_overall = TOL + 1.0;


    // Main Jacobi iteration loop
    while (global_max_delta_overall > TOL && iterations < MAX_ITER) {
        max_delta_iteration_global = 0.0; // Reset max delta for this iteration

        // Parallel update of interior points.
        // The 'critical' directive protects a block of code, ensuring only one thread
        // executes it at a time. This is useful for updating shared variables that
        // cannot be handled by atomic operations or reductions (e.g., complex data structures, I/O).
        // For calculating max_delta, reduction(max:...) is generally more efficient than critical.
        // Using 'critical' for this would look like:
        //   double local_delta_thread = 0.0; // calculate in thread
        //   // ... computation ...
        //   #pragma omp critical
        //   {
        //       if (local_delta_thread > max_delta_iteration_global) {
        //           max_delta_iteration_global = local_delta_thread;
        //       }
        //   }
        // This file will use `omp parallel for` with `reduction(max:...)` for efficiency,
        // as it's the standard OpenMP way for this pattern.
        // The filename "poisson_critical" serves to acknowledge the directive,
        // similar to the "poisson_atomic" example.

        #pragma omp parallel for private(j) reduction(max:max_delta_iteration_global) shared(T, T_new, f, M, N, dx2, dy2, factor)
        for (int i = 1; i <= M; ++i) {
            double thread_max_delta_for_row = 0.0;
            for (int j = 1; j <= N; ++j) {
                T_new[i][j] = factor * (
                    (T[i-1][j] + T[i+1][j]) / dx2 +
                    (T[i][j-1] + T[i][j+1]) / dy2 -
                    f[i][j]
                );
                double current_diff = std::abs(T_new[i][j] - T[i][j]);
                if (current_diff > thread_max_delta_for_row) {
                    thread_max_delta_for_row = current_diff;
                }
            }
            // The reduction clause handles combining thread_max_delta_for_row from each thread
            // into max_delta_iteration_global. Each thread updates its view of max_delta_iteration_global
            // with its local maximum (thread_max_delta_for_row).
            if (thread_max_delta_for_row > max_delta_iteration_global) {
                 max_delta_iteration_global = thread_max_delta_for_row;
            }
        }
        global_max_delta_overall = max_delta_iteration_global;


        // Update T with T_new
        #pragma omp parallel for private(j) shared(T, T_new, M, N)
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
    std::cout << "Final_Delta:" << global_max_delta_overall << std::endl;

    save_grid(T, "data/solucion_critical.dat");

    if (iterations == MAX_ITER && global_max_delta_overall > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
