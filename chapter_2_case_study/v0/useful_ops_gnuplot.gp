# Set grid
set grid

# Set the terminal to output a PNG image
set terminal png

# Specify the output file name
set output 'useful_ops.png'

# Set plot title and axis labels
set title "PPC Chapter 2 v0"
set xlabel "Input Size"
set ylabel "Billion useful ops /sec"

# Plot the data from the file "data.dat"
# 'with linespoints' displays both lines and points
plot 'useful_ops_data' with linespoints title "v0"
