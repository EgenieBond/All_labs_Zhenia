set terminal pngcairo size 800,600 enhanced font 'Arial,14'
set output 'asymptotic_plot.png'
set title 'Асимптотическая ЛАЧХ'
set xlabel 'ω, рад/с' font 'Arial,12'
set ylabel 'L(ω), дБ' font 'Arial,12'
set logscale x
set grid linecolor rgb '#DDDDDD'
set key top right
set xrange [0.1:100000]
set arrow from graph 0, first 0 to graph 1, first 0 nohead lc 'black' lw 1.5
set arrow from first 0.1, graph 0 to first 0.1, graph 1 nohead lc 'black' lw 1.5
plot 'asymptotic_data.txt' with lines lw 3 lc rgb 'red' title 'Асимптотическая ЛАЧХ'
