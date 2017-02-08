mkdir -p output_galea
rm output_galea/*.dat
./galea_prl --n=$1
#gnuplot galea.plot
python galea.plot.py
