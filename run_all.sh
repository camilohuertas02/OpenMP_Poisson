#!/bin/bash

# Script mejorado para análisis de escalabilidad.
# Ejecuta cada versión paralela con un número de hilos de 1 a MAX_THREADS.

RESULTS_CSV="resultados.csv"
BIN_DIR="bin"
# Puedes cambiar este valor si tuvieras una máquina con más o menos núcleos
MAX_THREADS=8

echo "Starting the SCALABILITY benchmark process..."

# --- Build Step ---
echo "Cleaning and building all executables..."
make clean > /dev/null && make all
if [ $? -ne 0 ]; then
    echo "Build failed. Aborting benchmark."
    exit 1
fi

# --- CSV Header ---
# Añadimos una columna para el número de hilos
echo "Version,Directiva usada,Num Hilos,Tiempo (s),Iteraciones,Observaciones" > $RESULTS_CSV

# --- Definición de las Versiones a Probar ---
# Usamos arrays para gestionar las versiones de forma más limpia
declare -a EXECUTABLES=(
    "poisson_serial"
    "poisson_parallel_for"
    "poisson_collapse"
    "poisson_schedule"
    "poisson_atomic"
    "poisson_critical"
    "poisson_task"
)
declare -a NAMES=(
    "Secuencial"
    "Paralelo (for)"
    "Colapsado de bucles"
    "Schedule (static)"
    "Contador Atómico"
    "Sección Crítica"
    "Tareas (task)"
)
declare -a DIRECTIVES=(
    "N/A"
    "#pragma omp parallel for"
    "collapse(2)"
    "schedule(static)"
    "atomic"
    "critical"
    "task"
)

# --- Bucle Principal de Ejecución ---
echo "Running solvers and collecting results..."

# Recorremos cada una de las versiones definidas en los arrays
for i in "${!EXECUTABLES[@]}"; do
    executable="${EXECUTABLES[$i]}"
    name="${NAMES[$i]}"
    directive="${DIRECTIVES[$i]}"

    # El caso de la versión serial se ejecuta una sola vez (siempre es 1 hilo)
    if [ "$executable" == "poisson_serial" ]; then
        echo "Running Serial..."
        output=$(./$BIN_DIR/$executable)
        time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
        iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
        echo "$name finished. Time: $time s, Iterations: $iters"
        echo "$name,$directive,1,$time,$iters," >> $RESULTS_CSV
    else
        # Para las versiones paralelas, hacemos un bucle de 1 a 8 hilos
        for threads in $(seq 1 $MAX_THREADS); do
            echo "Running $name with $threads thread(s)..."
            
            # ESTA ES LA LÍNEA CLAVE:
            # Exportamos la variable de entorno para que OpenMP la lea.
            export OMP_NUM_THREADS=$threads
            
            # Ejecutamos el programa y capturamos su salida
            output=$(./$BIN_DIR/$executable)
            time=$(echo "$output" | grep "Time:" | cut -d':' -f2 | xargs)
            iters=$(echo "$output" | grep "Iterations:" | cut -d':' -f2 | xargs)
            
            echo "$name with $threads thread(s) finished. Time: $time s, Iterations: $iters"
            
            # Guardamos la fila en el CSV, incluyendo el número de hilos
            echo "$name,$directive,$threads,$time,$iters," >> $RESULTS_CSV
        done
    fi
done

echo "-----------------------------------------------------"
echo "Benchmark process completed."
echo "Scalability results collected in $RESULTS_CSV"
echo "-----------------------------------------------------"
