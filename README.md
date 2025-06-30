# Taller OpenMP: Solucionador de la Ecuación de Poisson 2D

Este proyecto implementa un solucionador para la ecuación de Poisson en 2D usando el método iterativo de Jacobi. Proporciona varias versiones en C++, desde una implementación serial hasta diversas estrategias de paralelización con OpenMP. Se incluyen scripts para la automatización de pruebas y la visualización de resultados con Gnuplot.

## Estructura del Proyecto

```

Taller\_OpenMP\_Poisson/
├── Makefile                     \# Gestiona la compilación, ejecución y limpieza
├── README.md                      \# Este archivo
├── run\_all.sh                     \# Ejecuta todos los solucionadores y recolecta datos de rendimiento
├── visualize.gp                   \# Script de Gnuplot para generar mapas de calor
├── data/                          \# Directorio para los archivos de solución (.dat)
├── src/                           \# Archivos fuente de C++
│   ├── utils.h                    \# Utilidades comunes, constantes y definición de la Grilla
│   ├── poisson\_serial.cpp         \# Versión Serial
│   ├── poisson\_parallel\_for.cpp   \# Versión con OpenMP parallel for
│   ├── poisson\_collapse.cpp     \# Versión con la cláusula collapse de OpenMP
│   ├── poisson\_sections.cpp     \# Versión con la directiva sections de OpenMP
│   ├── poisson\_schedule.cpp     \# Versión con la cláusula schedule de OpenMP
│   ├── poisson\_atomic.cpp       \# Versión para ilustrar el uso de atomic
│   ├── poisson\_critical.cpp     \# Versión para ilustrar el uso de critical
│   └── poisson\_task.cpp         \# Versión basada en tareas de OpenMP
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

### 2\. Ejecutar Pruebas (Benchmark)

Para ejecutar todos los solucionadores, recolectar sus datos de rendimiento y generar el archivo `resultados.csv`:

```bash
make benchmark
```

Este comando primero compila todo el proyecto (si no ha sido compilado antes) y luego ejecuta el script `run_all.sh`. El archivo `resultados.csv` contendrá el tiempo de ejecución, número de iteraciones y otras métricas para cada versión.

### 3\. Generar Gráficos

Después de ejecutar los solucionadores (por ejemplo, con `make benchmark`), se habrán generado archivos de solución (`.dat`) en el directorio `data/`.

Para generar las visualizaciones en forma de mapa de calor a partir de estos archivos:

```bash
make plots
```

Este comando ejecutará el script `visualize.gp` para cada archivo `.dat` encontrado en `data/` y guardará las imágenes PNG resultantes en el directorio `imag/`.

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

  * `bin/*`: Archivos ejecutables compilados.
  * `data/*.dat`: Archivos de datos en crudo que representan la grilla de potencial calculada para cada versión.
  * `imag/*.png`: Imágenes de mapas de calor generadas por `visualize.gp`.
  * `resultados.csv`: Archivo CSV que resume el rendimiento (tiempo, iteraciones) de cada versión del solucionador.

