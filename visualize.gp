# --- Script de Gnuplot para visualizar la solución de Poisson ---

# datafile y outfile se pasarán desde el Makefile con el comando -e
# Ejemplo: gnuplot -e "datafile='data/solucion.dat'; outfile='imag/solucion.png'" visualize.gnuplot

# Configura el tipo de archivo de salida (terminal) a PNG
set term pngcairo size 800,600 enhanced font 'Verdana,10'

# Establece el nombre del archivo de salida
set output outfile

# --- Estilo del Gráfico ---
# Título principal del gráfico
set title "Solución de la Ecuación de Poisson"

# Etiquetas para los ejes
set xlabel "Eje X"
set ylabel "Eje Y"

# Configura el gráfico para que sea un mapa de calor 2D
set view map
set pm3d map

# Elimina la leyenda de la superficie (no es necesaria para un mapa 2D)
unset key

# Define una paleta de colores bonita (de azul a rojo pasando por verde y amarillo)
set palette defined ( 0 "#000090", 1 "#000fff", 2 "#0090ff", 3 "#0fffee", \
                      4 "#90ff70", 5 "#ffee00", 6 "#ff7000", 7 "#ee0000", 8 "#7f0000")


# --- Comando de Ploteo ---
# Dibuja los datos del archivo. 'matrix' le dice a gnuplot que los datos son una
# matriz de valores z, y 'with image' lo dibuja como un mapa de calor pixelado.
splot datafile matrix with image

# Imprime un mensaje en la terminal
print "Gráfico generado para ".datafile." y guardado en ".outfile
