set terminal pngcairo size 800,600 enhanced font 'Arial,14'
set output 'real_imag_plot.png'
set title 'Действительная и мнимая части передаточной функции' font 'Arial,14'
set xlabel 'ω, рад/с' font 'Arial,12'
set ylabel 'Re(H), Im(H)' font 'Arial,12'
unset logscale x
set xrange [0:200]
set grid linecolor rgb '#DDDDDD'
set key top right
set arrow from graph 0, first 0 to graph 1, first 0 nohead lc 'black' lw 1.5
set arrow from first 0, graph 0 to first 0, graph 1 nohead lc 'black' lw 1.5
plot 'real_imag_data.txt' using 1:2 with lines lw 3 lc rgb 'blue' title 'Re(H)', \
     'real_imag_data.txt' using 1:3 with lines lw 3 lc rgb 'orange' title 'Im(H)'
