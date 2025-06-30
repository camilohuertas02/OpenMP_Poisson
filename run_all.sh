#!/bin/bash

# Ensure the script exits on any error
set -e

echo "Starting the benchmark process..."

# Clean previous build and data, then build all executables
echo "Cleaning and building all executables..."
make clean
make all

# Create results directory if it doesn't exist (though data is top-level for .dat)
mkdir -p data
mkdir -p imag # visualize.py will save images here

# CSV Header
HEADER="Version,Directiva,Tiempo (s),Iteraciones,Observaciones (Final Delta)"
RESULTS_FILE="resultados.csv"
echo "$HEADER" > "$RESULTS_FILE"

echo "Running solvers and collecting results..."

# Function to run a solver and parse its output
run_and_parse() {
    local executable_path=$1
    local version_name=$2
    local directive_name=$3 # e.g., "N/A", "parallel for", "collapse(2)"

    echo "Running $version_name..."

    # Execute and capture output
    # The output is expected to be:
    # Time:VALUE
    # Iterations:VALUE
    # Final_Delta:VALUE
    output=$(./"$executable_path")

    # Extract values using grep and cut
    # Using awk for more robust parsing
    time_s=$(echo "$output" | grep "Time:" | awk -F':' '{print $2}' | tr -d '[:space:]')
    iterations=$(echo "$output" | grep "Iterations:" | awk -F':' '{print $2}' | tr -d '[:space:]')
    final_delta=$(echo "$output" | grep "Final_Delta:" | awk -F':' '{print $2}' | tr -d '[:space:]')

    # Append to CSV
    echo "$version_name,$directive_name,$time_s,$iterations,$final_delta" >> "$RESULTS_FILE"
    echo "$version_name finished. Time: $time_s s, Iterations: $iterations, Final Delta: $final_delta"
}

# Run each version
run_and_parse "bin/poisson_serial" "Serial" "N/A"
run_and_parse "bin/poisson_parallel_for" "Parallel For" "omp parallel for"
run_and_parse "bin/poisson_collapse" "Collapse" "omp parallel for collapse(2)"
run_and_parse "bin/poisson_sections" "Sections" "omp parallel sections"
run_and_parse "bin/poisson_schedule" "Schedule Static" "omp parallel for schedule(static)" # Assuming schedule(static) is implemented
run_and_parse "bin/poisson_atomic" "Atomic (Illustrative)" "omp parallel for" # Atomic is context-dependent, main parallel structure is for
run_and_parse "bin/poisson_critical" "Critical (Illustrative)" "omp parallel for" # Critical is context-dependent, main parallel structure is for
run_and_parse "bin/poisson_task" "Task" "omp task"

echo "-----------------------------------------------------"
echo "All solvers executed. Results collected in $RESULTS_FILE"
echo "Data files saved in data/"
echo "-----------------------------------------------------"

# Optional: Suggest next steps
echo "To visualize the results, run: make plots"
echo "The generated images will be in imag/"
echo "Benchmark process completed."
chmod +x Taller_OpenMP_Poisson/run_all.sh
