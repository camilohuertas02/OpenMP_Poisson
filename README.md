# Taller OpenMP: Solucionador de la Ecuación de Poisson 2D

Este proyecto implementa un solucionador para la ecuación de Poisson en 2D usando el método iterativo de Jacobi. Proporciona varias versiones en C++, desde una implementación serial hasta diversas estrategias de paralelización con OpenMP. Se incluyen scripts para realizar un análisis de escalabilidad (de 1 a 8 hilos) y para la visualización de resultados con Gnuplot.

## Estructura del Proyecto

```

Taller\_OpenMP\_Poisson/
├── Makefile                     \# Gestiona la compilación, ejecución y limpieza
├── README.md                      \# Este archivo
├── run\_all.sh                     \# Ejecuta el análisis de escalabilidad y recolecta los datos
├── visualize.gp                   \# Script de Gnuplot para generar mapas de calor
├── data/                          \# Directorio para los archivos de solución (.dat)
├── src/                           \# Archivos fuente de C++
│   ├── utils.h                    \# Utilidades comunes y constantes
│   ├── poisson\_serial.cpp         \# Versión Serial
│   ├── poisson\_parallel\_for.cpp   \# Versión con OpenMP parallel for
│   ├── poisson\_collapse.cpp     \# Versión con la cláusula collapse
│   ├── poisson\_sections.cpp     \# Versión con la directiva sections
│   ├── poisson\_schedule.cpp     \# Versión con la cláusula schedule
│   ├── poisson\_atomic.cpp       \# Versión con contador atómico
│   ├── poisson\_critical.cpp     \# Versión con sección crítica
│   └── poisson\_task.cpp         \# Versión basada en tareas
├── imag/                          \# Directorio para las imágenes de salida (.png)
├── actividades/
│   └── Informe\_de\_Resultados.md   \# Plantilla para el informe de resultados
└── bin/                           \# Directorio para los ejecutables compilados

````

## Requisitos Previos

* Un compilador de C++ con soporte para C++17 y OpenMP (ej. `g++`).
* El programa `gnuplot` instalado en el sistema.
    * En sistemas basados en Debian/Ubuntu como el tuyo, se puede instalar con:
        ```bash
        sudo apt-get update && sudo apt-get install gnuplot
        ```

## Modo de Uso

### 1. Compilar
Para compilar todas las versiones del solucionador en C++:
```bash
make all
````

Esto creará los archivos ejecutables en el directorio `bin/`.

### 2\. Ejecutar Pruebas de Escalabilidad (Benchmark)

Para ejecutar todos los solucionadores con un número de hilos de 1 a 8, recolectar sus datos de rendimiento y generar el archivo `resultados.csv`:

```bash
make benchmark
```

Este comando compila el proyecto y luego ejecuta el script `run_all.sh`. El archivo `resultados.csv` contendrá el tiempo de ejecución y las iteraciones para cada versión y cada número de hilos.

### 3\. Generar Gráficos

Después de ejecutar las pruebas, se habrán generado archivos de solución (`.dat`) en el directorio `data/`. Para generar las visualizaciones en forma de mapa de calor a partir de estos archivos:

```bash
make plots
```

Este comando ejecutará el script `visualize.gp` para cada archivo `.dat` y guardará las imágenes PNG resultantes en el directorio `imag/`.

Si quieres visualizar un archivo de solución específico:

```bash
gnuplot -e "datafile='data/solucion_serial.dat'; outfile='imag/solucion_serial.png'" visualize.gp
```

(Reemplaza `solucion_serial.dat` con el archivo de datos que desees visualizar).

### 4\. Limpiar

Para eliminar todos los archivos compilados, datos generados, imágenes y el archivo de resultados:

```bash
make clean
```

## Archivos de Salida

  * **`bin/*`**: Archivos ejecutables compilados.
  * **`data/*.dat`**: Archivos de datos en crudo que representan la grilla de potencial calculada para cada versión.
  * **`imag/*.png`**: Imágenes de mapas de calor generadas por `visualize.gp`.
  * **`resultados.csv`**: Archivo CSV que resume el rendimiento (tiempo, iteraciones) de cada versión para cada configuración de hilos probada.

