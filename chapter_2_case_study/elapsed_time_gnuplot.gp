# Set the grid
set grid

# Set the terminal to output a PNG image
set terminal png

# Specify the output file name
set output 'graphs/elapsed_time.png'

# Set plot title and axis labels
set title "PPC Chapter 2"
set xlabel "Input Size"
set ylabel "Wall-clock time in Sec"

# Plot the data from the file "data.dat"
# 'with linespoints' displays both lines and points
plot 'v0/elapsed_time_data' with linespoints title "v0", \
     'v1/elapsed_time_data' with linespoints title "v1"