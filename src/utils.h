#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <fstream>
#include <iomanip> // For std::fixed and std::setprecision
#include <cmath>   // For std::abs, std::sin, std::cos, M_PI if available
#include <iostream> // For error messages

// Define M_PI if not available (e.g. on Windows with MSVC)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Grid dimensions and Jacobi parameters
const int M = 200;      // Number of points in x-direction (interior)
const int N = 200;      // Number of points in y-direction (interior)
const double TOL = 1e-6; // Tolerance for convergence
const int MAX_ITER = 100000; // Maximum number of iterations

// Type alias for the grid
using Grid = std::vector<std::vector<double>>;

// Function to initialize the temperature grid T and source term f
// T is initialized to 0.0 everywhere.
// f is initialized with a sample source function, e.g., a Gaussian or trigonometric function.
// Boundary conditions for T are implicitly handled by not updating boundary cells in Jacobi.
// For this example, boundaries of T are kept at 0.
inline void initialize_grid(Grid& T, Grid& f) {
    // Initialize T to 0.0, including boundaries
    // Grid size is (M+2)x(N+2) to include boundary points
    T.assign(M + 2, std::vector<double>(N + 2, 0.0));
    f.assign(M + 2, std::vector<double>(N + 2, 0.0));

    // Example source term: f(x,y) = 2 * (x(1-x) + y(1-y))
    // scaled to the domain [0,1]x[0,1] for physical coordinates
    // For grid indices [1..M] and [1..N]
    double dx = 1.0 / (M + 1); // Step size in x
    double dy = 1.0 / (N + 1); // Step size in y

    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            double x = i * dx;
            double y = j * dy;
            // A common test function: f(x,y) = sin(pi*x) * sin(pi*y)
            // Or f(x,y) = 1.0 (constant source)
            // Or as specified in the problem: 2 * (x(1-x) + y(1-y))
            // f[i][j] = 2.0 * (x * (1.0 - x) + y * (1.0 - y));
            f[i][j] = 1.0; // Simple constant source for testing
            // f[i][j] = 100.0 * std::sin(M_PI * x) * std::sin(M_PI * y); // Example source
        }
    }

    // Note: Boundary conditions for T (e.g., T=0 on boundaries) are often set here
    // or handled by the Jacobi update loop (by not iterating over boundary points).
    // Our current T initialization to 0.0 effectively sets T=0 on boundaries.
}

// Function to save the grid data to a file
// Only interior points are saved.
inline void save_grid(const Grid& T, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    outfile << std::fixed << std::setprecision(5); // Format output

    for (int i = 1; i <= M; ++i) {
        for (int j = 1; j <= N; ++j) {
            outfile << T[i][j] << (j == N ? "" : " ");
        }
        outfile << std::endl;
    }
    outfile.close();
}

#endif // UTILS_H
