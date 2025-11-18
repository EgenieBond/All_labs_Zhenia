set terminal wxt size 800,600
set title 'Кривая Михайлова D(jω) для замкнутой системы'
set xlabel 'Re(D(jω))'
set ylabel 'Im(D(jω))'
set grid
set size ratio -1
set xrange [-50000:250000]
set yrange [-150000:150000]
plot 'mikhailov_data.txt' using 1:2 with lines linewidth 2 title 'Кривая Михайлова', \
     0 with lines linecolor rgb 'black' title 'Re=0', \
     0 with lines linecolor rgb 'black' axis x1y2 title 'Im=0'
pause -1
