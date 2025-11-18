set terminal wxt size 800,600
set title 'Годограф Найквиста W(s) = 200/(s(0.05s+1)(0.02s+1))'
set xlabel 'Re(W(jω))'
set ylabel 'Im(W(jω))'
set grid
set size ratio -1
set xrange [-5:1]
set yrange [-3:3]
plot 'nyquist_data.txt' using 1:2 with lines linewidth 2 title 'Годограф Найквиста', \
     'critical_point.txt' with points pointtype 7 pointsize 2 linecolor rgb 'red' title 'Точка (-1, j0)', \
     -1 with lines linecolor rgb 'red' linetype 2 title 'Re = -1'
pause -1
