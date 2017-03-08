runfile="./pert_prl"

perturb()
{
  pdfSuffix="${1}_bg${2}_cb${3}"
  echo $pdfSuffix
  if [ $useOldData == "0" ]; then
    $runfile --nsessions=$4 --learn_bg=$2 --learn_cb=$3 --pdfSuffix="$pdfSuffix" $1 --seed=2
  fi
  python pert.plot.py "$pdfSuffix"
  #rm -f $calc_dir/*.dat
}

if [ $# -eq 0 ]; then
  echo "Please supply number of sessions"
else

  mkdir -p output_pert
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

  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=0" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=45" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=90" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=135" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=180" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=225" 0 1 $1
  #perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=270" 0 1 $1
  perturb "--numTrialsPre=0 --numTrialsPost=0 --dirShiftInc=2 --endpoint_rotation1=1 --cue_change1=1 --action_change1=1 --dirShift=30 --target_rotation1=1 --targetPre1=315" 0 1 $1
fi
