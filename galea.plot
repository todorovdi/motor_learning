#!./learn >at 2>www

set term pdfcairo size 16,8
set out "./output_galea/galea.pdf"
unset key
set xrange [0:]
set yrange [0:]

var_dyn2 =        "./output_galea/galea_var_dyn2.dat" 
weights2 =        "./output_galea/galea_weights2.dat" 
arm =             "./output_galea/galea_arm.dat"          
                            
datold =          "./output_galea/galea_var_dyn.dat" 
weights =         "./output_galea/galea_weights.dat" 
weightsOnce =     "./output_galea/galea_weights_once.dat" 

err =     "./output_galea/galea_errors.dat" 

# ------------ first 

set multi lay 2,2

set xtics 0,5,100
set x2tics ("PRE2" 12, "ADAPT1" 24, "POST1" 49, "ADAPT2" 61, "POST2" 79) 

set grid
set title "Dynamics of neurons"
#set ytics 0,20,500
set ylabel "M1    D1    D2    GPe    GPi"
plot var_dyn2 u 2:1:3 matrix w image

#set ytics 0,20,300
set title "Synaptic weights"
set ylabel "W1,2,M 0->j" 
#plot weights2 matrix index 0 w image
plot weights2 u 2:1:3 matrix w image

set title "Reaching points"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 1 lc 0, arm u 7:8 w p pt 7 ps 1, arm u 2:3:1 w p pt 7 ps .3 lc palette z           

#unset ytics

#set yrange [-120:120]
set yrange [-0.2:0.2]
set key
set xtics 0,10,100
#set title "error (in degrees) -- averaged"
set title "error (in distance units) -- averaged"
plot err smooth unique with yerrorbars

#set yrange [-0.3:0.3]
#set key
#set xtics 0,10,100
#set title "error (in dist units) -- averaged"
#plot err smooth unique with yerrorbars

unset multi

unset xtics

# ------------ second 
#set x2tics ("PRE2" 12, "ADAPT1" 24, "POST1" 49, "ADAPT2" 61, "POST1" 79) 
set multi lay 2,2

set title "Reaching points PRE"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 1 lc 0, arm u 7:8 w p pt 7 ps 1, arm every ::::24 u 2:3:1 w p pt 7 ps .3 lc palette z           

set title "Reaching points ADAPT"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 1 lc 0, arm u 7:8 w p pt 7 ps 1, arm every ::24::49 u 2:3:1 w p pt 7 ps .3 lc palette z           

set title "Reaching points POST"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 0.8 lc 0, arm u 7:8 w p pt 7 ps 1, arm every ::49::61 u 2:3:1 w p pt 7 ps .3 lc palette z           


unset multi

set multi lay 2,2

unset xtics
unset ytics
set yrange [0:*]
set xrange [0:*]
unset key
unset grid

set title "Dynamics of neurons"
#set ytics 0,20,500
set ylabel "M1    D1    D2    GPe    GPi"
plot var_dyn2 every ::49::61 u 2:1:3 matrix w image

#set ytics 0,20,300
set title "Synaptic weights"
set ylabel "W1,2,M 0->j" 
#plot weights2 matrix index 0 w image
plot weights2 every ::49::61 u 2:1:3 matrix w image

set title "Reaching points"
set ylabel "Y"
set grid
plot [-.5:.5][0:.8]arm u 4:5 w p pt 7 ps 1 lc 0, arm u 7:8 w p pt 7 ps 1, arm  every ::49::61 u 2:3:1 w p pt 7 ps .3 lc palette z           

unset multi

#set yrange [-0.3:0.3]
#set title "x-coord"
#plot arm u 1:2 smooth unique w l

weightsOnce =     "./output_galea/prelearn_weights_once.dat" 

unset key
unset ytics
unset xtics

set multi lay 2,2

set xrange [0:*]
set yrange [0:*]

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
