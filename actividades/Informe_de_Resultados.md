# Informe de Resultados: Optimización del Solver de Poisson 2D con OpenMP

**Nombre del Alumno/Equipo:** [Tu Nombre/Nombre del Equipo Aquí]
**Fecha:** [Fecha de Entrega]
**Curso/Asignatura:** [Nombre del Curso o Asignatura]

## 1. Introducción

Breve descripción del proyecto: el objetivo fue implementar y evaluar el rendimiento de diferentes estrategias de paralelización OpenMP para resolver la ecuación de Poisson 2D mediante el método iterativo de Jacobi. Se exploraron directivas como `parallel for`, `collapse`, `sections`, `schedule`, y `task`.

Parámetros utilizados:
*   Dimensiones de la malla (interior): `M = 200`, `N = 200`
*   Tolerancia de convergencia: `TOL = 1e-6`
*   Número máximo de iteraciones: `MAX_ITER = 100000`
*   Entorno: `g++` (versión), C++17, OpenMP, Python 3 (Matplotlib, Numpy).

## 2. Metodología

Descripción de las versiones implementadas:
*   **Serial:** Versión secuencial base.
*   **Parallel For:** Uso de `#pragma omp parallel for` en el bucle principal.
*   **Collapse:** Uso de `#pragma omp parallel for collapse(2)` para paralelizar bucles anidados.
*   **Sections:** División del trabajo de la malla en secciones (`#pragma omp sections`).
*   **Schedule (Static):** Uso de `#pragma omp parallel for schedule(static)` para la distribución de iteraciones.
*   **Atomic (Ilustrativo):** Implementado con `#pragma omp parallel for` y `reduction`. Se discute el contexto de `atomic`.
*   **Critical (Ilustrativo):** Implementado con `#pragma omp parallel for` y `reduction`. Se discute el contexto de `critical`.
*   **Task:** Uso de `#pragma omp task` para procesar bloques de la malla.

El script `run_all.sh` se utilizó para compilar todas las versiones, ejecutarlas y recolectar los tiempos de ejecución, número de iteraciones y delta final en `resultados.csv`.
El script `visualize.py` se utilizó para generar mapas de calor de las soluciones (almacenados en `imag/`).

## 3. Resultados

Los resultados detallados de las ejecuciones se encuentran en el archivo `resultados.csv` adjunto o en la raíz del proyecto.

A continuación, se presenta una tabla resumen y/o gráficos comparativos basados en `resultados.csv`.

**Tabla Resumen de Rendimiento (Ejemplo):**

| Versión                 | Directiva Principal        | Tiempo (s) | Iteraciones | Delta Final | Speedup vs Serial | Observaciones (Ej: # Hilos Usados) |
| :---------------------- | :------------------------- | :--------- | :---------- | :---------- | :---------------- | :--------------------------------- |
| Serial                  | N/A                        | [tiempo]   | [iters]     | [delta]     | 1.00              | Baseline                           |
| Parallel For            | `omp parallel for`         | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos]                         |
| Collapse                | `omp parallel for collapse(2)` | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos]                         |
| Sections                | `omp parallel sections`    | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos, e.g. 2 sections]        |
| Schedule (Static)       | `omp parallel for schedule(static)` | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos]                         |
| Atomic (Ilustrativo)    | `omp parallel for`         | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos]                         |
| Critical (Ilustrativo)  | `omp parallel for`         | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos]                         |
| Task                    | `omp task`                 | [tiempo]   | [iters]     | [delta]     | [speedup]         | [Nº hilos, chunk_size]             |

*(Instruya al estudiante a completar esta tabla con los datos de `resultados.csv` y calcular el Speedup = Tiempo_Serial / Tiempo_Paralelo)*

**Gráficos:**
*(Instruya al estudiante a incluir gráficos generados a partir de `resultados.csv`, por ejemplo, un gráfico de barras comparando los tiempos de ejecución o el speedup de las diferentes versiones.)*

Ejemplo:
```
[Insertar aquí gráfico de barras de Tiempos de Ejecución]
[Insertar aquí gráfico de barras de Speedup]
```

Las visualizaciones de las soluciones (mapas de calor) se encuentran en la carpeta `imag/`. Estas ayudan a verificar cualitativamente que todas las versiones convergen a soluciones similares. (Ej: `imag/solucion_serial.png`, `imag/solucion_parallel_for.png`, etc.)

## 4. Análisis y Discusión de Resultados

*   Comparar el rendimiento de las versiones paralelizadas con la versión serial. ¿Se obtuvo speedup?
*   ¿Cuál fue la versión paralelizada más eficiente y por qué?
*   Analizar el impacto de las diferentes directivas (`collapse`, `sections`, `schedule`, `task`) en el rendimiento.
    *   ¿Cómo afectó `collapse` en comparación con un `parallel for` simple?
    *   ¿Fue `sections` una buena estrategia para este problema? ¿Limitaciones?
    *   ¿Hubo diferencias notables con `schedule(static)`? (Se podrían probar otros schedules como `dynamic` o `guided` para una discusión más profunda).
    *   ¿Cómo se comportó la versión con `task`? ¿Ventajas o desventajas observadas?
*   Discutir la sobrecarga (overhead) de la paralelización. ¿En qué casos podría una versión paralela ser más lenta que la serial? (Ej: problemas muy pequeños, demasiados hilos para poco trabajo).
*   Comentar sobre la escalabilidad: ¿Cómo podría variar el rendimiento si se aumenta el número de hilos (si es posible probarlo)?
*   Reflexionar sobre la aplicabilidad de `atomic` y `critical` en este problema específico. Aunque se usó `reduction` (que es más adecuado aquí), explicar cuándo estas directivas serían cruciales.
*   Cualquier observación inesperada o dificultad encontrada durante la implementación o pruebas.

## 5. Conclusiones

*   Resumir los hallazgos principales.
*   Reiterar qué estrategia de paralelización fue más efectiva para este problema y bajo las condiciones probadas.
*   Posibles trabajos futuros o mejoras (ej. probar diferentes tamaños de malla, otros schedules, afinidad de hilos, optimizaciones a nivel de caché, etc.).

## 6. Apéndice (Opcional)

*   Código fuente clave (fragmentos, si es relevante y no está ya en el repositorio).
*   Configuración detallada del entorno de pruebas (CPU, número de cores, versión de compilador, etc.).
```
