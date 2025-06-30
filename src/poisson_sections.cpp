#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <omp.h>
#include "utils.h" // Asume que aquí están M, N, TOL, etc. y save_grid.

/**
 * @brief  Initializes the grid T with boundary conditions and the source term f.
 * @note   This is a standalone function as required by the assignment.
 */
void initialize_grid_and_source(Grid& T, Grid& f) {
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			// Initialize potential grid T (e.g., all zeros)
			T[i][j] = 0.0;
			// Initialize source term f (e.g., a sine function)
			f[i][j] = sin(M_PI * i / (M - 1)) * sin(M_PI * j / (N - 1));
		}
	}
}


int main() {
	Grid T(M, std::vector<double>(N));
	Grid f(M, std::vector<double>(N));

	// --- PARALELIZACIÓN CON SECTIONS ---
	// El objetivo de esta actividad es paralelizar tareas funcionalmente distintas.
	// Aquí, simularemos que la inicialización de T y f son dos tareas separadas.
	// NOTA: En la práctica, estas funciones son tan rápidas que el overhead del paralelismo
	// probablemente hará que esta sección sea más lenta que la versión serial.
	// Este es el punto principal de la Actividad 3.

	auto start_init_time = std::chrono::high_resolution_clock::now();

#pragma omp parallel sections
	{
#pragma omp section
		{
			// Tarea 1: Inicializar la grilla T a ceros.
			for (int i = 0; i < M; ++i) {
				for (int j = 0; j < N; ++j) {
					T[i][j] = 0.0;
				}
			}
		}

#pragma omp section
		{
			// Tarea 2: Calcular la función fuente f.
			for (int i = 0; i < M; ++i) {
				for (int j = 0; j < N; ++j) {
					f[i][j] = sin(M_PI * i / (M-1)) * sin(M_PI * j / (N-1));
				}
			}
		}
	} // Fin de la región de sections

	auto end_init_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> init_elapsed = end_init_time - start_init_time;
	// std::cout << "Initialization time with sections: " << init_elapsed.count() << "s\n";

	// --- BUCLE DE CÁLCULO PRINCIPAL (SERIAL) ---
	// Para esta actividad, el bucle principal se mantiene igual que en poisson_serial.cpp
	// para aislar el efecto de paralelizar la inicialización.

	Grid T_new = T;
	int iterations = 0;
	double delta;

	auto start_time = std::chrono::high_resolution_clock::now();

	do {
		delta = 0.0;

		// El bucle de cálculo es SERIAL en este archivo
		for (int i = 1; i < M - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				T_new[i][j] = 0.25 * (T[i+1][j] + T[i-1][j] + T[i][j+1] + T[i][j-1] - f[i][j]);
				delta = std::max(delta, std::abs(T_new[i][j] - T[i][j]));
			}
		}

		// Copiar T_new a T para la siguiente iteración
		for (int i = 1; i < M - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				T[i][j] = T_new[i][j];
			}
		}

		iterations++;
	} while (delta > TOL && iterations < MAX_ITER);

	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed_time = end_time - start_time;

	// --- SALIDA DE RESULTADOS ---
	// El tiempo reportado debe ser el del bucle de cálculo para ser comparable
	// con las otras versiones. El tiempo de inicialización es solo para la reflexión.
	std::cout << std::fixed << std::setprecision(6);
	std::cout << "Time:" << elapsed_time.count() << std::endl;
	std::cout << "Iterations:" << iterations << std::endl;
	std::cout << "Final_Delta:" << delta << std::endl;

	save_grid(T, "data/solucion_sections.dat");

	return 0;
}
