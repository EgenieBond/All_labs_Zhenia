set terminal pngcairo size 800,600 enhanced font 'Arial,14'
set output 'amplitude_plot.png'
set title 'Амплитудно-частотная характеристика (АЧХ)'
set xlabel 'ω, рад/с' font 'Arial,12'
set ylabel 'A(ω)' font 'Arial,12'
unset logscale x
set xrange [0:300]
set grid linecolor rgb '#DDDDDD'
set key top right
set arrow from graph 0, first 0 to graph 1, first 0 nohead lc 'black' lw 1.5
set arrow from first 0, graph 0 to first 0, graph 1 nohead lc 'black' lw 1.5
plot 'amplitude_data.txt' with lines lw 3 lc rgb 'red' title 'АЧХ'
