mkdir -p output_galea
rm output_galea/*.dat
if [ $#=4 ]
then
    ./galea_prl --n=$1 --learn_cb=$2 --cb_learn_rate=$3 --seed=$4
else 
    if [ $#=3 ] 
    then
        ./galea_prl --n=$1 --learn_cb=$2 --cb_learn_rate=$3
    fi
fi
#./galea_prl --n=$1 --targetPre1=$2
#gnuplot galea.plot
python galea.plot.py
