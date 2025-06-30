#!/bin/bash

# Script to run all Poisson solver versions and collect performance results.

RESULTS_CSV="resultados.csv"
BIN_DIR="bin"
DATA_DIR="data"

echo "Starting the benchmark process..."

# --- Build Step ---
echo "Cleaning and building all executables..."
make clean > /dev/null && make all
if [ $? -ne 0 ]; then
    echo "Build failed. Aborting benchmark."
    exit 1
fi

# --- CSV Header ---
# Write header for the results file
echo "Version,Directiva usada,Tiempo (s),Iteraciones,Observaciones" > $RESULTS_CSV

# --- Execution and Data Collection ---
echo "Running solvers and collecting results..."

# Serial Version
echo "Running Serial..."
output=$(./$BIN_DIR/poisson_serial)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Serial finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Secuencial,N/A,$time,$iters," >> $RESULTS_CSV

# Parallel For Version
echo "Running Parallel For..."
output=$(./$BIN_DIR/poisson_parallel_for)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Parallel For finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Paralelo básico (for),#pragma omp parallel for,$time,$iters," >> $RESULTS_CSV

# Collapse Version
echo "Running Collapse..."
output=$(./$BIN_DIR/poisson_collapse)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Collapse finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Colapsado de bucles,collapse(2),$time,$iters," >> $RESULTS_CSV

# Sections Version
echo "Running Sections..."
output=$(./$BIN_DIR/poisson_sections)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Sections finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Inicialización en paralelo,sections,$time,$iters," >> $RESULTS_CSV

# Schedule Static Version
echo "Running Schedule Static..."
output=$(./$BIN_DIR/poisson_schedule)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Schedule Static finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Control explícito,schedule(static),$time,$iters," >> $RESULTS_CSV

# Atomic Version
echo "Running Atomic..."
output=$(./$BIN_DIR/poisson_atomic)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Atomic finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Contador Atómico,atomic,$time,$iters," >> $RESULTS_CSV

# Critical Version
echo "Running Critical..."
output=$(./$BIN_DIR/poisson_critical)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Critical finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Sección Crítica,critical,$time,$iters," >> $RESULTS_CSV

# Task Version
echo "Running Task..."
output=$(./$BIN_DIR/poisson_task)
time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
delta=$(echo "$output" | grep "Final_Delta:" | cut -d':' -f2 | xargs)
echo "Task finished. Time: $time s, Iterations: $iters, Final Delta: $delta"
echo "Paralelismo con Tareas,task,$time,$iters," >> $RESULTS_CSV

# --- Completion Message ---
echo "-----------------------------------------------------"
echo "All solvers executed. Results collected in $RESULTS_CSV"
echo "Data files saved in $DATA_DIR/"
echo "-----------------------------------------------------"
echo "To visualize the results, run: make plots"
echo "The generated images will be in imag/"
echo "Benchmark process completed."
