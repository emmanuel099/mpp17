# plots the performance of a strategy with different maximum skip list heights
# data file has to be "data.csv"

# usage:
# gnuplot -e "strategy='ascending insert - no failed inserts'" compare_max_height.plot

set terminal pdf
set output strategy . '.pdf'

# text
set title strategy
set xlabel "#Threads"
set ylabel "Throughput [ops/s]"

# input file sparator
set datafile separator ";"

# line style
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 0.75

# plot strategy with different max_height values

plot '<grep "' . strategy . '" data.csv' using 3:(column(2) == 8? $7:1/0)  title "8" with linespoints ls 1 lt rgb "green" , \
     '<grep "' . strategy . '" data.csv' using 3:(column(2) == 16? $7:1/0) title "16" with linespoints ls 1 lt rgb "red", \
     '<grep "' . strategy . '" data.csv' using 3:(column(2) == 64? $7:1/0) title "64" with linespoints ls 1 lt rgb "blue"
