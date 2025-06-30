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
    double max_delta_iteration; // Max delta in a single iteration, to be reduced

    // For calculating step sizes
    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    double global_max_delta = TOL + 1.0; // Initialize global_max_delta to be larger than TOL


    // Main Jacobi iteration loop
    while (global_max_delta > TOL && iterations < MAX_ITER) {
        max_delta_iteration = 0.0; // Reset max delta for this iteration

        // Parallel update of interior points using OpenMP parallel for
        // The loop over 'i' (rows) is parallelized.
        // Variables:
        // T, f, T_new: shared by default, which is correct. T is read, T_new is written.
        // M, N, dx2, dy2, factor: shared, read-only, correct.
        // i: private by default in omp for.
        // j: needs to be private for each thread's instance of the inner loop.
        // reduction(max:max_delta_iteration) will find the maximum delta across all threads.
        #pragma omp parallel for private(j) reduction(max:max_delta_iteration) shared(T, T_new, f, M, N, dx2, dy2, factor)
        for (int i = 1; i <= M; ++i) {
            double local_delta_thread = 0.0; // Thread-local variable to find max change in this thread's rows
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
            // Update the iteration's max_delta with the thread's local max
            // This is handled by the reduction clause more efficiently.
            // However, if not using reduction, one would need a critical section here:
            // #pragma omp critical
            // {
            // if (local_delta_thread > max_delta_iteration) {
            // max_delta_iteration = local_delta_thread;
            // }
            // }
            // For this implementation, using reduction(max:max_delta_iteration) on the pragma is preferred.
            // The loop body itself calculates local_delta_thread, which is then reduced.
            // To make reduction work correctly, each thread should update its own partial max.
            // The simplest way with current structure is to have each thread calculate its max_delta and then reduce.
            // So, `max_delta_iteration` in the reduction clause will correctly get the max of all `local_delta_thread` values
            // if `local_delta_thread` is used to update `max_delta_iteration` at the end of each thread's chunk.
            // Let's refine: the reduction variable should be updated by each thread.
            // The value of local_delta_thread from the last j iteration for a given i is what matters for that row.
            // This value then needs to be compared to update max_delta_iteration.
            if (local_delta_thread > max_delta_iteration) { // This line will be part of the reduction logic
                 max_delta_iteration = local_delta_thread; // The reduction handles combining these correctly
            }
        }
        global_max_delta = max_delta_iteration;


        // Update T with T_new for the next iteration
        // This copy can also be parallelized.
        #pragma omp parallel for private(j) shared(T, T_new, M, N)
        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                T[i][j] = T_new[i][j];
            }
        }
        // Note: Boundary rows/columns of T (0 and M+1, 0 and N+1) are not modified if they are fixed boundaries.
        // If T_new was only allocated for interior points, this would be simpler.
        // Given T and T_new are full size, copying only interior points is fine.

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
    save_grid(T, "data/solucion_parallel_for.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
