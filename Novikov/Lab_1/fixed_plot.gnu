set terminal pngcairo size 1600,1200 enhanced font 'Arial,12'
set output 'all_characteristics.png'
set multiplot layout 2,3 title 'H(jw) = 200 / (0.001*(jw)^3 + 0.07*(jw)^2 + jw + 200)'

# 1. Годограф Найквиста
set title 'Годограф Найквиста'
set xlabel 'Re(H(jω))'
set ylabel 'Im(H(jω))'
set grid
set size square
plot 'nyquist_data.txt' with lines lw 2 lc rgb 'blue' title 'Годограф'

# 2. АЧХ
set title 'Амплитудно-частотная характеристика'
set xlabel 'ω, рад/с'
set ylabel 'A(ω)'
set logscale x
set grid
plot 'amplitude_data.txt' with lines lw 2 lc rgb 'red' title 'A(ω)'

# 3. ЛАЧХ
set title 'Логарифмическая АЧХ'
set xlabel 'ω, рад/с'
set ylabel 'L(ω), дБ'
set logscale x
set grid
plot 'bode_data.txt' using 1:2 with lines lw 2 lc rgb 'green' title 'L(ω)'

# 4. ФЧХ
set title 'Фазо-частотная характеристика'
set xlabel 'ω, рад/с'
set ylabel 'φ(ω), градусы'
set logscale x
set grid
plot 'phase_data.txt' with lines lw 2 lc rgb 'purple' title 'φ(ω)'

# 5. Действительная и мнимая части
set title 'Действительная и мнимая части'
set xlabel 'ω, рад/с'
set ylabel 'Re(H), Im(H)'
set logscale x
set grid
plot 'real_imag_data.txt' using 1:2 with lines lw 2 lc rgb 'blue' title 'Re(H)', \
     'real_imag_data.txt' using 1:3 with lines lw 2 lc rgb 'orange' title 'Im(H)'

# 6. Диаграмма Боде (ЛАФЧХ)
set title 'Диаграмма Боде (ЛАФЧХ)'
set xlabel 'ω, рад/с'
set ylabel 'L(ω), дБ'
set y2label 'φ(ω), градусы'
set logscale x
set grid
set y2tics
plot 'bode_data.txt' using 1:2 with lines lw 2 lc rgb 'green' title 'L(ω)' axes x1y1, \
     'bode_data.txt' using 1:3 with lines lw 2 lc rgb 'purple' title 'φ(ω)' axes x1y2

unset multiplot