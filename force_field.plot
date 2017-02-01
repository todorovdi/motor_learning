#!./learn >dat 2>www

set term pdfcairo size 16,8
set out "./output_force_field/force_field.pdf"
unset key
set xrange [0:]
set yrange [0:]

var_dyn2 =        "./output_force_field/CCW_var_dyn2.dat" 
weights2 =        "./output_force_field/CCW_weights2.dat" 
arm =             "./output_force_field/CCW_arm.dat"          
#                                        CCW
datold =          "./output_force_field/CCW_var_dyn.dat" 
weights =         "./output_force_field/CCW_weights.dat" 
weightsOnce =     "./output_force_field/CCW_weights_once.dat" 

errCW =     "./output_force_field/CW_errors.dat" 
errCCW =     "./output_force_field/CCW_errors.dat" 

# ------------ first 

set multi lay 2,2

set x2tics ("force" 300, "washout" 300+350)
set x2tics ("force" 30, "washout" 30+35)

set title "Dynamics of neurons"
#set ytics 0,20,500
set ylabel "M1    D1    D2    GPe    GPi"
plot var_dyn2 u 2:1:3 matrix w image

#set ytics 0,20,300
set title "Synaptic weights"
set ylabel "W1,2,M[HOR,j]" 
#plot weights2 matrix index 0 w image
plot weights2 u 2:1:3 matrix w image

set title "Reaching points (CCW)"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 1 lc 0, arm u 7:8 w p pt 7 ps 1, arm u 2:3:1 w p pt 7 ps .3 lc palette z           

set y2tics
set ytics nomirror
#plot arm u 1:((-atan2($2-$7,$3-$8)/pi*180)) w d smooth unique, arm u 1:(-atan2($4-$7,$5-$8)/pi*180) w d,arm u 1:6 smooth unique axis x1y2 w l

#plot [][*:*]"<cat qqq*" u 1:((-atan2($2-$7,$3-$8)/pi*180)) w d smooth unique,"<cat qqq*" u 1:(-atan2($4-$7,$5-$8)/pi*180) w d,"<cat qqq*" u 1:6 smooth unique axis x1y2 w l
#out<<'\t'<<xcur<<'\t'<<ycur<<'\t'<<(P>.5?x0:x1)<<'\t'
#							<<(P>.5?y0:y1)<<'\t'<<d<<'\t'<<xc<<'\t'<<yc<<k<<endl;

#unset ytics
set yrange [-0.3:0.4]
set xtics 0,100,1000
set xtics 0,10,1000
set title "x-coord"
plot arm u 1:2 w l


unset multi


# ------------ second 

set yrange [-0.2:0.2]
set key
set title "error -- averaged"
plot errCW u 1:2 w l, errCCW u 1:2 w l


unset yrange
unset xtics
set grid
#set ytics 0,5,100
set title "population activity"
#set ylabel "M1    D1    D2    GPe    GPi"
set ylabel "M1"
#plot var_dyn2 matrix w image
# <number of column in file containing x coord>:<-//- y coord>:<-//- color intensity>

#plot "my.var_dyn2" every <line incr>:<dataBlock incr>:<first line>:<first block>:<last line>:<last block>

set ytics 0,20,500
set ylabel "M1    D1    D2    GPe    GPi"
plot var_dyn2 u 2:1:3 matrix w image

#set grid
#set ytics 0,5,500
#set title "population activity"
#set ylabel "M1    D1    D2    GPe    GPi"

#set title "W cortex->striatum"
#set ylabel "w 25->i"
#plot weights2 index 0 u 26 w lp title "D1",weights2 index 1 u 26 w lp title "D2"

set ytics 0,20,600
set title "Synaptic weights"
set ylabel "W1,2,M" 
#plot weights2 matrix index 0 w image
plot weights2 u 2:1:3 matrix w image

#set ylabel "W1 W2 WM 0->j"
#set title "W2 i->j"
#set ylabel "j"
#plot weights2 matrix index 1 w image

set ylabel "Y"

set multi lay 2,2

#set xrange [0:24]
#set yrange [0:100]

set xrange [0:*]
set yrange [0:*]

set ytics
set title "W1 i->j for final trial"
set ylabel "j"
plot weightsOnce matrix index 0 w image

set title "W2 i->j for final trial"
set ylabel "j"
plot weightsOnce matrix index 1 w image

set title "WM i->j for final trial"
set ylabel "j"
plot weightsOnce matrix index 2 w image

unset multi
