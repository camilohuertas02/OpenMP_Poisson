#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono> // For timing
#include <omp.h>  // OpenMP library
#include "utils.h" // Common utilities, Grid type, constants

// Helper function to compute a section of the grid
void compute_section(Grid& T_new, const Grid& T, const Grid& f,
                     int start_row, int end_row,
                     double dx2, double dy2, double factor, double& max_delta_section) {
    max_delta_section = 0.0;
    for (int i = start_row; i <= end_row; ++i) {
        for (int j = 1; j <= N; ++j) {
            T_new[i][j] = factor * (
                (T[i-1][j] + T[i+1][j]) / dx2 +
                (T[i][j-1] + T[i][j+1]) / dy2 -
                f[i][j]
            );
            double current_diff = std::abs(T_new[i][j] - T[i][j]);
            if (current_diff > max_delta_section) {
                max_delta_section = current_diff;
            }
        }
    }
}

// Helper function to copy a section of the grid
void copy_section(Grid& T, const Grid& T_new, int start_row, int end_row) {
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

    // Define number of sections (e.g., 2 or 4)
    const int num_sections = std::min(omp_get_max_threads(), M); // Max sections = M or num threads
                                                              // Or a fixed number like 2 or 4
    // For simplicity, let's use 2 sections if M > 1, otherwise 1 section.
    // More generally, divide M rows among available threads.
    // For this example, let's use 2 sections for demonstration.
    // If you want to use more sections, you'd need to adjust the section definitions.
    // We'll use a fixed number of sections, e.g., 2. If M is small, this might not be efficient.
    int sections_to_use = (M >= 2) ? 2 : 1; // Using 2 sections for example
    if (omp_get_max_threads() < 2 && M >=2) sections_to_use = 1; // Don't use 2 sections if only 1 thread available

    // Main Jacobi iteration loop
    while (global_max_delta > TOL && iterations < MAX_ITER) {
        global_max_delta = 0.0;
        double delta_section1 = 0.0, delta_section2 = 0.0; // For 2 sections
        // Add more delta variables if using more sections

        #pragma omp parallel sections shared(T_new, T, f, M, N, dx2, dy2, factor) reduction(max:global_max_delta)
        {
            #pragma omp section
            {
                double max_d_s1 = 0.0;
                int start1 = 1;
                int end1 = (sections_to_use == 1) ? M : M / 2;
                compute_section(T_new, T, f, start1, end1, dx2, dy2, factor, max_d_s1);
                if (max_d_s1 > global_max_delta) global_max_delta = max_d_s1;
            }

            if (sections_to_use == 2) {
                #pragma omp section
                {
                    double max_d_s2 = 0.0;
                    int start2 = M / 2 + 1;
                    int end2 = M;
                    compute_section(T_new, T, f, start2, end2, dx2, dy2, factor, max_d_s2);
                    if (max_d_s2 > global_max_delta) global_max_delta = max_d_s2;
                }
            }
        } // End of parallel sections for computation

        // Update T with T_new for the next iteration
        // This copy can also be parallelized using sections or a parallel for
        #pragma omp parallel sections shared(T, T_new, M, N)
        {
            #pragma omp section
            {
                int start1 = 1;
                int end1 = (sections_to_use == 1) ? M : M / 2;
                copy_section(T, T_new, start1, end1);
            }
            if (sections_to_use == 2) {
                #pragma omp section
                {
                    int start2 = M / 2 + 1;
                    int end2 = M;
                    copy_section(T, T_new, start2, end2);
                }
            }
        } // End of parallel sections for copy
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
    save_grid(T, "data/solucion_sections.dat");

    if (iterations == MAX_ITER && global_max_delta > TOL) {
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
