mkdir -p output_galea
rm output_galea/*.dat
./galea --n=$1
#gnuplot galea.plot
python galea.plot.py
