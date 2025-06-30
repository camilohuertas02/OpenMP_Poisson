import numpy as np
import matplotlib.pyplot as plt
import argparse
import os

def read_grid_data(filepath):
    """
    Reads grid data from a space-separated file.
    Each row in the file corresponds to a row in the grid.
    """
    try:
        data = np.loadtxt(filepath)
        return data
    except Exception as e:
        print(f"Error reading data file {filepath}: {e}")
        return None

def plot_heatmap(grid_data, output_filepath):
    """
    Generates and saves a heatmap of the grid data.
    """
    if grid_data is None or grid_data.size == 0:
        print("Cannot plot empty or invalid data.")
        return

    plt.figure(figsize=(10, 8))
    plt.imshow(grid_data, cmap='viridis', aspect='auto', origin='lower') # 'viridis' is a common colormap
    plt.colorbar(label='Temperature')
    plt.title(f'Heatmap of Solution: {os.path.basename(output_filepath).replace(".png", "")}')
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')

    # Ensure the output directory exists
    output_dir = os.path.dirname(output_filepath)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)
        print(f"Created directory: {output_dir}")

    try:
        plt.savefig(output_filepath)
        print(f"Heatmap saved to {output_filepath}")
    except Exception as e:
        print(f"Error saving heatmap to {output_filepath}: {e}")
    finally:
        plt.close() # Close the plot to free memory

def main():
    parser = argparse.ArgumentParser(description="Visualize 2D Poisson solver output data as a heatmap.")
    parser.add_argument("datafile", type=str, help="Path to the .dat file containing the grid data.")

    args = parser.parse_args()

    if not os.path.isfile(args.datafile):
        print(f"Error: Data file not found at {args.datafile}")
        return

    grid_data = read_grid_data(args.datafile)

    if grid_data is not None:
        # Construct output filename
        base_filename = os.path.basename(args.datafile)
        name_part, _ = os.path.splitext(base_filename)
        output_filename = f"{name_part}.png"
        output_filepath = os.path.join("imag", output_filename)

        plot_heatmap(grid_data, output_filepath)

if __name__ == "__main__":
    main()
