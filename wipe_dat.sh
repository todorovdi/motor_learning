p=""
if [ $# -ne 0 ]; then
  p=$1
fi
calc_dir=$HOME/tmp_out_calc
rm -f $calc_dir/${p}*.dat
