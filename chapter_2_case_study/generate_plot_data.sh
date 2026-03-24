#!/bin/bash

dest_1="$1/elapsed_time_data"
dest_2="$1/useful_ops_data"
rm $dest_1 $dest_2
echo -e "#\tX\tY" > $dest_1
echo -e "#\tX\tY" > $dest_2

for ((size = 200; size <= 4000; size = size + 200)); do
    make benchmark T=$1 N=$size | grep AWK_OUTPUT | awk -v dest_1=$dest_1 -v dest_2=$dest_2 '{print $2"\t"$3 >> (dest_1); print $2"\t"$4 >> (dest_2)}'
done

echo "Generated file $dest_1"
echo "Generated file $dest_2"


