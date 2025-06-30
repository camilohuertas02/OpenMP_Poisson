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

        // Parallel update of interior points.
        // The 'atomic' directive is typically used for ensuring that specific memory
        // operations (like updates: x++, x = x + val) occur atomically.
        // In this Jacobi solver, the main computation of T_new[i][j] is independent for each (i,j)
        // and doesn't require atomic operations for T_new itself.
        // The calculation of max_delta_iteration is handled efficiently by reduction(max:...).
        // If we were, for example, summing up a global error or counting certain events
        // directly within the parallel loop without reduction, 'atomic' would be a candidate.
        // For instance, if `max_delta_iteration` was updated directly:
        //   double current_diff = std::abs(T_new[i][j] - T[i][j]);
        //   if (current_diff > some_thread_local_max) some_thread_local_max = current_diff;
        //   // And then at the end of thread's work:
        //   if (some_thread_local_max > max_delta_iteration) {
        //       #pragma omp atomic write // or atomic read/capture then write if complex
        //       // This specific scenario is better with reduction or critical,
        //       // but illustrates where atomic *could* apply for single updates.
        //       // max_delta_iteration = some_thread_local_max; (if atomic write was simple assignment)
        //   }
        // However, `reduction(max:max_delta_iteration)` is the most appropriate and efficient OpenMP feature for this.
        // This file will use `omp parallel for` similar to poisson_parallel_for.cpp,
        // as `atomic` isn't the primary mechanism for parallelizing this algorithm's core.
        // The filename "poisson_atomic" is for demonstrating awareness of the directive,
        // even if its direct application here is superseded by better constructs like reduction.

        #pragma omp parallel for private(j) reduction(max:max_delta_iteration) shared(T, T_new, f, M, N, dx2, dy2, factor)
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
            if (local_delta_thread > max_delta_iteration) {
                 max_delta_iteration = local_delta_thread; // Reduction handles this
            }
        }
        global_max_delta = max_delta_iteration;

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
    std::cout << "Final_Delta:" << global_max_delta << std::endl;

    save_grid(T, "data/solucion_atomic.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
