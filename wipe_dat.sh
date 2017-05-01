p=""
if [ $# -ne 0 ]; then
  p=$1
else
  delay="5.0s"
  echo "You are going to delete ALL data. Are you sure?"
  echo "You have $delay to change your mind"
  sleep $delay
fi
calc_dir=$HOME/tmp_out_calc
#rm -f $calc_dir/${p}*.dat   # if using this, if there are too many files, it does not delete it due to too long argument for rm command
find "$calc_dir" -maxdepth 1 -name "${p}*.dat" -delete
