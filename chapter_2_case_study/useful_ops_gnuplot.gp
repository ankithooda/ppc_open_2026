# Set grid
set grid

# Set the terminal to output a PNG image
set terminal png

# Specify the output file name
set output 'graphs/useful_ops.png'

# Set plot title and axis labels
set title "PPC Chapter 2"
set xlabel "Input Size"
set ylabel "Billion useful ops /sec"

# Plot the data from the file "data.dat"
# 'with linespoints' displays both lines and points
plot 'v1/useful_ops_data' with linespoints title "v1", \
     'v2/useful_ops_data' with linespoints title "v2", \
     'v3/useful_ops_data' with linespoints title "v3", \
     'v4/useful_ops_data' with linespoints title "v4"

