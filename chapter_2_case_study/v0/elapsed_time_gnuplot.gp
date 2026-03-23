# Set the grid
set grid

# Set the terminal to output a PNG image
set terminal png

# Specify the output file name
set output 'elapsed_time.png'

# Set plot title and axis labels
set title "PPC Chapter 2 v0"
set xlabel "Input Size"
set ylabel "Wall-clock time in Sec"

# Plot the data from the file "data.dat"
# 'with linespoints' displays both lines and points
plot 'elapsed_time_data' with linespoints title "v0"
