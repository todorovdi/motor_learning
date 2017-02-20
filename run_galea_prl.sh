mkdir -p output_galea
calc_dir=$HOME/tmp_out_calc 
mkdir -p $calc_dir
rm -f output_galea/*.dat
rm -f $calc_dir/*.dat
if [ $# -eq 4 ]; then
    ./galea_prl --n=$1 --learn_cb=$2 --cb_learn_rate=$3 --seed=$4
elif [ $# -eq 3 ]; then
    ./galea_prl --n=$1 --learn_cb=$2 --cb_learn_rate=$3
elif [ $# -eq 2 ];  then
    ./galea_prl --n=$1 --learn_cb=$2
elif [ $# -eq 1 ];  then
    ./galea_prl --n=$1
fi
#gnuplot galea.plot
python galea.plot.py
