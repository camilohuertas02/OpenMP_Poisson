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
    Grid T, T_new, f;

    // Secciones para inicializar T y f en paralelo.
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            T.assign(M + 2, std::vector<double>(N + 2, 0.0));
        }
        #pragma omp section
        {
            f.assign(M + 2, std::vector<double>(N + 2, 0.0));
            for (int i = 1; i <= M; ++i) {
                for (int j = 1; j <= N; ++j) {
                    f[i][j] = 1.0;
                }
            }
        }
    }
    
    T_new = T;
    int iterations = 0;
    double max_delta;

    double dx = 1.0 / (M + 1);
    double dy = 1.0 / (N + 1);
    double dx2 = dx * dx;
    double dy2 = dy * dy;
    double factor = 1.0 / (2.0 / dx2 + 2.0 / dy2);

    auto start_time = std::chrono::high_resolution_clock::now();

    do {
        max_delta = 0.0;
        
        // Bucle de cálculo SERIAL, con límites corregidos
        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                T_new[i][j] = factor * (
                    (T[i-1][j] + T[i+1][j]) / dx2 +
                    (T[i][j-1] + T[i][j+1]) / dy2 -
                    f[i][j]
                );
                max_delta = std::max(max_delta, std::abs(T_new[i][j] - T[i][j]));
            }
        }

        for (int i = 1; i <= M; ++i) {
            for (int j = 1; j <= N; ++j) {
                T[i][j] = T_new[i][j];
            }
        }

        iterations++;
    } while (max_delta > TOL && iterations < MAX_ITER);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Time:" << elapsed_time.count() << std::endl;
    std::cout << "Iterations:" << iterations << std::endl;
    std::cout << "Final_Delta:" << max_delta << std::endl;

    save_grid(T, "data/solucion_sections.dat");
    return 0;
}
