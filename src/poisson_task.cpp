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

    initialize_grid(T, f);
    T_new = T;

    int iterations = 0;
    double max_delta;

    omp_set_num_threads(8);

    // --- Task parameters ---
    // Define a fixed block size for each task. This is simpler and often more effective.
    // A value between 16 and 64 is usually a good starting point.
    const int block_size = 32;
    const int num_blocks = (M - 2 + block_size) / block_size; // Ceiling division
    std::vector<double> block_deltas(num_blocks, 0.0); // To store delta from each task block

    auto start_time = std::chrono::high_resolution_clock::now();

    do {
        max_delta = 0.0;
        
        // --- A SINGLE PARALLEL REGION for the entire iteration ---
        // This is much more efficient than creating and destroying threads repeatedly.
        #pragma omp parallel
        {
            // Use 'single' to have one thread create all the tasks.
            #pragma omp single
            {
                // --- Task Generation for Computation ---
                for (int block_idx = 0; block_idx < num_blocks; ++block_idx) {
                    const int start_row = 1 + block_idx * block_size;
                    const int end_row = std::min(start_row + block_size, M - 1);

                    // Create a task for each block of rows.
                    // 'firstprivate' creates a private copy of the variable for each task.
                    #pragma omp task firstprivate(block_idx, start_row, end_row)
                    {
                        double local_block_delta = 0.0;
                        for (int i = start_row; i < end_row; ++i) {
                            for (int j = 1; j < N - 1; ++j) {
                                T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);
                                local_block_delta = std::max(local_block_delta, std::abs(T_new[i][j] - T[i][j]));
                            }
                        }
                        // Each task writes its result to its own slot in the array.
                        block_deltas[block_idx] = local_block_delta;
                    } // End of omp task
                }
            } // End of omp single

            // --- SYNCHRONIZATION AND REDUCTION ---
            // 'taskwait' is not needed here because the following 'omp for' has an
            // implicit barrier that waits for all tasks at this level to complete.

            // All threads participate in finding the maximum delta in parallel.
            // This is much faster than the sequential loop.
            #pragma omp for reduction(max:max_delta)
            for (int i = 0; i < num_blocks; ++i) {
                max_delta = std::max(max_delta, block_deltas[i]);
            }

            // --- Parallel Copy ---
            // Use a simple parallel for for the copy; it's the most direct tool for this job.
            #pragma omp for
            for (int i = 1; i < M - 1; ++i) {
                for (int j = 1; j < N - 1; ++j) {
                    T[i][j] = T_new[i][j];
                }
            }
            
            // One thread increments the shared iteration counter
            #pragma omp single
            {
                iterations++;
            }

        } // --- End of the single parallel region ---

    } while (max_delta > TOL && iterations < MAX_ITER);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Output results
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << max_delta << std::endl;

    save_grid(T, "data/solucion_task.dat");

    return 0;
}
