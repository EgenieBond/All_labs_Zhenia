set terminal pngcairo size 1000,600 enhanced font 'Arial,16'
set output 'formulas_plot.png'
unset border
unset tics
unset key
set xrange [0:1]
set yrange [0:1]
set label 1 at 0.5,0.9 'Исходная передаточная функция:' center front font 'Arial-Bold,18'
set label 2 at 0.5,0.8 'H(jω) = {200} / {0.001(jω)^3 + 0.07(jω)^2 + jω + 200}' center front font 'Arial,16'
set label 3 at 0.5,0.65 'Действительная часть:' center front font 'Arial-Bold,16'
set label 4 at 0.5,0.55 'Re(H) = {200(200 - 0.07ω^2)} / {(200 - 0.07ω^2)^2 + ω^2(1 - 0.001ω^2)^2}' center front font 'Arial,14'
set label 5 at 0.5,0.4 'Мнимая часть:' center front font 'Arial-Bold,16'
set label 6 at 0.5,0.3 'Im(H) = {-200ω(1 - 0.001ω^2)} / {(200 - 0.07ω^2)^2 + ω^2(1 - 0.001ω^2)^2}' center front font 'Arial,14'
set label 7 at 0.5,0.15 'Амплитуда:' center front font 'Arial-Bold,16'
set label 8 at 0.5,0.05 'A(ω) = {200} / {√[(200 - 0.07ω^2)^2 + ω^2(1 - 0.001ω^2)^2]}' center front font 'Arial,14'
plot 2
