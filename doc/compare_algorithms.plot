# plots the performance of a strategy with different maximum skip list heights

# usage:
# gnuplot -e "strategy='ascending insert - no failed inserts'; max_height=16" compare_algorithms.plot

set terminal pdf
set output strategy . '.pdf'

set logscale yx
set grid ytics lw 1 lt 0
set grid xtics lw 1 lt 0
set yrange [1:]

# text
set title strategy
set xlabel "#Threads"
set ylabel "Throughput [ops/s]"

# input file sparator
set datafile separator ";"

# line style
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 0.75

# plot strategy with different algorithms

plot '<grep "' . strategy . '" ConcurrentSkipList*.csv' using 3:(column(2) == max_height? $7:1/0) title "ConcurrentSkipList" with linespoints ls 1 lt rgb "green" , \
     '<grep "' . strategy . '" LazySkipList*.csv' using 3:(column(2) == max_height? $7:1/0) title "LazySkipList" with linespoints ls 1 lt rgb "red", \
