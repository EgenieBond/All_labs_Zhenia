set terminal wxt size 1000,800
set multiplot layout 2,1 title 'Диаграммы Боде разомкнутой системы'
set title 'Амплитудно-частотная характеристика (АЧХ)'
set xlabel 'Частота ω, рад/с'
set ylabel 'Усиление, дБ'
set logscale x
set grid
plot 'bode_data.txt' using 1:2 with lines linewidth 2 title 'АЧХ'
set title 'Фазо-частотная характеристика (ФЧХ)'
set xlabel 'Частота ω, рад/с'
set ylabel 'Фаза, градусы'
set logscale x
set grid
set yrange [-270:-90]
plot 'bode_data.txt' using 1:3 with lines linewidth 2 title 'ФЧХ', \
     -180 with lines linecolor rgb 'red' linetype 2 title '-180°'
unset multiplot
pause -1
