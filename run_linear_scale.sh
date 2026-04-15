#!/bin/bash

# Initial Variables
width=600
height=300
phi=0.40
f="0.7"

echo "Starting linear scale-up analysis..."

# Loop 10 times
for i in {1..10}; do
    # Ensure phi is formatted with a leading zero (e.g., 0.40 instead of .40)
    formatted_phi=$(printf "%.2f" "$phi")

    echo "=========================================="
    echo "Iteration $i: phi=$formatted_phi"

    # Create unique filenames using the formatted phi
    base_csv="base_w${width}_h${height}_phi${formatted_phi}_ff${f}.csv"
    network_csv="network_w${width}_h${height}_phi${formatted_phi}_ff${f}.csv"
    network_svg="network_w${width}_h${height}_phi${formatted_phi}_ff${f}.svg"

    # 1. Generate new particles
    echo "Generating and relaxing particles..."
    ./bin/sim_engine -g -phi "$formatted_phi" -x "$width" -y "$height" -o "$base_csv"

    # 2. Connectivity scan
    echo "Running DSU connectivity scan..."
    # Note: 'time' outputs to stderr, which is perfect for visibility
    time ./bin/sim_engine -i "$base_csv" -f "$f" -o "$network_csv"

    # 3. Plot the results
    echo "Rendering SVG..."
    ./bin/plotter -i "$network_csv" -x "$width" -y "$height" -o "$network_svg"

    # 4. Increment phi using bc
    phi=$(echo "$phi + 0.05" | bc)
done

echo "=========================================="
echo "Linear scale-up analysis completed successfully!"