#!/usr/bin/gnuplot -persist
set term wxt
set xlabel "fringes"
set ylabel "sigmas"
zero(x) = 0
plot "ba1-a1_residuals.txt" u 1:($2/$3) w l tit "ba1a1", "ba1-a2_residuals.txt" u 1:($2/$3) w l tit "ba1a2",\
"ba1-a3_residuals.txt" u 1:($2/$3) w l tit "ba1a3", "ba2-a1_residuals.txt" u 1:($2/$3) w l tit "ba2a1",\
"ba2-a2_residuals.txt" u 1:($2/$3) w l tit "ba2a2", "ba2-a3_residuals.txt" u 1:($2/$3) w l tit "ba2a3", zero(x)

#	EOF