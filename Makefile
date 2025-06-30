# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
OMPFLAGS = -fopenmp
LDFLAGS =

# Source and executable names
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data
IMG_DIR = imag

# List of source files (without .cpp extension)
SOURCES = \
	  poisson_serial \
	  poisson_parallel_for \
	  poisson_collapse \
	  poisson_sections \
	  poisson_schedule \
	  poisson_atomic \
	  poisson_critical \
	  poisson_task

EXECUTABLES = $(patsubst %,$(BIN_DIR)/%,$(SOURCES))

# Default target
all: $(EXECUTABLES)

# Rule to compile each C++ source file
$(BIN_DIR)/%: $(SRC_DIR)/%.cpp $(SRC_DIR)/utils.h
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $< -o $@ $(LDFLAGS)

# Target to run all benchmarks
benchmark: all
	@echo "Running benchmarks..."
	@./run_all.sh

# Target to generate plots using Gnuplot
plots:
	@echo "Generating plots with Gnuplot..."
	@mkdir -p $(IMG_DIR)
	@for dat_file in $(DATA_DIR)/*.dat; do \
		if [ -f "$$dat_file" ]; then \
		base_name=$$(basename $$dat_file .dat); \
		gnuplot -e "datafile='$$dat_file'; outfile='$(IMG_DIR)/$$base_name.png'" visualize.gp; \
		else \
		echo "No .dat files found in $(DATA_DIR) to generate plots."; \
		break; \
		fi \
		done
	@echo "Plots saved in $(IMG_DIR)/"


# Target to clean up
clean:
	@echo "Cleaning up..."
	@rm -rf $(BIN_DIR)
	@rm -f $(DATA_DIR)/*.dat
	@rm -f $(IMG_DIR)/*.png
	@rm -f resultados.csv
	@echo "Cleanup complete."

# Phony targets
.PHONY: all clean benchmark plots
