mkdir -p output_galea
rm output_galea/*.dat
./galea_prl --n=$1 --targetPre1=$2
#gnuplot galea.plot
python galea.plot.py
