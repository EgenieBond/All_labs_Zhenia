set terminal pngcairo size 800,600 enhanced font 'Arial,14'
set output 'nyquist_plot.png'
set title 'Годограф Найквиста'
set xlabel 'Re(H(jω))' font 'Arial,12'
set ylabel 'Im(H(jω))' font 'Arial,12'
set grid linecolor rgb '#DDDDDD'
set size square
set key top left
set arrow 1 from graph 0, first 0 to graph 1, first 0 nohead lc 'black' lw 1.5
set arrow 2 from first 0, graph 0 to first 0, graph 1 nohead lc 'black' lw 1.5
plot 'nyquist_data.txt' with lines lw 3 lc rgb 'blue' title 'Годограф'
