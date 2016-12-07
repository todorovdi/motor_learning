unset key
set grid
set xrange [-.5:.5]
set yrange [0:1]
!make reach && ./reach -i >qqq
load 'qqq'
pause -1
