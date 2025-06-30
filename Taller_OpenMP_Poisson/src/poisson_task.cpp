#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono> // For timing
#include <omp.h>  // OpenMP library
#include "utils.h" // Common utilities, Grid type, constants

// Helper function for task-based computation of a block of rows
void compute_rows_task(Grid& T_new, const Grid& T, const Grid& f,
                       int start_row, int end_row,
                       double dx2, double dy2, double factor, double& max_delta_block) {
    max_delta_block = 0.0;
    for (int i = start_row; i <= end_row; ++i) {
        for (int j = 1; j <= N; ++j) {
            T_new[i][j] = factor * (
                (T[i-1][j] + T[i+1][j]) / dx2 +
                (T[i][j-1] + T[i][j+1]) / dy2 -
                f[i][j]
            );
            double current_diff = std::abs(T_new[i][j] - T[i][j]);
            if (current_diff > max_delta_block) {
                max_delta_block = current_diff;
            }
        }
    }
}

// Helper function for task-based copy of a block of rows
void copy_rows_task(Grid& T, const Grid& T_new, int start_row, int end_row) {
    for (int i = start_row; i <= end_row; ++i) {
        for (int j = 1; j <= N; ++j) {
            T[i][j] = T_new[i][j];
        }
    }
}

int main() {
    Grid T, T_new, f;

    // Initialize grids
    initialize_grid(T, f);
    T_new = T;

    // Jacobi iteration parameters
    int iterations = 0;

    // For calculating step sizes
    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    double global_max_delta = TOL + 1.0;

    // Task parameters
    int num_threads_avail = omp_get_max_threads();
    // Define chunk size for tasks (e.g., a few rows per task)
    // Ensure chunk_size is at least 1.
    // Let's aim for roughly num_threads_avail * 4 tasks, or more if M is large.
    int num_tasks = std::max(1, num_threads_avail * 4);
    if (M < num_tasks) num_tasks = M; // Cannot have more tasks than rows
    int chunk_size = (M + num_tasks - 1) / num_tasks; // Ceiling division M/num_tasks
    if (chunk_size == 0 && M > 0) chunk_size = 1;


    // Main Jacobi iteration loop
    while (global_max_delta > TOL && iterations < MAX_ITER) {
        global_max_delta = 0.0;
        std::vector<double> task_deltas(num_tasks, 0.0); // Store max delta from each task

        #pragma omp parallel shared(T_new, T, f, M, N, dx2, dy2, factor, chunk_size, task_deltas, num_tasks)
        {
            #pragma omp single // Only one thread creates all the tasks
            {
                for (int task_idx = 0; task_idx < num_tasks; ++task_idx) {
                    int start_row = task_idx * chunk_size + 1;
                    int end_row = std::min((task_idx + 1) * chunk_size, M);

                    if (start_row > M) continue; // No more rows to process

                    // Create a task for computing a block of rows
                    // firstprivate for loop variables, shared for grids etc.
                    // task_deltas[task_idx] is where this task writes its result.
                    #pragma omp task firstprivate(start_row, end_row, task_idx) \
                                     shared(T_new, T, f, dx2, dy2, factor, task_deltas)
                    {
                        compute_rows_task(T_new, T, f, start_row, end_row, dx2, dy2, factor, task_deltas[task_idx]);
                    } // End of omp task for computation
                }
            } // End of omp single for task creation

            // #pragma omp taskwait // Not strictly needed here if reduction is outside parallel region
            // or if the subsequent copy loop also uses tasks and has its own taskwait.
            // However, to calculate global_max_delta from task_deltas, all compute tasks must finish.
        } // End of parallel region for computation tasks. Implicit barrier here.

        // After all compute tasks are done (implicit barrier at end of parallel region),
        // reduce task_deltas to find the global_max_delta for this iteration.
        for (int i = 0; i < num_tasks; ++i) {
            if (task_deltas[i] > global_max_delta) {
                global_max_delta = task_deltas[i];
            }
        }

        // Update T with T_new using tasks for copying
        #pragma omp parallel shared(T, T_new, M, N, chunk_size, num_tasks)
        {
            #pragma omp single
            {
                for (int task_idx = 0; task_idx < num_tasks; ++task_idx) {
                    int start_row = task_idx * chunk_size + 1;
                    int end_row = std::min((task_idx + 1) * chunk_size, M);

                    if (start_row > M) continue;

                    #pragma omp task firstprivate(start_row, end_row) shared(T, T_new)
                    {
                        copy_rows_task(T, T_new, start_row, end_row);
                    } // End of omp task for copy
                }
            } // End of omp single for copy task creation
            // #pragma omp taskwait // Ensure all copy tasks complete before next iteration
                                // Implicit barrier at end of parallel region handles this.
        } // End of parallel region for copy tasks.

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

    save_grid(T, "data/solucion_task.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
