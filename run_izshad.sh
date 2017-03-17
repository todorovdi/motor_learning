make izshad_prl
runfile="./izshad_prl"
plotfile="izshad.plot.py"

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
  args_mult1=""
  args_mult2=""
  args_mult3=""
  if [ $bg_on_cb_on -eq 1 ]; then
    perturb "$args" 1 1 $1
    args_mult1="${args}_bg1_cb1"
  fi
  if [ $bg_off_cb_on -eq 1 ]; then
    perturb "$args" 0 1 $1
    #perturb "$args" 0 1 1
    args_mult2="${args}_bg0_cb1"
  fi
  if [ $bg_on_cb_off -eq 1 ]; then
    perturb "$args" 1 0 $1
    args_mult3="${args}_bg1_cb0"
  fi

  #python $plotfile "$args_mult1" "$args_mult2" "$args_mult3"
}

if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
else

  mkdir -p output_izshad
  mkdir -p output_izshad/single_sess
  calc_dir=$HOME/tmp_out_calc 
  mkdir -p $calc_dir

  useOldData="1"  #by default we use old data
  if [ $# -ge 2 ]; then
    useOldData=$2
  fi

  if [ $useOldData == "0" ]; then
    rm -f output_ishad/*.dat
    rm -f $calc_dir/*.dat
  else
    echo "Plotting without recalc"
  fi

  bg_on_cb_on=1
  bg_off_cb_on=1
  bg_on_cb_off=0

  seed=0     #makes <more or less> random seed

  #p="--Q=0.05" 
  #ud="--updateCBStateDist=0.12"

  perturbAllConfig $1 "--habit2PMCdirectly=0" #$p $ud 

fi
