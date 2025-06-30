#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono> // For timing
#include "utils.h" // Common utilities, Grid type, constants

int main() {
    Grid T, T_new, f;

    // Initialize grids
    initialize_grid(T, f); // T is (M+2)x(N+2), f is (M+2)x(N+2)
    T_new = T; // Make T_new the same size and copy initial values

    // Jacobi iteration parameters
    int iterations = 0;
    double delta = TOL + 1.0; // Initialize delta to be larger than TOL

    // For calculating step sizes, assuming domain [0,1]x[0,1]
    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;

    // Start timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Main Jacobi iteration loop
    while (delta > TOL && iterations < MAX_ITER) {
        delta = 0.0; // Reset delta for this iteration

        // Update interior points
        // T_new[i][j] = 0.25 * (T[i-1][j] + T[i+1][j] + T[i][j-1] + T[i][j+1] + f[i][j]*dx*dx) for dx=dy
        // More general form for dx != dy:
        // T_new[i][j] = ( (T[i-1][j] + T[i+1][j])/dx2 + (T[i][j-1] + T[i][j+1])/dy2 - f[i][j] ) / (2/dx2 + 2/dy2)
        double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                T_new[i][j] = factor * (
                    (T[i-1][j] + T[i+1][j]) / dx2 +
                    (T[i][j-1] + T[i][j+1]) / dy2 -
                    f[i][j]
                );
                // Calculate the maximum change for convergence check
                double current_diff = std::abs(T_new[i][j] - T[i][j]);
                if (current_diff > delta) {
                    delta = current_diff;
                }
            }
        }

        // Update T with T_new for the next iteration
        T = T_new;
        iterations++;
    }

    // Stop timer
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    // Output results in Key:Value format
    std::cout << std::fixed << std::setprecision(6); // Set precision for output
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << delta << std::endl;

    // Save the final grid
    save_grid(T, "data/solucion_serial.dat");

    if (iterations == MAX_ITER && delta > TOL) {
        // std::cerr can be used for warnings/errors not meant for parsing by run_all.sh
        // std::cerr << "Warning: Maximum iterations reached without convergence." << std::endl;
    }

    return 0;
}
