set terminal pngcairo size 800,600 enhanced font 'Arial,14'
set output 'bode_plot.png'
set title 'Диаграмма Боде (ЛАФЧХ)'
set xlabel 'ω, рад/с' font 'Arial,12'
set ylabel 'L(ω), дБ' font 'Arial,12'
set y2label 'φ(ω), градусы' font 'Arial,12'
set logscale x
set grid linecolor rgb '#DDDDDD'
set y2tics
set key top right
set arrow from graph 0, first 0 to graph 1, first 0 nohead lc 'black' lw 1.5
set arrow from first 0.1, graph 0 to first 0.1, graph 1 nohead lc 'black' lw 1.5
plot 'bode_data.txt' using 1:2 with lines lw 3 lc rgb 'green' title 'L(ω)' axes x1y1, \
     'bode_data.txt' using 1:3 with lines lw 3 lc rgb 'purple' title 'φ(ω)' axes x1y2
