runfile="./pert_prl"
#runfile="./pert"
plotfile="pert.plot.py"

perturb()
{
  pdfSuffix="${1}_bg${2}_cb${3}"
  echo $pdfSuffix
  if [ $useOldData == "0" ]; then
    $runfile --nsessions=$4 --learn_bg=$2 --learn_cb=$3 --pdfSuffix="$pdfSuffix" $1 --seed=$seed
  fi
  python "$plotfile" "$pdfSuffix"
  #rm -f $calc_dir/*.dat
}

perturbAllConfig()
{
  args="$2"
  args_mult3=""
  perturb "$args" 1 0 $1
}

if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
else

  mkdir -p output_pert
  mkdir -p output_pert/single_sess
  calc_dir=$HOME/tmp_out_calc 
  mkdir -p $calc_dir

  useOldData="1"  #by default we use old data
  if [ $# -ge 2 ]; then
    useOldData=$2
  fi

  if [ $useOldData == "0" ]; then
    rm -f output_pert/*.dat
    rm -f $calc_dir/*.dat
  else
    echo "Plotting without recalc"
  fi

  seed=0     #makes <more or less> random seed

  perturbAllConfig $1 "--actcue_change1=1 $p --dirShift=90 $ud" 
fi
