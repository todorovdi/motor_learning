mkdir -p output_galea
wipe_output.sh galea
./galea --n=$1 --learn_cb=$2
#--targetPre1=$2
#gnuplot galea.plot
python galea.plot.py
